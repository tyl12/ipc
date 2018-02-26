

function check_package()
{
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
