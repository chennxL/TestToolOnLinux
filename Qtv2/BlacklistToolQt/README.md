# 黑名单查询测试工具 (Qt版本)

基于Qt 6的黑名单查询测试工具，从Vue3前端迁移而来。

## 项目结构

```
BlacklistToolQt/
├── CMakeLists.txt          # CMake构建配置
├── include/                # 头文件
│   ├── mainwindow.h
│   ├── networkrequest.h
│   ├── apiservice.h
│   ├── blackliststore.h
│   ├── testsetstore.h
│   ├── createblacklistwidget.h
│   ├── createtestsetwidget.h
│   ├── encryptiontestwidget.h
│   └── messagehelper.h
├── src/                    # 主要源文件
│   ├── main.cpp
│   └── mainwindow.cpp
├── network/                # 网络请求模块
│   ├── networkrequest.cpp
│   └── apiservice.cpp
├── stores/                 # 状态管理模块
│   ├── blackliststore.cpp
│   └── testsetstore.cpp
├── widgets/                # UI组件
│   ├── createblacklistwidget.cpp
│   ├── createtestsetwidget.cpp
│   └── encryptiontestwidget.cpp
└── utils/                  # 工具类
    └── messagehelper.cpp
```

## 环境要求

- **Qt版本**: Qt 6.2 或更高版本
- **编译器**: 
  - Windows: MSVC 2019 或更高版本 / MinGW
  - Linux: GCC 9+ 或 Clang 10+
  - macOS: Xcode 12+
- **CMake**: 3.16 或更高版本
- **C++标准**: C++17

## 编译步骤

### Windows (使用Qt Creator)

1. 安装Qt 6 (推荐使用在线安装器)
2. 打开Qt Creator
3. 打开项目: 文件 -> 打开文件或项目 -> 选择 `CMakeLists.txt`
4. 配置构建套件 (Kit)
5. 点击"构建"按钮

### Windows (使用命令行)

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置CMake (替换为你的Qt安装路径)
cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/6.5.0/msvc2019_64"

# 3. 编译
cmake --build . --config Release

# 4. 运行
Release\BlacklistTool.exe
```

### Linux

```bash
# 1. 安装Qt6依赖
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置和编译
cmake ..
make -j4

# 4. 运行
./BlacklistTool
```

### macOS

```bash
# 1. 安装Qt6 (使用Homebrew)
brew install qt@6

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置和编译
cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
make -j4

# 4. 运行
./BlacklistTool.app/Contents/MacOS/BlacklistTool
```

## 配置后端API地址

在 `src/main.cpp` 中修改API基础URL:

```cpp
// 默认: http://localhost:8080
NetworkRequest::instance().setBaseUrl("http://your-server:port");
```

## API接口说明

### 黑名单API

- **POST** `/blacklist/create` - 创建黑名单
  - 请求体: `{ "size": 1000 }`
  - 响应: `{ "code": 200, "message": "success" }`

- **GET** `/blacklist/status` - 获取黑名单状态

### 测试集API

- **POST** `/testset/create` - 创建测试集
  - 请求体: `{ "insideSize": 100, "outsideSize": 100 }`
  
- **POST** `/testset/query` - 查询黑名单
  - 响应: `{ "code": 200, "data": { "matchCount": 50, "totalCount": 200 } }`

- **GET** `/testset/export` - 导出结果 (返回Excel文件)

### 加密API

- **POST** `/encryption/encrypt` - 加密身份证号
  - 请求体: `{ "idCard": "110101199001011234" }`
  - 响应: `{ "code": 200, "data": "encrypted_string" }`

- **POST** `/encryption/decrypt` - 解密身份证号
  - 请求体: `{ "encryptedIdCard": "encrypted_string" }`
  - 响应: `{ "code": 200, "data": "110101199001011234" }`

## 主要功能模块

### 1. 创建黑名单
- 输入规模 (1-5000万条)
- 创建黑名单数据库
- 显示创建状态和当前规模

### 2. 创建测试集
- 输入库内/库外规模 (各0-5000个)
- 创建测试集并进行同态加密
- 黑名单查询
- 导出查询结果为Excel

### 3. 加密测试
- 输入身份证号
- 调用同态加密接口
- 显示加密结果

## 与Vue3版本的对应关系

| Vue3 | Qt |
|------|-----|
| `<el-tabs>` | `QTabWidget` |
| `<el-input>` | `QLineEdit` |
| `<el-button>` | `QPushButton` |
| `<el-tag>` | `QLabel` + 自定义样式 |
| `ElMessage` | `QMessageBox` |
| Pinia Store | 单例类 + 信号槽 |
| axios | `QNetworkAccessManager` |

## 注意事项

1. **网络请求**: 所有HTTP请求都通过 `NetworkRequest` 单例类管理
2. **状态管理**: 使用单例模式的 `BlacklistStore` 和 `TestSetStore`
3. **信号槽机制**: Widget通过信号槽与Store通信,实现UI自动更新
4. **文件下载**: Excel导出文件保存到系统下载目录

## 常见问题

### Q: 编译时找不到Qt库
A: 确保在CMake配置时指定了正确的 `CMAKE_PREFIX_PATH`

### Q: 运行时崩溃
A: 检查后端API是否正常运行,网络请求是否能正常访问

### Q: 中文显示乱码
A: 确保源文件使用UTF-8编码保存

## 开发团队

- 从Vue3 + Java后端迁移到Qt桌面应用
- 保持与原Vue版本相同的功能和交互逻辑

## 许可证

内部工具，仅供内部使用
