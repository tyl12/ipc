
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "GetStream.h"
#include "public.h"
#include "ConfigParams.h"
#include "Alarm.h"
#include "CapPicture.h"
#include "playback.h"
#include "Voice.h"
#include "tool.h"
#include "Config.h"

using namespace std;


/**  @fn  void CALLBACK ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
 *   @brief Process exception.
 *   @param (IN) DWORD dwType  
 *   @param (IN) LONG lUserID  
 *   @param (IN) LONG lHandle  
 *   @param (IN) void *pUser  
 *   @return none.  
 */
#if defined(_WIN32)
static void CALLBACK ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
#else
static void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
#endif
{
	printf("ExceptionCallBack lUserID:%d, handle:%d, user data:%p", lUserID, lHandle, pUser);

	char tempbuf[256];
	memset(tempbuf, 0, 256);
	switch(dwType)
	{
		case EXCEPTION_AUDIOEXCHANGE:		//Audio exchange exception
			cout<<"Audio exchange exception!"<<endl;
			//TODO: close audio exchange
			break;

			//Alarm//
		case EXCEPTION_ALARM:			            //Alarm exception
			cout<<"Alarm exception!"<<endl;
			//TODO: close alarm update
			break;
		case EXCEPTION_ALARMRECONNECT:  //Alarm reconnect
			cout<<"Alarm reconnect."<<endl;
			break;
		case ALARM_RECONNECTSUCCESS:      //Alarm reconnect success
			cout<<"Alarm reconnect success."<<endl;
			break;

		case EXCEPTION_SERIAL:			           //Serial exception
			cout<<"Serial exception!"<<endl;
			//TODO: close exception
			break;

			//Preview//
		case EXCEPTION_PREVIEW:			     //Preview exception
			cout<<"Preview exception!"<<endl;
			//TODO: close preview
			break;
		case EXCEPTION_RECONNECT:			 //preview reconnect
			cout<<"preview reconnecting."<<endl;
			break;
		case PREVIEW_RECONNECTSUCCESS: //Preview reconnect success
			cout<<"Preview reconncet success."<<endl;
			break;
		default:
			break;
	}
}


static void setSubStreamConfig(NET_DVR_COMPRESSION_INFO_V30& highPara){
	highPara.byStreamType=0;
	highPara.byResolution=6;//19 for 720p, 16 for vga, 6 for qvga
	highPara.byBitrateType=1;//0 for variable rate, 1 for fix
	highPara.byPicQuality=3;//0 for best, 5 for worst, 0xfe for auto
	highPara.dwVideoBitrate=4;//25 for 4096k
	highPara.dwVideoFrameRate=0;//0 for all, 1 for 1/60, 10 for 10, 14 for 15, 18 for 30
	highPara.wIntervalFrameI=50;

	highPara.byVideoEncType=10;//1 for std h264, 0 for pri h264, 10 for h265
	highPara.byVideoEncComplexity=1; //0 for low, 1 for middle, 2 for high
	highPara.byEnableSvc=0;//0 for disable, 1 for enable, 2 for auto
	highPara.bySmartCodec=0;//0 for disable, 1 for enable
	highPara.byStreamSmooth=50;//1~100, 1 for clear, 100 for smooth
}

static void setMainStreamConfig(NET_DVR_COMPRESSION_INFO_V30& highPara){
	highPara.byStreamType=0;
	highPara.byResolution=19;//19 for 720p, 16 for vga, 6 for qvga
	highPara.byBitrateType=0;//0 for variable rate, 1 for fix
	highPara.byPicQuality=3;//0 for best, 5 for worst, 0xfe for auto
	highPara.dwVideoBitrate=25;//25 for 4096k, 4 for 64k
	highPara.dwVideoFrameRate=10;//0 for all, 1 for 1/60, 10 for 10, 14 for 15, 18 for 30
	highPara.wIntervalFrameI=20;

	highPara.byVideoEncType=1;//1 for std h264, 0 for pri h264, 10 for h265
	highPara.byVideoEncComplexity=1; //0 for low, 1 for middle, 2 for high
	highPara.byEnableSvc=0;//0 for disable, 1 for enable, 2 for auto
	highPara.bySmartCodec=0;//0 for disable, 1 for enable
	highPara.byStreamSmooth=50;//1~100, 1 for clear, 100 for smooth

	//keep other default
	//highPara.byIntervalBPFrame=;
	//highPara.byAudioEncType=;
	//highPara.byFormatType=;
	//highPara.byAudioBitRate=2;//0 for def, 1 for 8k, 2 for 16k, 4 for 64k
	//highPara.byAudioSamplingRate=;
	//highPara.byres=;
	//highPara.wAverageVideoBitrate=;
}

