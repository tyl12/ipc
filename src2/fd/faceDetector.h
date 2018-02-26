// faceDetector.h
// This is just the face, eye, smile, profile detector from OpenCV's samples/c directory
//
/* *************** License:**************************
   Jul. 18, 2016
   Author: Liuph
   Right to use this code in any way you want without warranty, support or any guarantee of it working.   

   OTHER OPENCV SITES:
   * The source code is on sourceforge at:
     http://sourceforge.net/projects/opencvlibrary/
   * The OpenCV wiki page (As of Oct 1, 2008 this is down for changing over servers, but should come back):
     http://opencvlibrary.sourceforge.net/
   * An active user group is at:
     http://tech.groups.yahoo.com/group/OpenCV/
   * The minutes of weekly OpenCV development meetings are at:
     http://pr.willowgarage.com/wiki/OpenCV
   ************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <iostream>
using namespace std;


static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
static CvHaarClassifierCascade* nested_cascade = 0;
static CvHaarClassifierCascade* smile_cascade = 0;
static CvHaarClassifierCascade* profile = 0;
int use_nested_cascade = 0;

void detect_and_draw( IplImage* image );


/* The path that stores the trained parameter files.
   After openCv is installed, the file path is 
   "opencv/opencv/sources/data/haarcascades_cuda" or "opencv/opencv/sources/data/haarcascades" */
const char* cascade_name =
    "./haarcascade_frontalface_alt2.xml";
const char* nested_cascade_name =
    "./haarcascade_eye_tree_eyeglasses.xml";
const char* smile_cascade_name = 
	"./haarcascade_smile.xml";
const char* profile_name = 
	"./haarcascade_profileface.xml";
double scale = 1;

int faceDetector(const char* imageName, int nNested, int nSmile, int nProfile)
{
    CvCapture* capture = 0;
    IplImage *frame, *frame_copy = 0;
    IplImage *image = 0;
    const char* scale_opt = "--scale=";
    int scale_opt_len = (int)strlen(scale_opt);
    const char* cascade_opt = "--cascade=";
    int cascade_opt_len = (int)strlen(cascade_opt);
    const char* nested_cascade_opt = "--nested-cascade";
    int nested_cascade_opt_len = (int)strlen(nested_cascade_opt);
	const char* smile_cascade_opt = "--smile-cascade";
	int smile_cascade_opt_len = (int)strlen(smile_cascade_opt);
	const char* profile_opt = "--profile";
	int profile_opt_len = (int)strlen(profile_opt);
    int i;
    const char* input_name = 0;


	int opt_num = 7;
	char** opts = new char*[7];
	opts[0] = "compile_opencv.exe";
	opts[1] = "--scale=1";
	opts[2] = "--cascade=1";
	if (nNested == 1)
		opts[3] = "--nested-cascade=1";
	else
		opts[3] = "--nested-cascade=0";
	if (nSmile == 1)
		opts[4] = "--smile-cascade=1";
	else
		opts[4] = "--smile-cascade=0";
	if (nProfile == 1)
		opts[5] = "--profile=1";
	else
		opts[5] = "--profile=0";
	opts[6] = (char*)imageName;
	


    for( i = 1; i < opt_num; i++ )
    {
        if( strncmp( opts[i], cascade_opt, cascade_opt_len) == 0)
        {
			cout<<"cascade: "<<cascade_name<<endl;
		}
        else if( strncmp( opts[i], nested_cascade_opt, nested_cascade_opt_len ) == 0)
        {
            if( opts[i][nested_cascade_opt_len + 1] == '1')
			{
				cout<<"nested: "<<nested_cascade_name<<endl;
				nested_cascade = (CvHaarClassifierCascade*)cvLoad( nested_cascade_name, 0, 0, 0 );
			}
            if( !nested_cascade )
                fprintf( stderr, "WARNING: Could not load classifier cascade for nested objects\n" );
        }
        else if( strncmp( opts[i], scale_opt, scale_opt_len ) == 0 )
        {
			cout<< "scale: "<< scale<<endl;
            if( !sscanf( opts[i] + scale_opt_len, "%lf", &scale ) || scale < 1 )
                scale = 1;
        }
		else if (strncmp( opts[i], smile_cascade_opt, smile_cascade_opt_len ) == 0)
		{
			if( opts[i][smile_cascade_opt_len + 1] == '1')
			{
				cout<<"smile: "<<smile_cascade_name<<endl;
				smile_cascade = (CvHaarClassifierCascade*)cvLoad( smile_cascade_name, 0, 0, 0 );
			}
			if( !smile_cascade )
				fprintf( stderr, "WARNING: Could not load classifier cascade for smile objects\n" );
		}
		else if (strncmp( opts[i], profile_opt, profile_opt_len ) == 0)
		{
			if( opts[i][profile_opt_len + 1] == '1')
			{
				cout<<"profile: "<<profile_name<<endl;
				profile = (CvHaarClassifierCascade*)cvLoad( profile_name, 0, 0, 0 );
			}
			if( !profile )
				fprintf( stderr, "WARNING: Could not load classifier cascade for profile objects\n" );
		}
        else if( opts[i][0] == '-' )
        {
            fprintf( stderr, "WARNING: Unknown option %s\n", opts[i] );
        }
        else
		{
            input_name = imageName;
			printf("input_name: %s\n", imageName);
		}
    }

    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        fprintf( stderr,
        "Usage: facedetect [--cascade=\"<cascade_path>\"]\n"
        "   [--nested-cascade[=\"nested_cascade_path\"]]\n"
        "   [--scale[=<image scale>\n"
        "   [filename|camera_index]\n" );
        return -1;
    }
    storage = cvCreateMemStorage(0);
    
    if( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0') )
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0' );
    else if( input_name )
    {
        image = cvLoadImage( input_name, 1 );
        if( !image )
            capture = cvCaptureFromAVI( input_name );
    }
    else
        image = cvLoadImage( "../lena.jpg", 1 );

    cvNamedWindow( "result", 1 );

    if( capture )
    {
        for(;;)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );
            if( !frame )
                break;
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frame_copy, 0 );
            else
                cvFlip( frame, frame_copy, 0 );
            
            detect_and_draw( frame_copy );

            if( cvWaitKey( 10 ) >= 0 )
                goto _cleanup_;
        }

        cvWaitKey(0);
