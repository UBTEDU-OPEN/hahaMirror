#!/bin/bash

baseDir=$(pushd $(dirname "$0") >/dev/null; pwd; popd >/dev/null)

closehaha() {
    kill -15 $(ps -ef | grep bin/hahaMirror | gawk '$0 !~/grep/ {print $2}' | tr -s '\n' ' ')
    sleep 30
}

export LD_LIBRARY_PATH=/opt/ubtech/hahaMirror/bin/lib:$LD_LIBRARY_PATH
source /opt/intel/openvino_2020.3.194/bin/setupvars.sh

trap 'closehaha; exit' SIGTERM

while :
do
    ${baseDir}/../service_cli.sh start
    sleep 30
done