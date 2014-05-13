#include "WS_Camera.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

#define DEBUG 1
#define FPS 15
#define WIDTH 640
#define HEIGHT 480

using namespace std;
using namespace cv;

int WS_InitCamera(RaspiCam_Cv *Camera){
	if(DEBUG)cout<<"\tWS_InitCamera: Setting format of output data to CV_8UC3"<<endl;
	Camera->set(CV_CAP_PROP_FORMAT, CV_8UC3); //CV_8UC3 seems to be standard. CV_8UC1 return black and white images (one channels instead of three).
	if(DEBUG)cout<<"\tWS_InitCamera: Setting FPS to "<<FPS<<endl;
	Camera->set(CV_CAP_PROP_FPS, FPS);
	if(DEBUG)cout<<"\tWS_InitCamera: Setting frame dimensions to "<<WIDTH<<"x"<<HEIGHT<<endl;
	Camera->set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
	Camera->set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
	if(DEBUG)cout<<"\tWS_InitCamera: Opening camera... "<<Camera->open()<<endl;

	return 0;
}
