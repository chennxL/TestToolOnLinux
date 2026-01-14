#!/bin/bash

echo "=========================================="
echo "  黑名单查询测试工具 - 启动脚本"
echo "=========================================="
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 基础目录
BASE_DIR=$(cd "$(dirname "$0")" && pwd)

# 1. 启动Docker服务
echo -e "${YELLOW}[1/3] 启动C++ gRPC服务...${NC}"
cd $BASE_DIR/psigrpc
sudo docker compose up -d

# 检查Docker状态
if sudo docker ps | grep -q psigrpc; then
    echo -e "${GREEN}✓ C++ gRPC服务启动成功${NC}"
else
    echo -e "${RED}✗ C++ gRPC服务启动失败${NC}"
    exit 1
fi

echo ""

# 2. 启动Java后端
echo -e "${YELLOW}[2/3] 启动Java后端服务...${NC}"
cd $BASE_DIR/Java

# 检查是否已经在运行
if [ -f app.pid ] && ps -p $(cat app.pid) > /dev/null 2>&1; then
    echo -e "${YELLOW}Java后端已在运行，跳过启动${NC}"
else
    ./start.sh
    sleep 3
    
    # 检查Java状态
    if [ -f app.pid ] && ps -p $(cat app.pid) > /dev/null 2>&1; then
        echo -e "${GREEN}✓ Java后端启动成功 (PID: $(cat app.pid))${NC}"
    else
        echo -e "${RED}✗ Java后端启动失败${NC}"
        echo "查看日志: tail -f $BASE_DIR/Java/application.log"
        exit 1
    fi
fi

echo ""

# 3. 启动Qt客户端
echo -e "${YELLOW}[3/3] 启动Qt客户端...${NC}"
cd $BASE_DIR/Qtv2/BlacklistToolQt/build

# 设置库路径
export LD_LIBRARY_PATH=../third_party/psiwrapper/lib:../third_party/libpsi/lib:$LD_LIBRARY_PATH

echo -e "${GREEN}✓ 正在启动Qt图形界面...${NC}"
echo ""
echo "=========================================="
echo -e "${GREEN}所有服务已启动！${NC}"
echo "=========================================="
echo ""
echo "服务状态："
echo "  - C++ gRPC: Docker容器 (端口 50051)"
echo "  - Java后端: PID $(cat $BASE_DIR/Java/app.pid) (端口 8080)"
echo "  - Qt客户端: 正在启动图形界面..."
echo ""
echo "查看日志："
echo "  - Java日志: tail -f $BASE_DIR/Java/application.log"
echo "  - Docker日志: sudo docker logs -f psigrpc"
echo ""
echo "停止服务："
echo "  - 运行: $BASE_DIR/stop_all.sh"
echo ""

# 启动Qt客户端（前台运行）
./BlacklistTool
