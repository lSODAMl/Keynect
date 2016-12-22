#include <Windows.h>
#include "Calibration.h"
#include <cmath>
#include <utility>


std::vector<Point2f> pos(4);
int i;
cv::Mat roiImg;
cv::Mat copyImg;
bool drag = false;
bool flag = false;
bool rs = false;
Point p1, p2;
Point h1, h2, v1, v2;
double s = 1;

Calibration::Calibration()
{
}

Calibration::~Calibration()
{
}

void CallBackFunc1(int event, int x, int y, int flags, void * idx)
{
	static int index = 0;

	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Point is - position (" << x << ", " << y << ")" << endl;
		pos[index] = Point(x, y);
		(index)++;
	}
	if (index == 4)
	{
		flag = true;
		(index) = 0;

	}
	std::cout << "index: " << index << std::endl;
}

Mat Calibration::RoiSetUp(Mat depthImage)
{
	namedWindow("calibImage", WINDOW_NORMAL);
	Mat blackWindow = depthImage.clone();

	int idx = 0;
	setMouseCallback("calibImage", CallBackFunc1, &idx);
	Mat copy;
	if (flag == true)
	{

		double w1 = abs(pos[0].x - pos[1].x);
		double w2 = abs(pos[2].x - pos[3].x);
		double h1 = abs(pos[0].y - pos[2].y);
		double h2 = abs(pos[1].y - pos[3].y);

		double maxWidth = (w1 < w2) ? w2 : w1;
		double maxHeight = (h1 < h2) ? h2 : h1;

		Mat warpImg(Size(maxWidth, maxHeight), (blackWindow).type());

		//Warping 후의 좌표

		vector<Point2f> warpCorners(4);

		warpCorners[0] = Point2f(0, 0);
		warpCorners[1] = Point2f(warpImg.cols, 0);
		warpCorners[2] = Point2f(0, warpImg.rows);
		warpCorners[3] = Point2f(warpImg.cols, warpImg.rows);


		leftTop = Point2f(pos[0].x, pos[0].y);
		rightBottom = Point2f(pos[3].x, pos[3].y);

		//Transformation Matrix 구하기

		Mat trans = getPerspectiveTransform(pos, warpCorners);

		//Warping

		warpPerspective((blackWindow), warpImg, trans, Size(maxWidth, maxHeight));

		copy = warpImg.clone();
		return copy;

	}

	imshow("calibImage", blackWindow);

	return depthImage;
}

Point2f Calibration::GetLeftTop()
{
	return leftTop;
}

Point2f Calibration::GetRightBottom()
{
	return rightBottom;
}