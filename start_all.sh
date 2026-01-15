#!/bin/bash

# 1. 启动 Docker
cd /home/test/TestTool/psigrpc
docker compose up -d

# 2. 启动 Java
cd /home/test/TestTool/JavaSource/blacklist-backend
./run.sh &

sleep 5

# 3. 启动 Qt
cd /home/test/TestTool/Qtv2/BlacklistToolQt/build
./run.sh &

echo "所有服务已启动"
