//openCV配置  
//附加包含目录: include, include/opencv, include/opencv2  
//附加库目录: lib   
//附加依赖项: debug:-->  opencv_calib3d243d.lib;...;  
//          release:--> opencv_calib3d243.lib;...;  

#include<string>  
//#include <opencv2\opencv.hpp>  
//
//#include "CV2_compile.h"  
//#include "CV_compile.h"  
//

#include <stdio.h>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "faceDetector.h"  

using namespace cv;  
using namespace std;  

int main(int argc, char** argv)  
{  
#if 0
    const char* imagename = "lena.jpg";  
    faceDetector(imagename,1,0,0);  
#endif
	while (true) {
        //faceDetector("0",1,0,0);
        faceDetector("rtsp://admin:Marvell12@192.168.10.180:554/ch0/sub/av_stream",1,0,0);
		cv::waitKey(1);
	}

    return 0;  
}  
