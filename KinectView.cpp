#include "KinectView.h"

using namespace cv;

KinectView::KinectView()
{
}

KinectView::~KinectView()
{
}

void KinectView::ShowScreen(Mat *depthImage)
{
	medianBlur(*depthImage, *depthImage, 5);

	Mat edge, draw;

	Canny(*depthImage, edge, 50, 150, 3);
	edge.convertTo(draw, CV_8U);

	std::vector<std::vector<cv::Point>> contours;
	findContours(draw, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(*depthImage, contours, -1, Scalar(0, 0, 0), 1);

	resize(*depthImage, *depthImage, cv::Size(), 2, 1.811);
	flip(*depthImage, *depthImage, 1);

	Mat show = (*depthImage).clone();
	namedWindow("depthImage", WINDOW_NORMAL);
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