static void showStreamConfig(const NET_DVR_COMPRESSION_INFO_V30& highPara){
	cout<< "*********major config**********" <<endl;
	cout<< "byStreamType= "		<<"\t" 	<< int(highPara.byStreamType)		<<endl;
	cout<< "byResolution= "		<<"\t" 	<< int(highPara.byResolution)		<<endl;
	cout<< "byBitrateType= "	<<"\t" 	<< int(highPara.byBitrateType)		<<endl;
	cout<< "byPicQuality= "		<<"\t" 	<< int(highPara.byPicQuality)		<<endl;
	cout<< "dwVideoBitrate= "	<<"\t" 	<< int(highPara.dwVideoBitrate)		<<endl;
	cout<< "dwVideoFrameRate= "	<<"\t" 	<< int(highPara.dwVideoFrameRate)	<<endl;
	cout<< "wIntervalFrameI= "	<<"\t" 	<< int(highPara.wIntervalFrameI)	<<endl;

	cout<< "byVideoEncType= "	<<"\t" 	<< int(highPara.byVideoEncType)		<<endl;
	cout<< "byVideoEncComplexity= "	<<"\t" 	<< int(highPara.byVideoEncComplexity)	<<endl;
	cout<< "byEnableSvc= "		<<"\t" 	<< int(highPara.byEnableSvc)		<<endl;
	cout<< "bySmartCodec= "		<<"\t" 	<< int(highPara.bySmartCodec)		<<endl;

	cout<< "*********minor config**********" <<endl;
	cout<< "byIntervalBPFrame= " 	<<"\t" 	<< int(highPara.byIntervalBPFrame)	<<endl;
	cout<< "byAudioEncType= " 	<<"\t" 	<< int(highPara.byAudioEncType)		<<endl;
	cout<< "byFormatType= " 	<<"\t" 	<< int(highPara.byFormatType)		<<endl;
	cout<< "byStreamSmooth= " 	<<"\t" 	<< int(highPara.byStreamSmooth)		<<endl;
	cout<< "byAudioBitRate= " 	<<"\t" 	<< int(highPara.byAudioBitRate)		<<endl;
	cout<< "byAudioSamplingRate= " 	<<"\t" 	<< int(highPara.byAudioSamplingRate)	<<endl;
	cout<< "byres= " 		<<"\t" 	<< int(highPara.byres)			<<endl;
	cout<< "wAverageVideoBitrate= " <<"\t" 	<< int(highPara.wAverageVideoBitrate)	<<endl;

}


Config::Config(){
	cout<<"Config ctor"<<endl;
}

Config::~Config(){
	cout<<"~Config"<<endl;
	NET_DVR_Cleanup();  
}

int Config::updateConfig(string ip, int port, string user, string passwd)
{
	LONG lUserID = 0;
	cout<<__LINE__<<endl;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo = {0};
	cout<<__LINE__<<endl;

  	usleep(1000*1000*3);
	cout<<__LINE__<<endl;

	//lUserID = NET_DVR_Login_V30("192.168.1.104", 8000, "admin", "Aim@12345", &struDeviceInfo);  
	lUserID = NET_DVR_Login_V30((char*)ip.c_str(), port, (char*)user.c_str(), (char*)passwd.c_str(), &struDeviceInfo);  

	if (lUserID < 0)  
	{  
		printf("Login error, %d\n", NET_DVR_GetLastError());  
		NET_DVR_Cleanup();  
		return -1;
	}  
  
	//---------------------------------------  
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);  

	//---------------------------------------  
	LONG lRealPlayHandle;  
	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };  
	struPlayInfo.lChannel = 1;           //Ô¤ÀÀÍ¨µÀºÅ  
	//struPlayInfo.hPlayWnd = h;         //ÐèÒªSDK½âÂëÊ±¾ä±úÉèÎªÓÐÐ§Öµ£¬½öÈ¡Á÷²»½âÂëÊ±¿ÉÉèÎª¿Õ  
	//struPlayInfo.dwStreamType = 1;       //0-Ö÷ÂëÁ÷£¬1-×ÓÂëÁ÷£¬2-ÂëÁ÷3£¬3-ÂëÁ÷4£¬ÒÔ´ËÀàÍÆ  
	//struPlayInfo.dwLinkMode = 0;         //0- TCP·½Ê½£¬1- UDP·½Ê½£¬2- ¶à²¥·½Ê½£¬3- RTP·½Ê½£¬4-RTP/RTSP£¬5-RSTP/HTTP  

	int Ret;  
	NET_DVR_COMPRESSIONCFG_V30  struParams = { 0 };  
	DWORD dwReturnLen;  
	Ret = NET_DVR_GetDVRConfig(lUserID, NET_DVR_GET_COMPRESSCFG_V30, struPlayInfo.lChannel, &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30), &dwReturnLen);  

	if (!Ret)  
	{  
		cout << "Failed to get config" << endl;  
		printf("error code: %d\n", NET_DVR_GetLastError());  
	}  
	else  
	{  
		cout<<"----------------------------------------show main-----------------------------------"<<endl;
		showStreamConfig(struParams.struNormHighRecordPara);
		cout<<"----------------------------------------show sub-----------------------------------"<<endl;
		showStreamConfig(struParams.struNetPara);

		cout<<"----------------------------------------update main & sub-----------------------------------"<<endl;
		setMainStreamConfig(struParams.struNormHighRecordPara);
		setSubStreamConfig(struParams.struNetPara);

		cout<<"----------------------------------------set main & sub-----------------------------------"<<endl;
		int SetCamera = NET_DVR_SetDVRConfig(lUserID, NET_DVR_SET_COMPRESSCFG_V30, struPlayInfo.lChannel,  &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30));  
		if (SetCamera)  
		{  
			cout<<"update config success" <<endl;
		}  
	}  
	NET_DVR_Logout(lUserID);  
	return 0;  

}

