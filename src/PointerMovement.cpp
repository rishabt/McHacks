// mynewMchacksprj.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <windows.h>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <conio.h>
#include "Filter.h"

using namespace std;
using namespace cv;

typedef struct vBall{
	double lastX;
	double lastY;

	double lowerH;
	double lowerS;
	double lowerV;

	double upperH;
	double upperS;
	double upperV;
 };

//This function threshold the HSV image and create a binary image

 IplImage* GetThresholdedImage(IplImage* imgHSV, struct vBall strBall){
 
 IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
 cvInRangeS(imgHSV, cvScalar(strBall.lowerH,strBall.lowerS,strBall.lowerV), cvScalar(strBall.upperH,strBall.upperS,strBall.upperV), imgThresh); 
 
 return imgThresh;
}


static void trackObject(IplImage* imgThresh, double XY[]){
	
	double lastXY[] = {0,0};
		
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(imgThresh, moments, 1);

	double moment10 = cvGetSpatialMoment(moments,1,0);
	double moment01 = cvGetSpatialMoment(moments,0,1);
	double area = cvGetCentralMoment(moments,0,0);

	XY[0] = moment10/area;
	XY[1] = moment01/area;
	XY[2] = area;

}


void setwindowSettings(bluered* obj){
 cvNamedWindow("Video");
 cvNamedWindow("Ball");
 
 cvCreateTrackbar("LowerH", "Ball", &obj.lowerH, 180, NULL);
        cvCreateTrackbar("UpperH", "Ball", &obj.upperH, 180, NULL);

  cvCreateTrackbar("LowerS", "Ball", &lowerS, 256, NULL);
        cvCreateTrackbar("UpperS", "Ball", &upperS, 256, NULL);

  cvCreateTrackbar("LowerV", "Ball", &lowerV, 256, NULL);
        cvCreateTrackbar("UpperV", "Ball", &upperV, 256, NULL); 
}

 
/*int main(){

 CvCapture* capture = 0; 

 capture = cvCaptureFromCAM(0);
 if(!capture){
   printf("Capture failure\n");
   return -1;
 }

 IplImage* frame=0;

 vBall blueBall = vBall();
 blueBall.lastX=0.0;
 blueBall.lastY=0.0;
 blueBall.lowerH=117;
 blueBall.lowerS=250;
 blueBall.lowerV=6;
 blueBall.upperH=180;
 blueBall.upperS=256;
 blueBall.upperV=158;

 vBall redBall = vBall();
 redBall.lastX=0.0;
 redBall.lastY=0.0;
 redBall.lowerH=0;
 redBall.lowerS=252;
 redBall.lowerV=14;
 redBall.upperH=6;
 redBall.upperS=256;
 redBall.upperV=163;
 
  
 double RED_XY[] = {0,0,0};
 double BLUE_XY[] = {0,0,0};
  
  //iterate through each frames of the video
 while(true){
 
  frame = cvQueryFrame(capture);
  if(!frame)  break;
  frame=cvCloneImage(frame); 

  IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3); 
  cvSmooth(frame, frame, CV_GAUSSIAN,3,3);

  cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
  
  IplImage* imgThresh_BLUE = GetThresholdedImage(imgHSV,blueBall);
  IplImage* imgThresh_RED = GetThresholdedImage(imgHSV,redBall);

  cvSmooth(imgThresh_BLUE, imgThresh_BLUE, CV_GAUSSIAN,3,3);
  cvSmooth(imgThresh_RED, imgThresh_RED, CV_GAUSSIAN,3,3);
   
  //cvShowImage("Ball", imgThresh_RED);
  cvShowImage("Ball", imgThresh_BLUE);
  //cvShowImage("Ball", imgThresh_RED);
  cvShowImage("Video", frame);

  //setwindowSettings();
  //trackObject(imgThresh_RED,RED_XY);
  trackObject(imgThresh_BLUE,BLUE_XY);
  //trackObject(imgThresh_RED,RED_XY);

  //if(BLUE_XY[2]>500){
	  SetCursorPos(RED_XY[0],BLUE_XY[1]);
  //}

   //Clean up used images
  cvReleaseImage(&imgHSV);
  cvReleaseImage(&imgThresh_BLUE);
  cvReleaseImage(&imgThresh_RED);
  cvReleaseImage(&frame);

   //Wait 10mS
  int c = cvWaitKey(10);
  //If 'ESC' is pressed, break the loop
  if((char)c==27 ) break;

 }

  cvDestroyAllWindows();
  cvReleaseCapture(&capture);

   return 0;
}*/



