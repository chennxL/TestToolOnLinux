#!/bin/bash
echo "=========================================="
echo "  编译Java项目"
echo "=========================================="

# 清理并编译
mvn clean package -DskipTests

# 检查编译结果
if [ $? -eq 0 ]; then
    echo ""
    echo "✓ 编译成功！"
    echo "jar包位置: target/blacklist-test-tool-1.0.0.jar"
else
    echo ""
    echo "✗ 编译失败，请检查错误信息"
    exit 1
fi
