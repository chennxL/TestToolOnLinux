#include "testsetstore.h"
#include "apiservice.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include <QDateTime>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

TestSetStore::TestSetStore(QObject *parent)
    : QObject(parent)
    , m_testSetStatus(NotCreated)
    , m_insideCount(0)
    , m_outsideCount(0)
    , m_queryStatus(NotExecuted)
    , m_matchCount(0)
    , m_totalCount(0)
    , m_queryTime(0.0)
    , m_queryStartTime(0)
{
}

TestSetStore::~TestSetStore()
{
}

TestSetStore& TestSetStore::instance()
{
    static TestSetStore instance;
    return instance;
}

QString TestSetStore::testSetStatusText() const
{
    switch (m_testSetStatus) {
    case NotCreated:
        return "未创建";
    case Creating:
        return "创建中";
    case Created:
        return "已创建";
    case CreateFailed:
        return "创建失败";
    default:
        return "未知";
    }
}

QString TestSetStore::queryStatusText() const
{
    switch (m_queryStatus) {
    case NotExecuted:
        return "未执行";
    case Querying:
        return "查询中";
    case QueryCompleted:
        return "查询完成";
    case QueryFailed:
        return "查询失败";
    default:
        return "未知";
    }
}

void TestSetStore::setTestSetStatus(TestSetStatus status)
{
    if (m_testSetStatus != status) {
        m_testSetStatus = status;
        emit testSetStatusChanged(status);
    }
}

void TestSetStore::setTestSetSize(int inside, int outside)
{
    if (m_insideCount != inside || m_outsideCount != outside) {
        m_insideCount = inside;
        m_outsideCount = outside;
        emit testSetSizeChanged(inside, outside);
    }
}

void TestSetStore::setQueryStatus(QueryStatus status)
{
    if (m_queryStatus != status) {
        m_queryStatus = status;
        emit queryStatusChanged(status);
    }
}

void TestSetStore::setQueryResult(int matched, int total, double time)
{
    m_matchCount = matched;
    m_totalCount = total;
    m_queryTime = time;
    emit queryResultChanged(matched, total, time);
}