Mat getThresholdedImage(Mat img, String color) { 	
	Mat thresholded_red;
	Mat thresholded_blue;

	// Convert the image into an HSV image
	Mat hsvImage_red;
	Mat hsvImage_blue;

	cvtColor(img, hsvImage_red, CV_BGR2HSV);
	cvtColor(img, hsvImage_blue, CV_BGR2HSV);



	inRange(hsvImage_red, Scalar(0, 252, 6), Scalar(6, 256, 163), thresholded_red); // Red Color

    inRange(hsvImage_blue, Scalar(117, 250, 6), Scalar(180, 256, 158), thresholded_blue);// Blue Color

		// Get rid of single pixel errors
	erode(thresholded_red, thresholded_red, Mat(), Point(-1,-1), 2);
	dilate(thresholded_red, thresholded_red, Mat(), Point(-1,-1), 3);

	erode(thresholded_blue, thresholded_blue, Mat(), Point(-1,-1), 2);
	dilate(thresholded_blue, thresholded_blue, Mat(), Point(-1,-1), 3);

	if(color == "red")
		return thresholded_red;

	else
		return thresholded_blue;
}


int main(){
	bool initialized = false;

	// The two windows we'll be using
	namedWindow("video");
	namedWindow("Object");

	VideoCapture camera(0); // open the default camera

	// Make sure camera is setup
	waitKey(1000);


	if(!camera.isOpened())  // check if we succeeded
		return -1;

	do
	{ 
		Mat frame, colorDetect_red, colorDetect_blue;

		// Send data from the camera to the frame
		camera >> frame; // get a new frame from camera

		// This function does the work of finding a specific color
		colorDetect_red = getThresholdedImage( frame, "red" );
		colorDetect_blue = getThresholdedImage( frame, "blue" );

		// This is some magic that turns all the found pixels into a center of gravity location
		cv::Moments moment_red; //moments variable
		moment_red=moments(colorDetect_red,1); //calculate all the moment of image

		cv::Moments moment_blue; //moments variable
		moment_blue=moments(colorDetect_blue,1); //calculate all the moment of image

		double x_red,y_red;
		// Store the center of the pixels
		x_red = moment_red.m10/moment_red.m00;
		y_red = moment_red.m01/moment_red.m00;

		double x_blue,y_blue;
		// Store the center of the pixels
		x_blue = moment_blue.m10/moment_blue.m00;
		y_blue = moment_blue.m01/moment_blue.m00;

		// Taking the x and y values and running them throught the exponential
		// smooting filter, if the if conditional statement is correct
			if ( x >= 0 && y >= 0 )
			{
				 colorLightBlue.methodUpdate(x,y);
			}
			else 
			{
				std::cout<< "x & y are equal to zero" << std::endl;
			}
		//outputting the colors center coordinates:

		cout << x_red << " , " << y_red;
		cout << x_blue << " , " << y_blue;

		// Draw a circle at the center of the red color
		circle(frame, cvPoint(x_red,y_red), 4, cvScalar(0,255,255), 2);

		// Draw a circle at the center of the blue color
	    circle(frame, cvPoint(x_blue,y_blue), 4, cvScalar(0,255,255), 2);

		Mat inputs [2] = {colorDetect_red, colorDetect_blue};

		imshow( "Video", frame);
		imshow("Object", colorDetect_red);
		//imshow("Object", colorDetect_blue);

		SetCursorPos(x_red,y_red);

		if(y_red <= y_blue){
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		}

		else{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}



	} while (waitKey(10) < 0); // waitkey returns -1 if no key pressed and a positive value if pressed

	return 0;
}