_cleanup_:
        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }
    else
    {
        if( image )
        {
            detect_and_draw( image );
            cvWaitKey(0);
            cvReleaseImage( &image );
        }
        else if( input_name )
        {
            /* assume it is a text file containing the
               list of the image filenames to be processed - one per line */
            FILE* f = fopen( input_name, "rt" );
            if( f )
            {
                char buf[1000+1];
                while( fgets( buf, 1000, f ) )
                {
                    int len = (int)strlen(buf), c;
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    printf( "file %s\n", buf ); 
                    image = cvLoadImage( buf, 1 );
                    if( image )
                    {
                        detect_and_draw( image );
                        c = cvWaitKey(0);
                        if( c == 27 || c == 'q' || c == 'Q' )
                            break;
                        cvReleaseImage( &image );
                    }
                }
                fclose(f);
            }
        }
    }
    
    cvDestroyWindow("result");

    return 0;
}

void detect_and_draw( IplImage* img )
{
    static CvScalar colors[] = 
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };

    IplImage *gray, *small_img;
    int i, j;

    gray = cvCreateImage( cvSize(img->width,img->height), 8, 1 );
    small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
                         cvRound (img->height/scale)), 8, 1 );

    cvCvtColor( img, gray, CV_BGR2GRAY );
    cvResize( gray, small_img, CV_INTER_LINEAR );
    cvEqualizeHist( small_img, small_img );
    cvClearMemStorage( storage );

    if( cascade )
    {
        double t = (double)cvGetTickCount();
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
                                            1.1, 2, 0
                                            //|CV_HAAR_FIND_BIGGEST_OBJECT
                                            //|CV_HAAR_DO_ROUGH_SEARCH
                                            |CV_HAAR_DO_CANNY_PRUNING
                                            //|CV_HAAR_SCALE_IMAGE
                                            ,
                                            cvSize(30, 30) );
        t = (double)cvGetTickCount() - t;
        printf( "faces detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            CvMat small_img_roi;
            CvSeq* nested_objects;
			CvSeq* smile_objects;
            CvPoint center;
            CvScalar color = colors[i%8];
            int radius;
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            cvCircle( img, center, radius, color, 3, 8, 0 );

			//eye
            if( nested_cascade != 0)
            {
				cvGetSubRect( small_img, &small_img_roi, *r );
				nested_objects = cvHaarDetectObjects( &small_img_roi, nested_cascade, storage,
					1.1, 2, 0
					//|CV_HAAR_FIND_BIGGEST_OBJECT
					//|CV_HAAR_DO_ROUGH_SEARCH
					//|CV_HAAR_DO_CANNY_PRUNING
					//|CV_HAAR_SCALE_IMAGE
					,
					cvSize(0, 0) );
				for( j = 0; j < (nested_objects ? nested_objects->total : 0); j++ )
				{
					CvRect* nr = (CvRect*)cvGetSeqElem( nested_objects, j );
					center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
					center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
					radius = cvRound((nr->width + nr->height)*0.25*scale);
					cvCircle( img, center, radius, color, 3, 8, 0 );
				}
			}
			//smile
			if (smile_cascade != 0)
			{
				cvGetSubRect( small_img, &small_img_roi, *r );
				smile_objects = cvHaarDetectObjects( &small_img_roi, smile_cascade, storage,
					1.1, 2, 0
					//|CV_HAAR_FIND_BIGGEST_OBJECT
					//|CV_HAAR_DO_ROUGH_SEARCH
					//|CV_HAAR_DO_CANNY_PRUNING
					//|CV_HAAR_SCALE_IMAGE
					,
					cvSize(0, 0) );
				for( j = 0; j < (smile_objects ? smile_objects->total : 0); j++ )
				{
					CvRect* nr = (CvRect*)cvGetSeqElem( smile_objects, j );
					center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
					center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
					radius = cvRound((nr->width + nr->height)*0.25*scale);
					cvCircle( img, center, radius, color, 3, 8, 0 );
				}
			}
        }
    }

	if( profile )
	{
		double t = (double)cvGetTickCount();
		CvSeq* faces = cvHaarDetectObjects( small_img, profile, storage,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			|CV_HAAR_DO_CANNY_PRUNING
			//|CV_HAAR_SCALE_IMAGE
			,
			cvSize(30, 30) );
		t = (double)cvGetTickCount() - t;
		printf( "profile faces detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
		for( i = 0; i < (faces ? faces->total : 0); i++ )
		{
			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			CvMat small_img_roi;
			CvSeq* nested_objects;
			CvSeq* smile_objects;
			CvPoint center;
			CvScalar color = colors[(7-i)%8];
			int radius;
			center.x = cvRound((r->x + r->width*0.5)*scale);
			center.y = cvRound((r->y + r->height*0.5)*scale);
			radius = cvRound((r->width + r->height)*0.25*scale);
			cvCircle( img, center, radius, color, 3, 8, 0 );

			//eye
			if( nested_cascade != 0)
			{
				cvGetSubRect( small_img, &small_img_roi, *r );
				nested_objects = cvHaarDetectObjects( &small_img_roi, nested_cascade, storage,
					1.1, 2, 0
					//|CV_HAAR_FIND_BIGGEST_OBJECT
					//|CV_HAAR_DO_ROUGH_SEARCH
					//|CV_HAAR_DO_CANNY_PRUNING
					//|CV_HAAR_SCALE_IMAGE
					,
					cvSize(0, 0) );
				for( j = 0; j < (nested_objects ? nested_objects->total : 0); j++ )
				{
					CvRect* nr = (CvRect*)cvGetSeqElem( nested_objects, j );
					center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
					center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
					radius = cvRound((nr->width + nr->height)*0.25*scale);
					cvCircle( img, center, radius, color, 3, 8, 0 );
				}
			}
			//smile
			if (smile_cascade != 0)
			{
				cvGetSubRect( small_img, &small_img_roi, *r );
				smile_objects = cvHaarDetectObjects( &small_img_roi, smile_cascade, storage,
					1.1, 2, 0
					//|CV_HAAR_FIND_BIGGEST_OBJECT
					//|CV_HAAR_DO_ROUGH_SEARCH
					//|CV_HAAR_DO_CANNY_PRUNING
					//|CV_HAAR_SCALE_IMAGE
					,
					cvSize(0, 0) );
				for( j = 0; j < (smile_objects ? smile_objects->total : 0); j++ )
				{
					CvRect* nr = (CvRect*)cvGetSeqElem( smile_objects, j );
					center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
					center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
					radius = cvRound((nr->width + nr->height)*0.25*scale);
					cvCircle( img, center, radius, color, 3, 8, 0 );
				}
			}
		}
	}

    cvShowImage( "result", img );
    cvReleaseImage( &gray );
    cvReleaseImage( &small_img );
}
