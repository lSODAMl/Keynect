#include "KinectController.h"
#include <fstream>

using namespace cv;

KinectController::KinectController()
{

}

KinectController::~KinectController()
{

}

void KinectController::InitKinect()
{
	HRESULT hr;

	//get sensor
	hr = GetDefaultKinectSensor(kModel.KinectSensorAddress());
	if (FAILED(hr))
	{
		printf("Failed to find sensor!\n");
		exit(10);
	}

	//get the depth frame source & depth frame reader
	if (kModel.KinectSensor())
	{
		IDepthFrameSource* pDepthFrameSource = NULL;

		hr = kModel.KinectSensor()->Open();

		if (SUCCEEDED(hr))
		{
			hr = kModel.KinectSensor()->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameSource->OpenReader(kModel.DepthFrameReaderAddress());
		}

		//release depth frame source
		kModel.SafeRelease(pDepthFrameSource);
	}

	if (!kModel.KinectSensor() || FAILED(hr))
	{
		printf("No ready Device found! \n");
		exit(10);
	}

	kModel.SetAngle(0);
}

UINT16 * KinectController::UpdateKinect()
{
	if (!kModel.DepthFrameReader())
		return false;

	HRESULT hr = kModel.DepthFrameReader()->AcquireLatestFrame(kModel.DepthFrameAddress());

	if (SUCCEEDED(hr))
	{
		UINT nBufferSize = 0;
		UINT16 *pBuffer = NULL;

		if (SUCCEEDED(hr))
			hr = kModel.DepthFrame()->get_FrameDescription(kModel.FrameDescriptionAddress());
		if (SUCCEEDED(hr))
			hr = kModel.FrameDescription()->get_Width(&nWidth);
		if (SUCCEEDED(hr))
			hr = kModel.FrameDescription()->get_Height(&nHeight);
		if (SUCCEEDED(hr))
			hr = kModel.DepthFrame()->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		if (SUCCEEDED(hr))
			hr = kModel.DepthFrame()->get_DepthMinReliableDistance(&nMinDepth);
		if (SUCCEEDED(hr))
			nMaxDepth = USHRT_MAX;
		if (SUCCEEDED(hr))
			return pBuffer;

	}

	return NULL;
}

void KinectController::ChoiceFlattening(bool *flat)
{
	int c = cvWaitKey(10);
	if (c == 'f')
	{
		std::cout << "flattening"<< std::endl;
		*flat = true;
	}
	
	else if (c == 'c')
	{
		std::cout << "cancel" << std::endl;
		*flat = false;
	}

}

void KinectController::MaintainKinect()
{
	kModel.SafeReleaseAddress(kModel.DepthFrame());
	kModel.DepthFrame(NULL);
	kModel.SafeReleaseAddress(kModel.FrameDescription());
	kModel.FrameDescription(NULL);
}

void KinectController::SetDepth(const UINT16* pBuffer, int nWidth, int nHeight, bool flat)
{
	// Make sure we've received valid data
	RGBQUAD* m_pDepthRGBX = kModel.DepthRGBX();

	if (m_pDepthRGBX && pBuffer && (nWidth == kModel.cDepthWidth) && (nHeight == kModel.cDepthHeight))
	{
		RGBQUAD* pRGBX = m_pDepthRGBX;

		// end pixel is start + width*height - 1
		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);
		
		// for cdeptharr
		int i = 0;
		while (pBuffer < pBufferEnd)
		{
			USHORT depth = *pBuffer;
			if (flat == false)
				kModel.cDepthArr[i++] = depth;
			else
				depth -= kModel.cDepthArr[i++];

			SetRGB(pRGBX, depth,flat);
			++pRGBX;
			++pBuffer;
		}
	}

}

int pos[3] = { 0 };
void KinectController::onMouse(int event, int x, int y, int flags, void*param)
{
	if (event == EVENT_LBUTTONDOWN) {
		std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
		pos[0] = x;
		pos[1] = y;
		pos[2] = 1;
	}
}

