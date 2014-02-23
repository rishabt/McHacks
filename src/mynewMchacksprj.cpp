// mynewMchacksprj.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <windows.h>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>

using namespace std;

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
	double tmpXY[] = {0.0,0.0,0.0};
	
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(imgThresh, moments, 1);

	double moment10 = cvGetSpatialMoment(moments,1,0);
	double moment01 = cvGetSpatialMoment(moments,0,1);
	double area = cvGetCentralMoment(moments,0,0);

	tmpXY[0] = moment10/area;
	tmpXY[1] = moment01/area;
	tmpXY[2] = area;

	XY = tmpXY;		
}


/*void setwindowSettings(bluered* obj){
 cvNamedWindow("Video");
 cvNamedWindow("Ball");
 
 cvCreateTrackbar("LowerH", "Ball", &obj.lowerH, 180, NULL);
        cvCreateTrackbar("UpperH", "Ball", &obj.upperH, 180, NULL);

  cvCreateTrackbar("LowerS", "Ball", &lowerS, 256, NULL);
        cvCreateTrackbar("UpperS", "Ball", &upperS, 256, NULL);

  cvCreateTrackbar("LowerV", "Ball", &lowerV, 256, NULL);
        cvCreateTrackbar("UpperV", "Ball", &upperV, 256, NULL); 
}*/

 
int main(){

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
 blueBall.lowerH=107;
 blueBall.lowerS=232;
 blueBall.lowerV=0;
 blueBall.upperH=151;
 blueBall.upperS=256;
 blueBall.upperV=32;

 vBall redBall = vBall();
 redBall.lastX=0.0;
 redBall.lastY=0.0;
 redBall.lowerH=0;
 redBall.lowerS=147;
 redBall.lowerV=10;
 redBall.upperH=9;
 redBall.upperS=256;
 redBall.upperV=86;
 
  
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
    
  cvShowImage("Ball", imgThresh_BLUE);
  cvShowImage("Ball", imgThresh_RED);
  cvShowImage("Video", frame);

  //setwindowSettings();
  trackObject(imgThresh_BLUE,BLUE_XY);
  trackObject(imgThresh_RED,RED_XY);

  if(BLUE_XY[2]>500){
	  SetCursorPos(BLUE_XY[0],BLUE_XY[1]);
  }

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
}




