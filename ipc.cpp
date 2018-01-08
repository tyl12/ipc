//g++ -std=c++11 ipc.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -pthread

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <iostream>
#ifndef USE_BOOST
#include <regex>
#else
#include <boost/regex.hpp>
#endif
#include <mutex>
#include <thread>
#include <algorithm>

using namespace cv;
using namespace std;
#define MAXLINE 1024
#define DEBUG true

mutex m;

int main(int argc, char** argv)
{


	vector<tuple<string,string>> mac_table;
    mac_table.push_back(make_tuple("b4:a3:82:5e:5f:15", "hc"));
    mac_table.push_back(make_tuple("4c:bd:8f:c6:30:1f", "hc"));
    mac_table.push_back(make_tuple("4c:bd:8f:3f:19:db", "hc"));
    mac_table.push_back(make_tuple("b4:a3:82:66:8b:e6", "hc"));
    mac_table.push_back(make_tuple("b4:a3:82:6a:80:f0", "hc"));
    mac_table.push_back(make_tuple("14:a7:8b:8f:d8:37", "dh"));
	String IPDomain = "192.168.0.0/24";

	char result_buf[MAXLINE];
	vector<tuple<string,string,string>> mac_ip_vendor_table;

    cout<<endl<<"Start to scan local device ip-mac tables....."<<endl;

#if 1
    string nmapcmd = string("sudo nmap -sP ") + IPDomain + " | awk '/Nmap scan report for/{printf $5;}/MAC Address:/{print \" => \"$3;}' | sort";
	FILE* fp = popen(nmapcmd.c_str(), "r");
	//FILE* fp = popen("sudo nmap -sP  192.168.0.0/24 | awk '/Nmap scan report for/{printf $5;}/MAC Address:/{print \" => \"$3;}' | sort", "r");
	if(NULL == fp)
	{
		cerr<<"fail to execute arp -a"<<endl;
		exit(1);
	}
	while(fgets(result_buf, sizeof(result_buf), fp) != NULL)
	{
		int len=strlen(result_buf);
		if (result_buf[len-1] == '\n'){
			result_buf[len-1]=0;
		}
		if (DEBUG){
			cout<<"Try search arp result: "<<result_buf<<endl;
		}
		string str=result_buf;
		transform(str.begin(), str.end(), str.begin(), ::tolower);

		for (auto macvendor:mac_table){
			string mac;
			string vendor;
			tie(mac,vendor)=macvendor;
#ifndef USE_BOOST
			regex re(string("([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+).*=>.*")+mac+".*");
			smatch sm;
			regex_match(str, sm, re);
			if (DEBUG){
				for (auto s:sm){
					cout<<"sm: " << s<<endl;
				}
			}
			if (sm.size() == 2){
				cout<<"ip="<<sm[1]<<" mac="<<mac<<endl;
				mac_ip_vendor_table.push_back(tuple<string,string,string>(sm[1], mac, vendor));
			}
#else
            string mas = string("([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+).*=>.*")+mac+".*";
            boost::regex re(mas);
            boost::smatch sm;
            bool match_ret = boost::regex_match(str, sm, re);
			if (match_ret && sm.size() == 2){
				cout<<"Match: " << "ip="<<sm[1]<<" mac="<<mac<<endl;
				mac_ip_vendor_table.push_back(tuple<string,string,string>(sm[1], mac, vendor));
			}
#endif
		}
	}
#else
    mac_ip_table.push_back(make_tuple("192.168.0.106","b4:a3:82:5e:5f:15","hc"));
    mac_ip_table.push_back(make_tuple("192.168.0.108","b4:a3:82:5e:5f:15","hc"));
    mac_ip_table.push_back(make_tuple("192.168.0.109","4c:bd:8f:c6:30:1f","hc"));
    mac_ip_table.push_back(make_tuple("192.168.0.105","33:bd:8f:c6:30:1f","hc"));
#endif

    cout<<"Scan Done."<<endl<<endl;

    cout<<"Found ip - map mapping:"<<endl;
    for (auto s:mac_ip_vendor_table){
	    string ip;
	    string mac;
	    string vendor;
	    tie(ip,mac,vendor)=s;
	    cout<<"    "<<ip<<" => "<<mac<<" => "<<vendor<<endl;
    }
    cout<<endl;


	vector<thread> ts;
	for (auto s:mac_ip_vendor_table){
		string ip;
		string mac;
		string vendor;
		tie(ip,mac,vendor)=s;

		string rtsp;
		if (vendor == string("dh")){
            cout<<"Launch DaHua IPC"<<endl;
			rtsp = string("rtsp://admin:xxxxxx@" + ip + ":554/cam/realmonitor?channel=1&subtype=0");
		}else{
            cout<<"Launch HaiKang IPC"<<endl;
			rtsp= string("rtsp://admin:xxxxxx@" + ip +":554/ch0/main/av_stream");
		}
		ts.push_back(thread(
			[=](){
				VideoCapture sequence(rtsp);
				if (!sequence.isOpened()){
					cerr << "Failed to open the image sequence!" << endl;
					return 1;
				}

				Mat image;
				namedWindow(ip, 1);

				for(;;){
					sequence >> image;

					if(image.empty()){
						cout << "End of Sequence" << endl;
						break;
					}

                    {
                        lock_guard<mutex> lock(m);
                        imshow(ip, image);
                        waitKey(1);
                    }
				}
			}
		));
	}

	for (auto& t:ts){
		t.join();
	}

	return 0;
}
