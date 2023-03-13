#!/bin/bash

image_base_name=ubtech/hahamirror
docker_container_name=hahamirror-daemon
docker_newest_image=

# 创建容器前的准备
IID_BUSINE_C_RUNNING=$(docker ps| grep $docker_container_name | awk '{print $1}')
IID_BUSINE_C=$(docker ps -a| grep $docker_container_name | awk '{print $1}')

config_temporary() {
    # 配置环境变量
    xhost +
    export DISPLAY=:0
}

create_docker() {
    config_temporary
    
    echo "开始创建容器==================================="

    docker run -itd -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY -e GDK_SCALE -e GDK_DPI_SCALE  --privileged  -v /dev:/dev -p 9004:9004 --network host --name $docker_container_name  -v /opt/ubtech/hahaMirror/bin:/opt/ubtech/hahaMirror/bin   $docker_newest_image /bin/bash /opt/ubtech/hahaMirror/bin/script/autostart.sh

    echo "创建容器完成==================================="
}

detect_network() {
    service network restart
}


start_docker() {
    detect_timectl

    # 获取最新版本的镜像
    docker_newest_version=$(docker images | grep $image_base_name | sort -t ' ' -k 2 -r | head -n1 | awk '{print $2}')

    docker_newest_image=$image_base_name:$docker_newest_version
    
    echo "docker_newest_image: " $docker_newest_image

    IID_BUSINE_C_RUNNING=$(docker ps| grep $docker_container_name | awk '{print $1}')

    if [ -z "$IID_BUSINE_C_RUNNING" ]; then
        if [ -n "$IID_BUSINE_C" ]; then
            echo "启动旧的容器==================================="
            docker_id=$(echo $IID_BUSINE_C | cut -d' ' -f1 )
            docker start $docker_id
            echo "启动容器完成==================================="
        else
            create_docker
        fi
    else
        echo "已存在启动的hahaMirror容器"
    fi
}

stop_docker() {
    if [[ -n "$IID_BUSINE_C_RUNNING" ]]; then
        echo "开始关闭容器==================================="
        docker_id=$(echo $IID_BUSINE_C_RUNNING | cut -d' ' -f1 )
        docker stop $docker_id
        echo "关闭容器完成==================================="
    else
        echo "没有正在运行的容器"
    fi
}

restart_docker() {
    if [ -n "$IID_BUSINE_C" ]; then
            if [ -n "$IID_BUSINE_C_RUNNING" ]; then
                stop_docker
            fi
            start_docker
    else
        echo "不存在容器"
    fi
}

newstart_docker() {
    echo "创建容器前的操作==start==================================="
	if [ -n "$IID_BUSINE_C_RUNNING" ]; then
		echo "创建容器前的操作：暂停运行该镜像的容器$IID_BUSINE_C_RUNNING容器"
		docker stop $IID_BUSINE_C_RUNNING
	fi
	if [ -n "$IID_BUSINE_C" ]; then
		echo "创建容器前的操作：删除该镜像存在的容器$IID_BUSINE_C容器"
		docker rm $IID_BUSINE_C
	fi
	echo "创建容器前的操作==end==================================="

    create_docker
}

# 检查时间同步服务是否开启
detect_timectl() {
    timectl_running=$(systemctl status systemd-timesyncd.service | grep running)
    if [[ -z $timectl_running ]]; then
        echo "open systemd-timesyncd service"
        sudo systemctl start systemd-timesyncd.service
    fi
}

exe=bin/hahaMirror
kill_exe() {
    pid=$(ps -ef | grep $exe | grep -v 'grep' | awk '{print $2}')
    if [[ ! -z $pid ]]; then
        sudo kill -15 $pid
        sleep 3
    fi
}

case $1 in 
start)
    config_temporary
    start_docker
    ;;
stop)
    stop_docker
    ;;
restart)
    restart_docker
    ;;
newstart)
    newstart_docker
    ;;
kill)
    kill_exe
    ;;
log)
    ;;
esac