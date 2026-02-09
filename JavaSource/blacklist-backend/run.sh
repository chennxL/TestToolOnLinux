#!/bin/bash
echo "=========================================="
echo "  启动Java后端"
echo "=========================================="
# 检查jar包是否存在
if [ ! -f target/blacklist-test-tool-1.0.0.jar ]; then
    echo "错误: jar包不存在,请先运行 ./build.sh 编译项目"
    exit 1
fi
# 停止旧进程
if [ -f app.pid ]; then
    OLD_PID=$(cat app.pid)
    if ps -p $OLD_PID > /dev/null 2>&1; then
        echo "停止旧进程 (PID: $OLD_PID)..."
        kill $OLD_PID
        sleep 2
    fi
fi
# 启动新进程
echo "启动Java后端..."
nohup java -Xms4g -Xmx6g -XX:+UseG1GC -XX:MaxGCPauseMillis=200 \
    -XX:NativeMemoryTracking=detail \
    -XX:+UnlockDiagnosticVMOptions \
    -jar target/blacklist-test-tool-1.0.0.jar \
    --spring.profiles.active=prod > application.log 2>&1 &
echo $! > app.pid
echo "✓ Java后端已启动 (PID: $(cat app.pid))"
echo ""
echo "查看日志: tail -f application.log"
echo "查看内存: jcmd \$(cat app.pid) VM.native_memory summary"
