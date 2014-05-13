#include "WS_Camera.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>

using namespace std;
using namespace cv;

int WS_InitCamera(RaspiCam_Cv *Camera){
	int debug=1;
	if(debug)cout<<"Setting format of output data to CV_8UC1"<<endl;
	Camera->set(CV_CAP_PROP_FORMAT, CV_8UC1);
	Camera->set(CV_CAP_PROP_FPS, 15);
	Camera->set(CV_CAP_PROP_FRAME_WIDTH, 640);
	Camera->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	
	if(debug)cout<<"Opening camera... "<<Camera->open()<<endl;
	/*if(!( Camera->open() ) ){
		if(debug)cout<<"The camera is active"<<endl;
	}
	else{
		cout<<"Error: camera not active"<<endl;
		return -1;
	}	*/
	
	return 0;
}

