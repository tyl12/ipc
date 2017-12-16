//g++ -std=c++11 h.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -pthread

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdio.h>
#include <iostream>
#include <regex>
#include <thread>

using namespace cv;
using namespace std;
#define MAXLINE 1024
#define DEBUG false

int main(int argc, char** argv)
{


	vector<string> mac_table;
	mac_table.push_back("b4:a3:82:5e:5f:15");

	char result_buf[MAXLINE];
	vector<tuple<string,string>> mac_ip_table;

	FILE* fp = popen("arp -a", "r");
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
			cout<<result_buf<<endl;
		}
		string str=result_buf;
		for (auto mac:mac_table){
			regex re(string(".*\\(([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)\\).*")+mac+".*");
			smatch sm;
			regex_match(str, sm, re);
			if (DEBUG){
				for (auto s:sm){
					cout<<"sm: " << s<<endl;
				}
			}
			if (sm.size() == 2){
				cout<<"ip="<<sm[1]<<" mac="<<mac<<endl;
				mac_ip_table.push_back(tuple<string,string>(sm[1], mac));
			}
		}
	}


	vector<thread> ts;
	for (auto s:mac_ip_table){
		string ip;
		string mac;
		tie(ip,mac)=s;

		string rtsp= string("rtsp://admin:xxxxxxxxx@" + ip +":554/ch0/main/av_stream");
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

					imshow(ip, image);
					waitKey(1);
				}
			}
		));
	}

	for (auto& t:ts){
		t.join();
	}

	return 0;
}
