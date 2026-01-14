#!/bin/bash
# Linux/macOS构建脚本

echo "========================================"
echo "黑名单查询测试工具 - 构建脚本"
echo "========================================"
echo ""

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "错误: 未找到CMake,请先安装CMake"
    exit 1
fi

# 检查Qt是否安装
if ! command -v qmake &> /dev/null && ! command -v qmake6 &> /dev/null; then
    echo "警告: 未找到qmake,请确保Qt已正确安装"
    echo "你可能需要在CMake配置时指定CMAKE_PREFIX_PATH"
fi

# 创建构建目录
if [ ! -d "build" ]; then
    mkdir build
    echo "已创建build目录"
fi

cd build

echo "正在配置CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake配置失败"
    cd ..
    exit 1
fi

echo ""
echo "正在编译项目..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -ne 0 ]; then
    echo "编译失败"
    cd ..
    exit 1
fi

echo ""
echo "========================================"
echo "编译成功!"
echo "可执行文件位置: build/BlacklistTool"
echo "========================================"
echo ""

cd ..
