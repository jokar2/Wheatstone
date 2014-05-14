#ifndef WS_CAMERA_H_INCLUDED
#define WS_CAMERA_H_INCLUDED

#include <raspicam/raspicam_cv.h>

using namespace raspicam;

int WS_InitCamera(RaspiCam_Cv *Camera,int setup); //Initiates the camera module with settings if "setup" is 1. Opens camera.
int WS_Motion_Detection(RaspiCam_Cv *Camera, int sensitivity); //When motion is detected the function returns 1. The variable "sensitivity" determines how sensitive the motion detection should be.
void WS_RecordAndSend(RaspiCam_Cv *Camera); //Starts the script file ./test.sh that should record with raspivid
#endif
