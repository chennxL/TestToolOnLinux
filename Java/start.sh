#!/bin/bash
nohup java -Xms2g -Xmx4g -jar blacklist-test-tool-1.0.0.jar --spring.config.location=application-prod.yml > application.log 2>&1 &
echo $! > app.pid
echo "Java后端已启动，PID: $(cat app.pid)"
echo "查看日志: tail -f application.log"
