

function check_package()
{
    #"libcv2.4  libhighgui2.4  libhighgui-dev libcv-dev  libopencv-contrib-dev libopencv-core-dev libopencv-core2.4v5 libopencv-dev libopencv-gpu-dev libopencv-gpu2.4v5 libopencv-imgproc-dev libopencv-video-dev  libopencv-video2.4v5 libopencv2.4-java libopencv2.4-jni"
    check_list="nmap libboost-all-dev"
    for s in $check_list; do
        apt list --installed $s 2>&1 |grep $s > /dev/null
        if [ "$?" != "0" ]; then
            echo "--> Package $s not installed. Try installing..."
            sudo apt-get install -y $s
        fi
    done
}

check_package

make clean
make all

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./:./lib:./lib/HCNetSDKCom

./ipc
