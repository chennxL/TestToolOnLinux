#!/bin/bash

echo "=========================================="
echo "  黑名单查询测试工具 - 停止脚本"
echo "=========================================="
echo ""

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

BASE_DIR=$(cd "$(dirname "$0")" && pwd)

# 1. 停止Java后端
echo -e "${YELLOW}[1/2] 停止Java后端...${NC}"
cd $BASE_DIR/Java
if [ -f app.pid ]; then
    kill $(cat app.pid) 2>/dev/null
    rm app.pid
    echo -e "${GREEN}✓ Java后端已停止${NC}"
else
    echo "Java后端未运行"
fi

echo ""

# 2. 停止Docker服务
echo -e "${YELLOW}[2/2] 停止C++ gRPC服务...${NC}"
cd $BASE_DIR/psigrpc
sudo docker compose down
echo -e "${GREEN}✓ C++ gRPC服务已停止${NC}"

echo ""
echo "=========================================="
echo -e "${GREEN}所有服务已停止！${NC}"
echo "=========================================="
