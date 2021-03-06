#pragma once
#include "opencv2\opencv.hpp"
#include "KinectController.h"

using namespace std;
using namespace cv;

class Calibration
{
public:
	Calibration();
	~Calibration();

	Mat RoiSetUp(Mat depthImage);

	Point2f GetLeftTop();
	Point2f GetRightBottom();

private:
	Point2f leftTop;
	Point2f rightBottom;
};