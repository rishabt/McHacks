// mynewMchacksprj.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <windows.h>
#include <cstdlib>
#include <stdlib.h>
#include <iostream>



int lastX = 0;
int lastY = 0;

int lowerH=0;
int lowerS=144;
int lowerV=137;

int upperH=180;
int upperS=244;
int upperV=256;

using namespace std;

//This function threshold the HSV image and create a binary image
IplImage* GetThresholdedImage(IplImage* imgHSV){
 
 IplImage* imgThresh=cvCreateImage(cvGetSize(imgHSV),IPL_DEPTH_8U, 1);
 cvInRangeS(imgHSV, cvScalar(lowerH,lowerS,lowerV), cvScalar(upperH,upperS,upperV), imgThresh); 
 
 return imgThresh;

}

void trackObject(IplImage* imgThresh){

	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments(imgThresh, moments, 1);
	double moment10 = cvGetSpatialMoment(moments,1,0);
	double moment01 = cvGetSpatialMoment(moments,0,1);
	double area = cvGetCentralMoment(moments,0,0);

	if(area>500){

		double posX = moment10/area;
		double posY = moment01/area;

		if(lastX>=0 && lastY>=0 && posX>=0 && posY>=0)
		{
			
		}

		ShowCursor(0);

		SetCursorPos(posX, posY);

		lastX = posX;
		lastY = posY;
	}
}

int main(){
 CvCapture* capture =0; 
 
 capture = cvCaptureFromCAM(0);
 if(!capture){
   printf("Capture failure\n");
   return -1;
 }

        IplImage* frame=0;
  
  //iterate through each frames of the video
 while(true){
 
  frame = cvQueryFrame(capture);
  if(!frame)  break;
  frame=cvCloneImage(frame); 

   IplImage* imgHSV = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3); 
  cvCvtColor(frame, imgHSV, CV_BGR2HSV); //Change the color format from BGR to HSV
   
  IplImage* imgThresh = GetThresholdedImage(imgHSV);
  
  cvShowImage("Ball", imgThresh);
                cvShowImage("Video", frame);

 trackObject(imgThresh);

   //Clean up used images
  cvReleaseImage(&imgHSV);
  cvReleaseImage(&imgThresh);
  cvReleaseImage(&frame);

   //Wait 80mS
  int c = cvWaitKey(10);
  //If 'ESC' is pressed, break the loop
  if((char)c==27 ) break;

 }

  cvDestroyAllWindows();
 cvReleaseCapture(&capture);

       return 0;
}



