# 黑名单查询测试工具 - 后端

基于 Spring Boot + MyBatis Plus + MySQL 的黑名单查询测试工具后端项目

## 技术栈

- **框架**: Spring Boot 2.7.18
- **数据库**: MySQL 8.0
- **ORM**: MyBatis Plus 3.5.3
- **加密**: JNA (调用.so库)
- **Excel**: Apache POI 5.2.3
- **工具**: Hutool, Lombok
- **构建**: Maven

## 项目结构

```
blacklist-backend/
├── src/main/java/com/blacklist/
│   ├── BlacklistApplication.java       # 主启动类
│   ├── common/                         # 通用类
│   │   ├── Result.java                 # 统一响应结果
│   │   ├── BusinessException.java      # 业务异常
│   │   └── GlobalExceptionHandler.java # 全局异常处理
│   ├── controller/                     # 控制器
│   │   ├── BlacklistController.java    # 黑名单接口
│   │   └── TestSetController.java      # 测试集接口
│   ├── crypto/                         # 加密相关
│   │   └── CryptoLibrary.java          # JNA接口定义
│   ├── dto/                            # 数据传输对象
│   │   ├── BlacklistStatusDTO.java
│   │   └── QueryResultDTO.java
│   ├── entity/                         # 实体类
│   │   ├── BlacklistMain.java          # 黑名单主表
│   │   ├── BehaviorRecord.java         # 行为记录表
│   │   └── TestSet.java                # 测试集表
│   ├── mapper/                         # Mapper接口
│   │   ├── BlacklistMainMapper.java
│   │   ├── BehaviorRecordMapper.java
│   │   └── TestSetMapper.java
│   ├── service/                        # 服务层
│   │   ├── BlacklistService.java
│   │   ├── TestSetService.java
│   │   └── impl/
│   │       ├── BlacklistServiceImpl.java
│   │       ├── TestSetServiceImpl.java
│   │       └── CryptoService.java      # 加密服务
│   └── util/                           # 工具类
│       ├── IdCardGenerator.java        # 身份证号生成
│       └── BehaviorGenerator.java      # 行为记录生成
├── src/main/resources/
│   ├── application.yml                 # 配置文件
│   └── sql/
│       └── init.sql                    # 数据库初始化脚本
└── pom.xml                             # Maven配置
```

## 快速开始

### 1. 环境要求

- JDK 1.8+
- Maven 3.6+
- MySQL 8.0+
- 同态加密.so库（需提供）

### 2. 数据库配置

#### 创建数据库

```sql
mysql -u root -p
source src/main/resources/sql/init.sql
```

或者在MySQL客户端中执行 `init.sql` 脚本。

#### 修改配置

编辑 `src/main/resources/application.yml`：

```yaml
spring:
  datasource:
    url: jdbc:mysql://localhost:3306/blacklist_test?...
    username: root        # 修改为你的用户名
    password: root        # 修改为你的密码
```

### 3. 配置加密库路径

编辑 `application.yml` 中的加密库路径：

```yaml
blacklist:
  crypto:
    library-path: /usr/local/lib/libcrypto.so  # 修改为实际路径
```

### 4. 构建项目

```bash
# 使用Maven构建
mvn clean package -DskipTests

# 或者使用IDEA直接运行
# 1. 导入Maven项目
# 2. 运行 BlacklistApplication.main()
```

### 5. 启动项目

```bash
# 方式1: 直接运行jar包
java -jar target/blacklist-test-tool-1.0.0.jar

# 方式2: 使用Maven
mvn spring-boot:run

# 方式3: IDEA中直接运行主类
```

启动成功后访问：`http://localhost:8080/api`

## API 接口文档

### 黑名单接口

#### 1. 创建黑名单

```
POST /api/blacklist/create
Content-Type: application/json

{
  "size": 1000000
}

Response:
{
  "code": 200,
  "message": "黑名单创建成功",
  "data": null
}
```

#### 2. 获取黑名单状态

```
GET /api/blacklist/status

Response:
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "status": "已创建",
    "size": 1000000
  }
}
```

### 测试集接口

#### 1. 创建测试集

```
POST /api/testset/create
Content-Type: application/json

{
  "insideSize": 100,
  "outsideSize": 100
}

Response:
{
  "code": 200,
  "message": "测试集创建成功",
  "data": null
}
```

#### 2. 查询黑名单

```
POST /api/testset/query

Response:
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "matchCount": 95,
    "totalCount": 200
  }
}
```

#### 3. 导出查询结果

```
GET /api/testset/export

Response: Excel文件流
```

## 关键功能说明

### 1. 身份证号生成规则

- **库内身份证号**: 前17位正常生成，第18位固定为 `X`
- **库外身份证号**: 前17位正常生成，第18位为 0-9 随机数字
- 格式：`[6位行政区划][8位出生日期][3位顺序码][1位校验码]`

### 2. 同态加密集成

需要对接实际的加密.so库，修改以下文件：

1. **CryptoLibrary.java** - 定义JNA接口，与.so库方法对应
2. **CryptoService.java** - 实现加密/解密/查询逻辑
3. **application.yml** - 配置.so库路径

### 3. 批量数据处理

- 黑名单创建：每批1000条，支持5000万数据
- 测试集创建：支持批量加密
- 查询：支持批量密态查询

### 4. Excel导出

使用Apache POI生成Excel文件，包含：
- 标题行样式（灰色背景、加粗）
- 数据行（身份证号、查询结果）
- 自动列宽
- 时间戳文件名

## 开发说明

### 添加新接口

1. 在 `entity` 包创建实体类
2. 在 `mapper` 包创建Mapper接口
3. 在 `service` 包创建Service接口和实现类
4. 在 `controller` 包创建Controller

### 日志配置

日志输出在控制台，级别配置在 `application.yml`：

```yaml
logging:
  level:
    com.blacklist: debug  # 项目日志级别
```

### 事务处理

使用 `@Transactional` 注解保证数据一致性：

```java
@Transactional(rollbackFor = Exception.class)
public void createBlacklist(Integer size) {
    // 业务逻辑
}
```

## 常见问题

### 1. 数据库连接失败

检查：
- MySQL是否启动
- 用户名密码是否正确
- 数据库是否已创建

### 2. JNA加载.so库失败

检查：
- .so库文件是否存在
- 路径配置是否正确
- 文件权限是否可读

解决方案：
```bash
# 查看.so库依赖
ldd /usr/local/lib/libcrypto.so

# 赋予执行权限
chmod +x /usr/local/lib/libcrypto.so
```

### 3. 内存不足

创建大规模黑名单时可能需要调整JVM参数：

```bash
java -Xms2g -Xmx4g -jar blacklist-test-tool-1.0.0.jar
```

### 4. 端口被占用

修改 `application.yml` 中的端口：

```yaml
server:
  port: 8081  # 修改为其他端口
```

## 性能优化建议

1. **批量插入**: 使用批量插入提高数据库写入效率
2. **索引优化**: 在常用查询字段上创建索引
3. **连接池**: 合理配置Hikari连接池参数
4. **异步处理**: 大规模数据创建可使用异步任务
5. **缓存**: 对频繁查询的数据使用Redis缓存

## 部署

### 生产环境配置

1. 修改 `application-prod.yml`
2. 配置生产数据库连接
3. 调整JVM参数
4. 配置日志文件输出

### 打包部署

```bash
# 打包
mvn clean package -DskipTests

# 运行
java -jar target/blacklist-test-tool-1.0.0.jar --spring.profiles.active=prod
```

## 许可

MIT License
