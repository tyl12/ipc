#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main (int argc, char **argv)
{
#if 0 //ok
	Mat image, image_gray;
	image = imread(argv[1], CV_LOAD_IMAGE_COLOR );
	if (argc != 2 || !image.data) {
		cout << "No image data\n";
		return -1;
	}

	cvtColor(image, image_gray, CV_RGB2GRAY);
	namedWindow("image", CV_WINDOW_AUTOSIZE);
	namedWindow("image gray", CV_WINDOW_AUTOSIZE);

	imshow("image", image);
	imshow("image gray", image_gray);

	waitKey(0);
	return 0;
#endif

#if 0 //ok
	VideoCapture capture(0);
	while(1)
	{
		Mat frame;
		capture >> frame;
		imshow("test",frame);
		waitKey(30);
	}
	return 0;
#endif

#if 1 //ok
	//cv::VideoCapture * stream = new cv::VideoCapture("rtsp://admin:Marvell12@192.168.10.180:554/ch1/main/av_stream");
	//cv::VideoCapture * stream = new cv::VideoCapture("rtsp://admin:Marvell12@192.168.10.180:554/ch1/sub/av_stream");
	cv::VideoCapture * stream = new cv::VideoCapture(0);
	if (!stream->isOpened()) return -1;

	cv::namedWindow("rtsp_stream", CV_WINDOW_AUTOSIZE);
	cv::Mat frame;

	while (true) {

		if (!stream->read(frame)) return -1;

        cout<<"before " <<frame.cols<< "x" << frame.rows <<endl;
        cv::rotate(frame, frame, ROTATE_90_CLOCKWISE);
        cout<<"after " <<frame.cols<< "x" << frame.rows <<endl;

		cv::imshow("rtsp_stream", frame);
		cv::waitKey(15);
	}
#endif

#if 0 //ok
	CvCapture* capture = NULL;
	if ((capture = cvCaptureFromCAM(-1)) == NULL)
	{
		std::cerr << "!!! ERROR: vCaptureFromCAM No camera found\n";
		return -1;
	}
	cout<<"test" <<endl;

/*
	cvNamedWindow("webcam", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("webcam", 50, 50);
	*/

	cvQueryFrame(capture);

	IplImage* src = NULL;
	int cnt = 0;
	for (;;)
	{
		if ((src = cvQueryFrame(capture)) == NULL)
		{
			std::cerr << "!!! ERROR: vQueryFrame\n";
			break;
		}
		cout << "get frame " <<cnt++<<endl;

		// perform processing on src->imageData 


//		cvShowImage("webcam", &src);

		char key_pressed = cvWaitKey(2000);

		if (key_pressed == 27) 
			break;

		cvSaveImage("test.jpeg", src);
	}

	cvReleaseCapture(&capture);

#endif

#if 0
	CvCapture* capture = NULL;
	if ((capture = cvCaptureFromCAM(-1)) == NULL)
	{
		std::cerr << "!!! ERROR: vCaptureFromCAM No camera found\n";
		return -1;
	}
	cout<<"test" <<endl;

	cvNamedWindow("webcam", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("webcam", 50, 50);

	cvQueryFrame(capture);


	IplImage* img=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);

	IplImage* src = NULL;
	int cnt = 0;
	for (;;)
	{
		if ((src = cvQueryFrame(capture)) == NULL)
		{
			std::cerr << "!!! ERROR: vQueryFrame\n";
			break;
		}
		cout << "get frame " <<cnt++<<endl;


/*

		cout<<"----------orig--------------"<<endl;

		cout<<"nSize="<<src->nSize<<endl;
		cout<<"ID="<<src->ID<<endl;
		cout<<"nChannels="<<src->nChannels<<endl;
		cout<<"alphaChannel="<<src->alphaChannel<<endl;
		cout<<"depth="<<src->depth<<endl;
		cout<<"dataOrder="<<src->dataOrder<<endl;
		cout<<"origin="<<src->origin<<endl;
		cout<<"align="<<src->align<<endl;
		cout<<"width="<<src->width<<endl;
		cout<<"height="<<src->height<<endl;
		cout<<"imageSize="<<src->imageSize<<endl;
		//cout<<"imageData="<<src->imageData<<endl;
		cout<<"widthStep="<<src->widthStep<<endl;

		cvCopy(src, img);
		cout<<"----------copy--------------"<<endl;

		cout<<"nSize="<<img->nSize<<endl;
		cout<<"ID="<<img->ID<<endl;
		cout<<"nChannels="<<img->nChannels<<endl;
		cout<<"alphaChannel="<<img->alphaChannel<<endl;
		cout<<"depth="<<img->depth<<endl;
		cout<<"dataOrder="<<img->dataOrder<<endl;
		cout<<"origin="<<img->origin<<endl;
		cout<<"align="<<img->align<<endl;
		cout<<"width="<<img->width<<endl;
		cout<<"height="<<img->height<<endl;
		cout<<"imageSize="<<img->imageSize<<endl;
		//cout<<"imageData="<<img->imageData<<endl;
		cout<<"widthStep="<<img->widthStep<<endl;

*/
		// perform processing on src->imageData 


		cvShowImage("webcam", src);

		char key_pressed = cvWaitKey(10);

		if (key_pressed == 27) 
			break;


		char fileout[20];
		sprintf(fileout, "test%d.jpeg", cnt);
		cvSaveImage(fileout, src);
	}

	cvReleaseCapture(&capture);

#endif


}



