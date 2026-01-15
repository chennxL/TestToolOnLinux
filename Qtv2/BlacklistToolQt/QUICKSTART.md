# 快速入门指南

## 1. 安装Qt 6

### Windows
1. 下载Qt在线安装器: https://www.qt.io/download-qt-installer
2. 运行安装器,选择以下组件:
   - Qt 6.5.x for MSVC 2019 64-bit
   - Qt Creator
   - CMake
3. 记录安装路径,例如: `C:\Qt\6.5.0\msvc2019_64`

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-base-dev-tools cmake build-essential
```

### macOS
```bash
brew install qt@6 cmake
```

## 2. 编译项目

### 方法1: 使用Qt Creator (推荐)
1. 打开Qt Creator
2. 文件 -> 打开文件或项目
3. 选择 `BlacklistToolQt/CMakeLists.txt`
4. 选择构建套件 (Kit)
5. 点击左下角的"运行"按钮 (绿色三角)

### 方法2: 使用命令行

**Windows:**
1. 修改 `build.bat` 中的Qt路径
2. 双击运行 `build.bat`
3. 运行 `build\BlacklistTool.exe`

**Linux/macOS:**
```bash
chmod +x build.sh
./build.sh
./build/BlacklistTool
```

## 3. 配置后端

在 `src/main.cpp` 中修改后端地址:
```cpp
NetworkRequest::instance().setBaseUrl("http://your-backend:8080");
```

重新编译运行即可。

## 4. 使用说明

### 创建黑名单
1. 点击"创建黑名单"标签
2. 输入规模 (1-50000000)
3. 点击"创建"按钮
4. 等待创建完成

### 创建测试集
1. 点击"创建测试集"标签  
2. 输入库内规模 (0-5000)
3. 输入库外规模 (0-5000)
4. 点击"创建"按钮
5. 创建成功后可以点击"黑名单查询"
6. 查询完成后可以点击"导出"保存Excel文件

### 加密测试
1. 点击"加密测试"标签
2. 输入18位身份证号
3. 点击"加密"按钮
4. 查看加密结果

## 5. 常见问题

**Q: 编译时提示找不到Qt**
A: 
- Qt Creator: 在"项目" -> "构建设置"中选择正确的Qt套件
- 命令行: 修改build.bat中的QT_PATH或在cmake命令中指定CMAKE_PREFIX_PATH

**Q: 运行时提示缺少DLL文件 (Windows)**
A: 
- 方法1: 将Qt安装目录的bin文件夹添加到系统PATH
- 方法2: 使用Qt Creator的"部署"功能
- 方法3: 手动复制缺少的DLL到exe同目录

**Q: 点击按钮没有反应**
A: 检查后端API是否正常运行,查看终端输出的错误信息

**Q: 中文显示为乱码**
A: 确保所有源文件使用UTF-8编码保存

## 6. 开发调试

### 查看网络请求
在 `networkrequest.cpp` 中已经包含了qDebug输出,运行时会在终端显示请求信息。

### 查看状态变化
各个Store类都会发出信号,可以在对应Widget的槽函数中添加qDebug进行调试。

### 调试技巧
1. 使用Qt Creator的调试器 (F5启动调试)
2. 在关键位置添加qDebug() << "变量名:" << 变量值;
3. 使用Qt Creator的"应用程序输出"窗口查看日志

## 7. 项目结构说明

```
BlacklistToolQt/
├── include/           # 所有头文件
├── src/               # 主程序源文件
├── network/           # 网络请求相关
├── stores/            # 状态管理 (类似Pinia)
├── widgets/           # UI组件 (类似Vue组件)
└── utils/             # 工具类
```

每个模块职责清晰,便于维护和扩展。

## 8. 下一步

- 根据需要调整UI布局和样式
- 添加更多错误处理和用户提示
- 实现进度条显示 (大规模数据时)
- 添加配置文件支持
- 实现日志记录功能