void KinectController::DecisionStandardDepth(const UINT16* pBuffer)
{
	const UINT16* p = pBuffer;
	UINT16 depth;
	// Make sure we've received valid data
	if (GetRGBX() && pBuffer && (nWidth == GetWidth()) && (nHeight == GetHeight()))
	{
		RGBQUAD* pRGBX = GetRGBX();

		// end pixel is start + width*height - 1
		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

		while (pBuffer < pBufferEnd)
		{
			USHORT depth = *pBuffer;
			BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);
			pRGBX->rgbRed = intensity;
			pRGBX->rgbGreen = intensity;
			pRGBX->rgbBlue = intensity;

			++pRGBX;
			++pBuffer;
		}

		// Draw the data with OpenCV
		Mat ForCalculateImage(nHeight, nWidth, CV_8UC4, GetRGBX());

		Mat show = ForCalculateImage.clone();
		imshow("DecisionStandardDepth", show);

		setMouseCallback("DecisionStandardDepth", onMouse, (void*)&show);

		if (pos[2] == 1) {
			depth = p[pos[0] + pos[1] * 512];
			SetStandardDepth(depth);
			std::cout << "standard depth : " << GetStandardDepth() << std::endl;
			pos[2] = 0;
		}

	}
}

void KinectController::SetRGB(RGBQUAD* pRGBX, USHORT depth, bool flat)
{

	UINT std_depth = GetStandardDepth();
	if (flat == true)
		depth += std_depth;


	if (std_depth < 600)
		std_depth = 600;
	if (depth < std_depth - 105)
	{
		pRGBX->rgbBlue = 255;
		pRGBX->rgbGreen = 255;
		pRGBX->rgbRed = 255;
	}

	else if (depth >= std_depth - 105 && depth < std_depth - 90)
	{
		pRGBX->rgbBlue = 206;
		pRGBX->rgbGreen = 206;
		pRGBX->rgbRed = 206;
	}

	else if (depth >= std_depth - 90 && depth < std_depth - 75)
	{
		pRGBX->rgbBlue = 161;
		pRGBX->rgbGreen = 161;
		pRGBX->rgbRed = 161;
	}

	else if (depth >= std_depth - 75 && depth < std_depth - 60)
	{
		pRGBX->rgbBlue = 30;
		pRGBX->rgbGreen = 30;
		pRGBX->rgbRed = 130;
	}
	else if (depth >= std_depth - 60 && depth < std_depth - 45)
	{
		pRGBX->rgbBlue = 0;
		pRGBX->rgbGreen = 67;
		pRGBX->rgbRed = 161;
	}

	else if (depth >= std_depth - 45 && depth < std_depth - 30)
	{
		pRGBX->rgbBlue = 125;
		pRGBX->rgbGreen = 215;
		pRGBX->rgbRed = 232;
	}

	else if (depth >= std_depth - 30 && depth < std_depth - 15)
	{
		pRGBX->rgbBlue = 47;
		pRGBX->rgbGreen = 122;
		pRGBX->rgbRed = 16;
	}

	else if (depth >= std_depth - 15 && depth < std_depth + 15)
	{
		pRGBX->rgbBlue = 71;
		pRGBX->rgbGreen = 97;
		pRGBX->rgbRed = 0;
	}/*
	else if (depth >= std_depth && depth < std_depth + 50)
	{
		pRGBX->rgbBlue = 255;
		pRGBX->rgbGreen = 226;
		pRGBX->rgbRed = 176;
	}*/
	else if (depth >= std_depth + 15 && depth < std_depth + 30)
	{
		pRGBX->rgbBlue = 250;
		pRGBX->rgbGreen = 206;
		pRGBX->rgbRed = 135;
	}

	else if (depth >= std_depth + 30 && depth < std_depth + 45)
	{
		pRGBX->rgbBlue = 205;
		pRGBX->rgbGreen = 140;
		pRGBX->rgbRed = 24;
	}

	else if (depth >= std_depth + 45 && depth < std_depth + 60)
	{
		pRGBX->rgbBlue = 160;
		pRGBX->rgbGreen = 108;
		pRGBX->rgbRed = 19;
	}
	else if (depth >= std_depth + 60 && depth < std_depth + 75)
	{
		pRGBX->rgbBlue = 102;
		pRGBX->rgbGreen = 50;
		pRGBX->rgbRed = 0;
	}
	else if (depth >= std_depth + 75 && depth < std_depth + 90)
	{
		pRGBX->rgbBlue = 100;
		pRGBX->rgbGreen = 30;
		pRGBX->rgbRed = 0;
	}
	else if (depth >= std_depth + 90)
	{
		pRGBX->rgbBlue = 80;
		pRGBX->rgbGreen = 0;
		pRGBX->rgbRed = 0;
	}

}

int KinectController::GetWidth()
{
	return kModel.cDepthWidth;
}

int KinectController::GetHeight()
{
	return kModel.cDepthHeight;
}

RGBQUAD* KinectController::GetRGBX()
{
	return kModel.DepthRGBX();
}

void KinectController::SetStandardDepth(UINT16 depth)
{
	standard_depth = depth; std::cout << depth << std::endl;
}