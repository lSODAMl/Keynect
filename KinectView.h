#pragma once
#include <windows.h>
#include <Kinect.h>
#include "KinectModel.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

class KinectView
{
public:
	KinectView();
	~KinectView();

public:
	static void ShowScreen(Mat *depthImage);
};

