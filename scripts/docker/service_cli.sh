#!/bin/bash

baseDir=$(pushd $(dirname "$0") >/dev/null; pwd; popd >/dev/null)

# 为了程序能够正确的加载模型
cd /opt/ubtech/hahaMirror/bin/

process_name=hahaMirror
pidpath="/tmp/$process_name.pid"
logpath="/tmp/$process_name".log

control_exe_start() {
    processNum=`ps -fe | grep bin/$process_name | grep -v grep | wc -l`
    date > $logpath
    if [ $processNum -eq 0 ];then
        ${baseDir}/${process_name} &

        echo $$ > ${pidpath}
    else
        echo "exe has already running!!"    
    fi
}

control_exe_stop() {
    pid=$(ps -ef | grep bin/$process_name | grep -v "grep" | awk  '{print $2}')
    echo "exe pid to be closed: " $pid
    kill -15 $pid
}

check_exe_status() {
    processNum=`ps -fe | grep bin/$process_name | grep -v grep | wc -l`
    if [ $processNum -eq 0 ];then
        echo "exe is stopped!"
    else
        echo "exe is running"
    fi
}

case "$1" in
    start)
        echo "control exe start"
        control_exe_start
        ;;
    stop)
        echo "control exe stop"
        control_exe_stop
        rm -rf $pidpath
        ;;
    restart)
        echo "control exe restart"
        control_exe_stop
        control_exe_start
        ;;
    status)
        echo "check exe status"
        check_exe_status
        ;;
    *)
    ;;
esac

rm -rf $pidpath
