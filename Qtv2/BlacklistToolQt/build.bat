@echo off
REM Windows构建脚本
REM 使用前请修改Qt安装路径

echo ========================================
echo 黑名单查询测试工具 - Windows构建脚本
echo ========================================
echo.

REM 设置Qt路径 (请根据实际安装路径修改)
set QT_PATH=C:\Qt\6.5.0\msvc2019_64

REM 检查Qt路径是否存在
if not exist "%QT_PATH%" (
    echo 错误: Qt路径不存在: %QT_PATH%
    echo 请修改脚本中的QT_PATH变量
    pause
    exit /b 1
)

REM 创建构建目录
if not exist build (
    mkdir build
    echo 已创建build目录
)

cd build

echo 正在配置CMake...
cmake .. -DCMAKE_PREFIX_PATH="%QT_PATH%" -G "NMake Makefiles"

if %ERRORLEVEL% NEQ 0 (
    echo CMake配置失败
    cd ..
    pause
    exit /b 1
)

echo.
echo 正在编译项目...
nmake

if %ERRORLEVEL% NEQ 0 (
    echo 编译失败
    cd ..
    pause
    exit /b 1
)

echo.
echo ========================================
echo 编译成功!
echo 可执行文件位置: build\BlacklistTool.exe
echo ========================================
echo.

cd ..
pause