void TestSetStore::createTestSet(int insideSize, int outsideSize)
{
    setTestSetStatus(Creating);
    m_pendingInsideSize = insideSize;
    m_pendingOutsideSize = outsideSize;

    // 第一步：调用Java后端生成测试集明文
    ApiService::instance().createTestSet(insideSize, outsideSize,
       [this](const QJsonObject& response) {
           int code = response.value("code").toInt();
           if (code != 200) {
               setTestSetStatus(CreateFailed);
               setTestSetSize(0, 0);
               QString message = response.value("message").toString("生成测试集失败");
               emit testSetCreateFailed(message);
               return;
           }

           // 解析身份证列表
           QJsonArray idCardsArray = response.value("data").toArray();
           QStringList idCards;
           for (const QJsonValue& val : idCardsArray) {
               idCards.append(val.toString());
           }
           qDebug() << "收到测试集数据，数量：" << idCards.size();
           // 🔥 保存原始测试集
           m_originalTestSet = idCards;

           // 🔥 保存库内身份证集合（最后一位是X的）
           m_insideIdCards.clear();
           for (const QString& idCard : idCards) {
               if (idCard.endsWith('X')) {
                   m_insideIdCards.insert(idCard);
               }
           }
           qDebug() << "库内数量：" << m_insideIdCards.size();
           qDebug() << "库外数量：" << (idCards.size() - m_insideIdCards.size());

           // 🔥 添加详细日志
           qDebug() << "========== Qt收到的测试集 ==========";
           qDebug() << "总数：" << idCards.size();
           for (int i = 0; i < idCards.size(); ++i) {
               QString idCard = idCards[i];
               size_t hash = CryptoWrapper::hashIdCard(idCard);
               qDebug() << QString("[%1] 身份证: %2, 哈希: %3")
                               .arg(i).arg(idCard).arg(hash);
           }
           qDebug() << "====================================";

           // 第二步：Qt端加密数据
           QString contextData;
           QString payloadData;
           bool success = m_cryptoWrapper.encryptIdCards(idCards, contextData, payloadData);
           if (!success) {
               setTestSetStatus(CreateFailed);
               setTestSetSize(0, 0);
               emit testSetCreateFailed("数据加密失败");
               return;
           }

           qDebug() << "数据加密完成";
           qDebug() << "Context大小:" << contextData.size();
           qDebug() << "Payload大小:" << payloadData.size();

           // 第三步：保存到本地内存（不再发送给后端）
           m_cachedContextData = contextData;
           m_cachedPayloadData = payloadData;

           setTestSetStatus(Created);
           setTestSetSize(m_pendingInsideSize, m_pendingOutsideSize);
           setQueryStatus(NotExecuted);
           setQueryResult(0, 0, 0.0);
           emit testSetCreateSuccess();
       },
       [this](const QString& error) {
           setTestSetStatus(CreateFailed);
           setTestSetSize(0, 0);
           emit testSetCreateFailed("生成测试集失败: " + error);
       }
       );
}
void TestSetStore::queryBlacklist()
{
    if (m_cachedContextData.isEmpty() || m_cachedPayloadData.isEmpty()) {
        emit queryFailed("请先创建测试集");
        return;
    }

    setQueryStatus(Querying);
    qDebug() << "开始查询,发送加密数据...";

    // 记录开始时间
    QDateTime startTime = QDateTime::currentDateTime();

    // 调用API发送加密数据进行查询
    ApiService::instance().queryBlacklistWithData(
        m_cachedPayloadData,
        m_cachedContextData,
        [this, startTime](const QJsonObject& response) {
            int code = response.value("code").toInt();
            if (code != 200) {
                setQueryStatus(QueryFailed);
                QString message = response.value("message").toString("查询失败");
                emit queryFailed(message);
                return;
            }

            // 解析加密结果
            QJsonObject data = response.value("data").toObject();
            QString encryptedResult = data.value("encryptedResult").toString();
            if (encryptedResult.isEmpty()) {
                setQueryStatus(QueryFailed);
                emit queryFailed("未收到查询结果");
                return;
            }

            qDebug() << "收到加密结果，大小:" << encryptedResult.size();

            // 🔥 检查是否是分批结果（包含分隔符）
            QString delimiter = "|||BATCH_SEPARATOR|||";
            QVector<MatchedBlacklistInfo> matchedInfoList;

            if (encryptedResult.contains(delimiter)) {
                qDebug() << "检测到分批结果，开始逐批解密...";

                // 分割多个批次的结果
                QStringList batchResults = encryptedResult.split(delimiter, Qt::SkipEmptyParts);
                qDebug() << "批次数量:" << batchResults.size();

                // 逐批解密并合并
                for (int i = 0; i < batchResults.size(); ++i) {
                    qDebug() << "解密批次" << (i + 1) << "/" << batchResults.size();

                    QVector<MatchedBlacklistInfo> batchMatched;
                    bool success = m_cryptoWrapper.decryptResultWithDetails(
                        batchResults[i],
                        batchMatched
                        );

                    if (success) {
                        qDebug() << "批次" << (i + 1) << "解密成功，匹配数:" << batchMatched.size();
                        matchedInfoList.append(batchMatched);
                    } else {
                        qDebug() << "批次" << (i + 1) << "解密失败";
                    }
                }

                qDebug() << "所有批次解密完成，总匹配数:" << matchedInfoList.size();

            } else {
                // 单批次结果，直接解密
                qDebug() << "单批次结果，直接解密";
                bool success = m_cryptoWrapper.decryptResultWithDetails(
                    encryptedResult,
                    matchedInfoList
                    );

                if (!success) {
                    setQueryStatus(QueryFailed);
                    emit queryFailed("解密结果失败");
                    return;
                }
            }

            qDebug() << "========================================";
            qDebug() << "解密成功，匹配信息详情：";
            qDebug() << "----------------------------------------";

            for (int i = 0; i < matchedInfoList.size(); ++i) {
                const auto& info = matchedInfoList[i];
                qDebug() << "匹配[" << i << "]:";
                qDebug() << "  身份证号:" << info.idCard;
                qDebug() << "  行为评级:" << info.riskLevelDesc();
                qDebug() << "  记录数:" << info.recordCount;

                for (int j = 0; j < info.records.size(); ++j) {
                    const auto& record = info.records[j];
                    qDebug() << "  行为记录[" << j << "]: "
                             << record.behaviorTypeDesc() << " + "
                             << record.toolTypeDesc();
                }
            }

            qDebug() << "========================================";

            // 计算耗时
            QDateTime endTime = QDateTime::currentDateTime();
            double elapsedTime = startTime.msecsTo(endTime) / 1000.0;

            // 统计匹配数量
            int matchCount = matchedInfoList.size();
            int totalCount = m_pendingInsideSize + m_pendingOutsideSize;

            setQueryStatus(QueryCompleted);
            setQueryResult(matchCount, totalCount, elapsedTime);

            // 存储匹配信息供导出使用
            m_matchedInfoList = matchedInfoList;

            emit querySuccess();
        },
        [this](const QString& error) {
            setQueryStatus(QueryFailed);
            emit queryFailed("查询失败: " + error);
        }
        );
}

