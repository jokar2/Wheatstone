#include "WS_Camera.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <vector>

#define DEBUG 0
#define FPS 15
#define WIDTH 320
#define HEIGHT 240

using namespace std;
using namespace cv;

int WS_InitCamera(RaspiCam_Cv *Camera, int setup){ //Initiates the camera module with settings if "setup" is 1. Opens camera.
    cout<<endl;
	if(setup==1){
		if(DEBUG)cout<<"\tWS_InitCamera: Setting format of output data to CV_8UC1"<<endl;
		Camera->set(CV_CAP_PROP_FORMAT, CV_8UC1); //CV_8UC3 seems to be standard. CV_8UC1 return black and white images (one channels instead of three).
		if(DEBUG)cout<<"\tWS_InitCamera: Setting FPS to "<<FPS<<endl;
		Camera->set(CV_CAP_PROP_FPS, FPS);
		if(DEBUG)cout<<"\tWS_InitCamera: Setting frame dimensions to "<<WIDTH<<"x"<<HEIGHT<<endl;
		Camera->set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
		Camera->set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
	}

	if(Camera->open()!=1){
		cout<<"\tWS_InitCamera: Failed to open camera"<<endl;
		return -1;
	}else{
		cout<<"\tWS_InitCamera: Camera sucessfully opened"<<endl;
	}
	return 1;
}
int WS_Motion_Detection_Vector(RaspiCam_Cv *Camera, int sensitivity)
{
	if(Camera->isOpened()==false)
			WS_InitCamera(Camera,0);

	int number_of_changes=0;  //The amount of pixels that have been changed indicating the magnitude of the motion.
	int changes_in_row=0;   //How many times in a row motion has been detected.

    Mat d1,d2,result;       //Difference 1. The difference between the oldest and the newest frame
    Mat placeHolder;        //Difference 2. The difference between the middle and the newest frame
    vector<Mat> frameVector;  //A vector that will contain the three frames

    if(DEBUG)cout<<"\tWS_Motion_Detection_Vector: Grabbing initial frames"<<endl;
    for(int i=0;i<3;i++) {
        Camera->grab();     //Grabbing an image from the camera placing in a temporary place.
        frameVector.insert(frameVector.begin(), placeHolder);  //Inserting a new entity in the vector
        Camera->retrieve(frameVector.at(0));    //Placing the grabbed image in the new entity.
    }

    if(DEBUG)cout<<"\tWS_Motion_Detection_Vector: Starting the motion detection loop"<<endl;
    while(1){
        if(Camera->isOpened()==false)    //If the camera is not activated, it will be done here.
			WS_InitCamera(Camera,0);

        frameVector.pop_back();     //Te last oldest frame is poped out of the end of the vector.
        Camera->grab();
        frameVector.insert(frameVector.begin(), placeHolder);  //The new fram is inserted in the beginning of the vector.
        Camera->retrieve(frameVector.at(0));

		absdiff(frameVector.at(2), frameVector.at(0), d1);  //Calculating the difference between the frames.
		absdiff(frameVector.at(1), frameVector.at(0), d2);
		bitwise_and(d1,d2,result);                          //Performing the bitwise_and to see the areas that have changed.
		threshold(result, result, 35, 255, CV_THRESH_BINARY); //Thresholding

		for(int j=0;j<HEIGHT;j+=2){     //Looking at any other pixel to count approximately the amount of changes there is.
			for(int i=0;i<WIDTH;i+=2){
				if( ( result.at<uint8_t>(j, i) ) == 255){
					number_of_changes++;
					if(number_of_changes>=sensitivity){  //If motion is detected there is no reason to continue to count the pixels that have been changed.
					    j=HEIGHT;
					    i=WIDTH;
                    }
				}
			}
		}
		if(DEBUG)cout<<"\tWS_Motion_Detection_Vector: motion: "<<number_of_changes<<endl;
		if(number_of_changes>=sensitivity){  //If the number of changes have met the maximum amount defined
			changes_in_row++;

			if(changes_in_row>2){  //If motion have been found in three frames in a row, the function returns 1 indicating that motion is detected.
				if(DEBUG)cout<<"\tWS_Motion_Detection_Vector: Motion detected"<<endl;
				return 1;
			}
		}
		else{
			if(changes_in_row>0)
				changes_in_row=0;
		}
		number_of_changes=0;
    }
	return 0;
}
int WS_Motion_Detection(RaspiCam_Cv *Camera, int sensitivity){ //When motion is detected the function returns 1. The variable "sensitivity" determines how sensitive the motion detection should be.
	if(Camera->isOpened()==false)
			WS_InitCamera(Camera,0);
	int number_of_changes=0;
	int changes_in_row=0;

	Mat *image_ptr, *image_prev_ptr, *image_next_ptr;
	Mat d1,d2,result;
	image_ptr=new Mat;
	image_prev_ptr=new Mat;
	image_next_ptr=new Mat;

	//Initial grab to fill the three frames
	if(DEBUG)cout<<"Grabbing initial frames"<<endl;
	Camera->grab();
	Camera->retrieve(*image_prev_ptr);
	Camera->grab();
	Camera->retrieve(*image_ptr);
	Camera->grab();
	Camera->retrieve(*image_next_ptr);

	if(DEBUG)cout<<"Starting the motion detection loop"<<endl;
	while(1)
	{
		if(Camera->isOpened()==false)
			WS_InitCamera(Camera,0);
		free(image_prev_ptr);
		image_prev_ptr=image_ptr;
		image_ptr=image_next_ptr;
		image_next_ptr=new Mat;
		Camera->grab();
		Camera->retrieve(*image_next_ptr);
		waitKey(1);
		absdiff(*image_prev_ptr, *image_next_ptr, d1);
		absdiff(*image_ptr, *image_next_ptr, d2);
		bitwise_and(d1,d2,result);
		threshold(result, result, 35, 255, CV_THRESH_BINARY);
		for(int j=0;j<HEIGHT;j+=2){
			for(int i=0;i<WIDTH;i+=2){
				if( ( result.at<uint8_t>(j, i) ) == 255){
					number_of_changes++;
				}
			}
		}
		waitKey(1);
		if(DEBUG)cout<<"motion: "<<number_of_changes<<endl;
		if(number_of_changes>sensitivity){
			changes_in_row++;

			if(changes_in_row>2){
				if(DEBUG)cout<<"Motion detected"<<endl;
				return 1;
			}
		}
		else{
			if(changes_in_row>0)
				changes_in_row=0;
		}
		number_of_changes=0;
	}
	return 0;
}
void WS_RecordAndSend(RaspiCam_Cv *Camera){ //Starts the script file ./test.sh that should record with raspivid
	cout<<"Start recording"<<endl;
	Camera->release(); //Releases the camera from openCV so that a video can be recorded in raspivid
	if(fork()==0){
		system("./test.sh"); //Records 5 sec and send the video via ssh to a remote computer
		exit(0);
	}
	else
	{
		sleep(6); //Waits until the 5 sec recording is over + extra sec
	}
}
