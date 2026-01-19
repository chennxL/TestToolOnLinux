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
#include <QSet>

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
           m_insideIdCardHashes.clear();  // 🆕 清空哈希映射表
           
           qDebug() << "";
           qDebug() << "========== 📊 测试集创建统计 ==========";
           
           int actualInsideCount = 0;
           int actualOutsideCount = 0;
           
           // 统计并保存库内数据
           for (const QString& idCard : idCards) {
               if (idCard.endsWith('X')) {
                   m_insideIdCards.insert(idCard);
                   actualInsideCount++;
                   
                   // 🆕 计算并保存哈希值
                   size_t hash = CryptoWrapper::hashIdCard(idCard);
                   m_insideIdCardHashes[hash] = idCard;
               } else {
                   actualOutsideCount++;
               }
           }
           
           qDebug() << "总数：" << idCards.size();
           qDebug() << "库内（以X结尾）：" << actualInsideCount;
           qDebug() << "库外（非X结尾）：" << actualOutsideCount;
           qDebug() << "====================================";
           qDebug() << "";

           // 🆕 检查哈希冲突
           qDebug() << "========== 🔍 哈希冲突检查 ==========";
           qDebug() << "库内身份证数量：" << m_insideIdCards.size();
           qDebug() << "库内唯一哈希数量：" << m_insideIdCardHashes.size();
           
           if (m_insideIdCards.size() != m_insideIdCardHashes.size()) {
               int collisionCount = m_insideIdCards.size() - m_insideIdCardHashes.size();
               qWarning() << "⚠️ 检测到哈希冲突！冲突数量：" << collisionCount;
               
               // 找出冲突的身份证号
               QMap<size_t, QStringList> hashCollisions;
               for (const QString& idCard : m_insideIdCards) {
                   size_t hash = CryptoWrapper::hashIdCard(idCard);
                   hashCollisions[hash].append(idCard);
               }
               
               qDebug() << "冲突详情：";
               int conflictIndex = 0;
               for (auto it = hashCollisions.begin(); it != hashCollisions.end(); ++it) {
                   if (it.value().size() > 1) {
                       conflictIndex++;
                       qWarning() << QString("  冲突#%1 - 哈希值: %2 (0x%3)")
                                         .arg(conflictIndex)
                                         .arg(it.key())
                                         .arg(it.key(), 0, 16);
                       qWarning() << "    对应" << it.value().size() << "个身份证号：";
                       for (const QString& idCard : it.value()) {
                           qWarning() << "      -" << idCard;
                       }
                   }
               }
           } else {
               qDebug() << "✓ 未检测到哈希冲突";
           }
           qDebug() << "====================================";
           qDebug() << "";

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
    qDebug() << "开始查询，发送加密数据...";

    // 记录开始时间
    QDateTime startTime = QDateTime::currentDateTime();

    // 调用API发送加密数据进行查询
    ApiService::instance().queryBlacklistWithData(
        m_cachedPayloadData,
        m_cachedContextData,
        m_cryptoWrapper.getWeight(),           
        m_cryptoWrapper.getEffectiveLambda(),  
        m_cryptoWrapper.getLogPolyMod(),       
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

            // 解密结果，得到完整的匹配信息列表
            QVector<MatchedBlacklistInfo> matchedInfoList;
            bool decryptSuccess = m_cryptoWrapper.decryptResultWithDetails(
                encryptedResult,
                matchedInfoList
            );

            if (!decryptSuccess) {
                setQueryStatus(QueryFailed);
                emit queryFailed("解密结果失败");
                return;
            }

            qDebug() << "";
            qDebug() << "========================================";
            qDebug() << "解密成功";
            qDebug() << "========================================";
            qDebug() << "";

            // ========== 【修改点1：新增】过滤逻辑 - 只保留库内测试集中的数据 ==========
            qDebug() << "==========  开始过滤匹配结果 ==========";
            qDebug() << "C++返回的总匹配数量：" << matchedInfoList.size();
            qDebug() << "库内测试集数量：" << m_insideIdCards.size();
            
            QVector<MatchedBlacklistInfo> filteredMatchedInfoList;
            int filteredOutCount = 0;
            
            for (const auto& info : matchedInfoList) {
                if (m_insideIdCards.contains(info.idCard)) {
                    // 这是真正的库内数据，保留
                    filteredMatchedInfoList.append(info);
                } else {
                    // 这是误匹配的数据，过滤掉
                    filteredOutCount++;
                    if (filteredOutCount <= 10) {  // 只打印前10个被过滤的
                        size_t hash = CryptoWrapper::hashIdCard(info.idCard);
                        qDebug() << QString("过滤掉误匹配数据[%1]: %2, 哈希: %3 (0x%4)")
                                        .arg(filteredOutCount)
                                        .arg(info.idCard)
                                        .arg(hash)
                                        .arg(hash, 0, 16);
                    }
                }
            }
            
            if (filteredOutCount > 10) {
                qDebug() << "... 还有" << (filteredOutCount - 10) << "条误匹配数据被过滤";
            }
            
            qDebug() << "";
            qDebug() << "过滤后的匹配数量：" << filteredMatchedInfoList.size();
            qDebug() << "被过滤掉的误匹配数量：" << filteredOutCount;
            qDebug() << "==========================================";
            qDebug() << "";
            // ========== 【修改点1 结束】 ==========

            // ========== 【修改点2：修改】使用过滤后的列表进行详细分析 ==========
            qDebug() << "==========  匹配结果详细分析 ==========";
            qDebug() << "有效匹配数量：" << filteredMatchedInfoList.size();
            
            QSet<QString> matchedIdCards;
            QMap<size_t, QString> matchedHashes;
            QMap<QString, int> idCardCount;
            
            // 统计匹配结果
            for (int i = 0; i < filteredMatchedInfoList.size(); ++i) {
                const auto& info = filteredMatchedInfoList[i];
                matchedIdCards.insert(info.idCard);
                idCardCount[info.idCard]++;
                
                size_t hash = CryptoWrapper::hashIdCard(info.idCard);
                matchedHashes[hash] = info.idCard;
                
                // 打印前10条详细信息
                if (i < 10) {
                    qDebug() << QString("匹配[%1]: %2, 哈希: %3 (0x%4), 评级: %5, 记录数: %6")
                                    .arg(i, 4)
                                    .arg(info.idCard)
                                    .arg(hash)
                                    .arg(hash, 0, 16)
                                    .arg(info.riskLevelDesc())
                                    .arg(info.recordCount);
                }
            }
            
            if (filteredMatchedInfoList.size() > 20) {
                qDebug() << "... 中间省略 ...";
                qDebug() << "最后10条：";
                for (int i = qMax(0, filteredMatchedInfoList.size() - 10); i < filteredMatchedInfoList.size(); ++i) {
                    const auto& info = filteredMatchedInfoList[i];
                    size_t hash = CryptoWrapper::hashIdCard(info.idCard);
                    qDebug() << QString("匹配[%1]: %2, 哈希: %3 (0x%4), 评级: %5, 记录数: %6")
                                    .arg(i, 4)
                                    .arg(info.idCard)
                                    .arg(hash)
                                    .arg(hash, 0, 16)
                                    .arg(info.riskLevelDesc())
                                    .arg(info.recordCount);
                }
            }
            
            qDebug() << "";
            qDebug() << "总匹配记录数：" << filteredMatchedInfoList.size();
            qDebug() << "唯一身份证数量：" << matchedIdCards.size();
            qDebug() << "唯一哈希值数量：" << matchedHashes.size();
            qDebug() << "==========================================";
            qDebug() << "";
            
            // 检查是否有重复的身份证号
            if (matchedIdCards.size() != filteredMatchedInfoList.size()) {
                int duplicateCount = filteredMatchedInfoList.size() - matchedIdCards.size();
                qWarning() << "========== ⚠️ 匹配结果中有重复 ==========";
                qWarning() << "重复记录数量：" << duplicateCount;
                
                qDebug() << "重复的身份证号：";
                int dupIndex = 0;
                for (auto it = idCardCount.begin(); it != idCardCount.end(); ++it) {
                    if (it.value() > 1) {
                        dupIndex++;
                        size_t hash = CryptoWrapper::hashIdCard(it.key());
                        qWarning() << QString("  重复#%1: %2 出现 %3 次, 哈希: %4 (0x%5)")
                                          .arg(dupIndex)
                                          .arg(it.key())
                                          .arg(it.value())
                                          .arg(hash)
                                          .arg(hash, 0, 16);
                    }
                }
                qWarning() << "==========================================";
                qWarning() << "";
            }
            
            // 对比发送的库内数据和匹配结果
            qDebug() << "==========  库内数据对比分析 ==========";
            qDebug() << "发送的库内身份证数量：" << m_insideIdCards.size();
            qDebug() << "匹配返回的唯一身份证数量：" << matchedIdCards.size();
            
            int diff = matchedIdCards.size() - m_insideIdCards.size();
            qDebug() << "数量差异：" << diff;
            qDebug() << "";
            
            if (diff != 0) {
                qWarning() << "⚠️ 数量不一致！";
                
                if (diff > 0) {
                    qWarning() << "多出" << diff << "条数据（这不应该发生，因为已经过滤）";
                } else {
                    qWarning() << "少了" << (-diff) << "条数据";
                    qWarning() << "漏掉的库内身份证号：";
                    
                    int missingIndex = 0;
                    for (const QString& idCard : m_insideIdCards) {
                        if (!matchedIdCards.contains(idCard)) {
                            missingIndex++;
                            size_t hash = CryptoWrapper::hashIdCard(idCard);
                            qWarning() << QString("  缺失#%1: %2").arg(missingIndex).arg(idCard);
                            qWarning() << QString("         哈希: %1 (0x%2)").arg(hash).arg(hash, 0, 16);
                            qWarning() << "";
                        }
                    }
                }
            } else {
                qDebug() << "✓ 数量一致";
                
                // 检查身份证号是否完全匹配
                bool allMatch = true;
                for (const QString& idCard : matchedIdCards) {
                    if (!m_insideIdCards.contains(idCard)) {
                        allMatch = false;
                        break;
                    }
                }
                
                if (allMatch) {
                    qDebug() << "✓ 身份证号完全匹配";
                } else {
                    qWarning() << "⚠️ 身份证号不完全一致（这不应该发生）";
                }
            }
            
            qDebug() << "==========================================";
            qDebug() << "";
            // ========== 【修改点2 结束】 ==========

            // 计算耗时
            QDateTime endTime = QDateTime::currentDateTime();
            double elapsedTime = startTime.msecsTo(endTime) / 1000.0;

            // ========== 【修改点3：修改】使用过滤后的数量 ==========
            int matchCount = m_insideIdCards.size();
            int totalCount = m_pendingInsideSize + m_pendingOutsideSize;

            setQueryStatus(QueryCompleted);
            setQueryResult(matchCount, totalCount, elapsedTime);

            // ========== 【修改点4：修改】保存过滤后的匹配信息列表 ==========
            m_matchedInfoList = filteredMatchedInfoList;

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

            } else {
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
