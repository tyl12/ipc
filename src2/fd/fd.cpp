#include <opencv2/opencv.hpp>

#include <iostream>  
#include <iterator>  
#include <string>  
#include <stdio.h>  

using namespace std;  
using namespace cv;  

void detectAndDraw( Mat& img, CascadeClassifier& cascade,  
        CascadeClassifier& nestedCascade,  
        double scale, bool tryflip );  

string cascadeName = "haarcascade_frontalface_alt2.xml";  
string nestedCascadeName = "haarcascade_eye_tree_eyeglasses.xml";  

int main( int argc, const char** argv )  
{  
    cv::CascadeClassifier cascade, nestedCascade;  
    double scale = 1;  
    bool tryflip = false;  


	cv::VideoCapture * stream = new cv::VideoCapture("rtsp://admin:Marvell12@192.168.10.180:554/ch0/main/av_stream");

	//cv::VideoCapture * stream = new cv::VideoCapture();
    //stream->open(0);

	if (!stream->isOpened()) return -1;
	cv::Mat frame;

    cascade.load( cascadeName );  
    nestedCascade.load( nestedCascadeName );  

    int cnt=0;
	while (true) {

		if (!stream->read(frame)) return -1;
        cnt++;
        if (cnt%10 == 0)
            detectAndDraw( frame, cascade, nestedCascade, scale, tryflip);  
		cv::waitKey(1);
	}

#if 0
    cv::Mat image = imread( "girls.jpg", 1 );  
    cascade.load( cascadeName );  
    nestedCascade.load( nestedCascadeName );  
    detectAndDraw( image, cascade, nestedCascade, scale, tryflip);  
    cv::waitKey(0);  
#endif
    return 0;  
}  

void detectAndDraw( Mat& img, CascadeClassifier& cascade,  
        CascadeClassifier& nestedCascade,  
        double scale, bool tryflip )  
{  
    int i = 0;  
    double t = 0;  
    vector<Rect> faces, faces2;  
    const static Scalar colors[] =  { CV_RGB(0,0,255),  
        CV_RGB(0,128,255),  
        CV_RGB(0,255,255),  
        CV_RGB(0,255,0),  
        CV_RGB(255,128,0),  
        CV_RGB(255,255,0),  
        CV_RGB(255,0,0),  
        CV_RGB(255,0,255)} ;  
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );  

    cvtColor( img, gray, CV_BGR2GRAY );  
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );  
    equalizeHist( smallImg, smallImg );  
    // 返回栏目页：http://www.bianceng.cn/Programming/cplus/
    t = (double)cvGetTickCount();  
    cascade.detectMultiScale( smallImg, faces,  
            1.1, 2, 0  
            //|CV_HAAR_FIND_BIGGEST_OBJECT  
            |CV_HAAR_DO_ROUGH_SEARCH //效果最好  
            //|CV_HAAR_SCALE_IMAGE  
            //|CV_HAAR_DO_CANNY_PRUNING  
            ,  
            Size(30, 30) );  
    if( tryflip )  
    {  
        flip(smallImg, smallImg, 1); //翻转  
        cascade.detectMultiScale( smallImg, faces2,  
                1.1, 2, 0  
                //|CV_HAAR_FIND_BIGGEST_OBJECT  
                |CV_HAAR_DO_ROUGH_SEARCH  
                //|CV_HAAR_SCALE_IMAGE  
                //|CV_HAAR_DO_CANNY_PRUNING  
                ,  
                Size(30, 30) );  
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )  
        {  
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));  
        }  
    }  
    t = (double)cvGetTickCount() - t;  
    printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );  
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )  
    {  
        Mat smallImgROI;  
        vector<Rect> nestedObjects;  
        Point center;  
        Scalar color = colors[i%8];  
        int radius;  

        double aspect_ratio = (double)r->width/r->height;  
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )  
        {  
            center.x = cvRound((r->x + r->width*0.5)*scale);  
            center.y = cvRound((r->y + r->height*0.5)*scale);  
            radius = cvRound((r->width + r->height)*0.25*scale);  
            circle( img, center, radius, color, 3, 8, 0 );  
        }  
        else
            rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),  
                    cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),  
                    color, 3, 8, 0);  
        if( nestedCascade.empty() )  {
            printf("continue");
            continue;  
        }
        smallImgROI = smallImg(*r);  
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,  
                1.1, 2, 0  
                //|CV_HAAR_FIND_BIGGEST_OBJECT  
                //|CV_HAAR_DO_ROUGH_SEARCH  
                //|CV_HAAR_DO_CANNY_PRUNING  
                |CV_HAAR_SCALE_IMAGE  
                ,  
                Size(30, 30) );  
        for( vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++ )  
        {  
            center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);  
            center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);  
            radius = cvRound((nr->width + nr->height)*0.25*scale);  
            circle( img, center, radius, color, 3, 8, 0 );  
        }  
    }  
    cv::imshow( "result", img );  
}