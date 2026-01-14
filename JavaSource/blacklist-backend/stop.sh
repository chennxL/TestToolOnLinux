#!/bin/bash
if [ -f app.pid ]; then
    kill $(cat app.pid)
    rm app.pid
    echo "✓ Java后端已停止"
else
    echo "未找到PID文件"
fi