void TestSetStore::exportResults()
{
    // 检查是否有数据
    if (m_originalTestSet.isEmpty()) {
        emit exportFailed("没有可导出的数据，请先创建测试集");
        return;
    }

    if (m_matchedInfoList.isEmpty()) {
        emit exportFailed("没有查询结果，请先执行查询");
        return;
    }

    qDebug() << "开始导出Excel...";
    qDebug() << "原始测试集数量：" << m_originalTestSet.size();
    qDebug() << "匹配数量：" << m_matchedInfoList.size();

    try {
        // 生成文件名
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
        QString fileName = QString("测试集查询结果_%1.xlsx").arg(timestamp);
QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString filePath = documentsPath + "/" + fileName;

        qDebug() << "导出路径：" << filePath;

        // 创建Excel文档
        QXlsx::Document xlsx;

        // 设置列宽
        xlsx.setColumnWidth(1, 8);   // 序号
        xlsx.setColumnWidth(2, 20);  // 身份证号
        xlsx.setColumnWidth(3, 10);  // 库内/库外
        xlsx.setColumnWidth(4, 12);  // 行为评级
        xlsx.setColumnWidth(5, 14);  // 行为记录数
        xlsx.setColumnWidth(6, 12);  // 行为类型
        xlsx.setColumnWidth(7, 12);  // 使用工具

        // 创建表头格式
        QXlsx::Format headerFormat;
        headerFormat.setFontBold(true);
        headerFormat.setFontSize(12);
        headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        headerFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        headerFormat.setPatternBackgroundColor(QColor(200, 200, 200));

        // 创建数据格式
        QXlsx::Format dataFormat;
        dataFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        // 创建红色字体格式（用于库内数据）
        QXlsx::Format redDataFormat;
        redDataFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        redDataFormat.setFontColor(QColor(Qt::red));
        // 创建红色字体居中格式（用于库内数据）
        QXlsx::Format redCenterFormat;
        redCenterFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        redCenterFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        redCenterFormat.setFontColor(QColor(Qt::red));
        // 创建居中格式
        QXlsx::Format centerFormat;
        centerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        centerFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);

        // 写入表头
        xlsx.write(1, 1, "序号", headerFormat);
        xlsx.write(1, 2, "身份证号", headerFormat);
        xlsx.write(1, 3, "库内/库外", headerFormat);
        xlsx.write(1, 4, "行为评级", headerFormat);
        xlsx.write(1, 5, "行为记录数", headerFormat);
        xlsx.write(1, 6, "行为类型", headerFormat);
        xlsx.write(1, 7, "使用工具", headerFormat);

        // 构建匹配信息的映射表（身份证号 -> 完整信息）
        QMap<QString, MatchedBlacklistInfo> matchedMap;
        for (const auto& info : m_matchedInfoList) {
            matchedMap[info.idCard] = info;
        }

        int currentRow = 2;  // 从第2行开始写数据
        int sequenceNum = 1; // 序号

        // 遍历原始测试集
        for (const QString& idCard : m_originalTestSet) {
            bool isInside = m_insideIdCards.contains(idCard);

            if (isInside && matchedMap.contains(idCard)) {
                // 库内且匹配的数据 - 使用红色字体
                const MatchedBlacklistInfo& info = matchedMap[idCard];

                int startRow = currentRow;
                int recordCount = info.records.size();

                // 序号（需要合并单元格）- 红色
                if (recordCount > 0) {
                    xlsx.mergeCells(QXlsx::CellRange(startRow, 1, startRow + recordCount - 1, 1), redCenterFormat);
                }
                xlsx.write(startRow, 1, sequenceNum, redCenterFormat);

                // 身份证号（需要合并单元格）- 红色
                if (recordCount > 0) {
                    xlsx.mergeCells(QXlsx::CellRange(startRow, 2, startRow + recordCount - 1, 2), redDataFormat);
                }
                xlsx.write(startRow, 2, idCard, redDataFormat);

                // 库内/库外（需要合并单元格）- 红色
                if (recordCount > 0) {
                    xlsx.mergeCells(QXlsx::CellRange(startRow, 3, startRow + recordCount - 1, 3), redCenterFormat);
                }
                xlsx.write(startRow, 3, "库内", redCenterFormat);

                // 行为评级（需要合并单元格）- 红色
                if (recordCount > 0) {
                    xlsx.mergeCells(QXlsx::CellRange(startRow, 4, startRow + recordCount - 1, 4), redCenterFormat);
                }
                xlsx.write(startRow, 4, info.riskLevelDesc(), redCenterFormat);

                // 行为记录数（需要合并单元格）- 红色
                if (recordCount > 0) {
                    xlsx.mergeCells(QXlsx::CellRange(startRow, 5, startRow + recordCount - 1, 5), redCenterFormat);
                }
                xlsx.write(startRow, 5, info.recordCount, redCenterFormat);

                // 写入每条行为记录 - 红色
                for (int i = 0; i < info.records.size(); ++i) {
                    const BehaviorRecordInfo& record = info.records[i];

                    int row = startRow + i;

                    // 行为类型 - 红色
                    xlsx.write(row, 6, record.behaviorTypeDesc(), redCenterFormat);

                    // 使用工具（只有藏匿才显示）- 红色
                    if (record.behaviorType == 1) {  // 1 = 藏匿
                        xlsx.write(row, 7, record.toolTypeDesc(), redCenterFormat);
                    } else {
                        xlsx.write(row, 7, "", redCenterFormat);  // 其他类型为空
                    }
                }

                currentRow += qMax(recordCount, 1);

            }else {
                // 库外数据或库内但未匹配的数据
                xlsx.write(currentRow, 1, sequenceNum, centerFormat);
                xlsx.write(currentRow, 2, idCard, dataFormat);
                xlsx.write(currentRow, 3, "库外", centerFormat);
                // 其他列留空

                currentRow++;
            }

            sequenceNum++;
        }

        // 保存文件
        if (xlsx.saveAs(filePath)) {
            qDebug() << "Excel导出成功：" << filePath;
            emit exportSuccess(filePath);
        } else {
            qDebug() << "Excel保存失败";
            emit exportFailed("文件保存失败");
        }

    } catch (const std::exception& e) {
        qDebug() << "导出失败：" << e.what();
        emit exportFailed(QString("导出失败: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "导出失败：未知错误";
        emit exportFailed("导出失败: 未知错误");
    }
}
void TestSetStore::reset()
{
    setTestSetStatus(NotCreated);
    setTestSetSize(0, 0);
    setQueryStatus(NotExecuted);
    setQueryResult(0, 0, 0.0);
}
