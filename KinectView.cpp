#include "KinectView.h"

using namespace cv;

KinectView::KinectView()
{
}

KinectView::~KinectView()
{
}

void KinectView::ShowScreen(Mat *depthImage)  //67   121.5
{
	//Mat fixedSize = Mat(424/2 * ((768 / 424) * (30.0 / 67)), 512/2 * ((1366 / 512) * (80.5 / 121.5))-100, CV_8UC3, Scalar(0, 0, 0));
	Mat fixedSize = Mat(630, 1000, CV_8UC3, Scalar(0, 0, 0));
	medianBlur(*depthImage, *depthImage, 3);

	/*
	Mat edge, draw;
	Canny(*depthImage, edge, 50, 150, 3);
	edge.convertTo(draw, CV_8U);
	std::vector<std::vector<cv::Point>> contours;
	findContours(draw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(*depthImage, contours, -41, Scalar(0, 0, 0), 1);
	*/
	Size size((int)(512 * 1366 / 512 * 97.5 / 107.5), (int)(424 * 768 / 424 * 47.3 / 60.2));
	resize(*depthImage, *depthImage, size);
	flip(*depthImage, *depthImage, 1);

	Mat show = (*depthImage).clone();
	//namedWindow("depthImage", WINDOW_AUTOSIZE);
	imshow("depthImage", show);



	char ch2 = waitKey(10);
	if (ch2 == 32)                // 32 == SPACE key
	{
		HWND m_hMediaWindow = (HWND)cvGetWindowHandle("depthImage");


		DWORD style = ::GetWindowLong(m_hMediaWindow, GWL_STYLE);
		style &= ~WS_OVERLAPPEDWINDOW;
		style |= WS_POPUP;
		::SetWindowLong(m_hMediaWindow, GWL_STYLE, style);

		HWND hParent = ::FindWindow(0, "depthImage");

		style = ::GetWindowLong(hParent, GWL_STYLE);

		style &= ~WS_OVERLAPPEDWINDOW;

		style |= WS_POPUP;

		::SetWindowLong(hParent, GWL_STYLE, style);

		imshow("depthImage", show);
	}

}