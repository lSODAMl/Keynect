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
		else
		{
			std::cout << "pBuffer error! in controller" << std::endl;
			return NULL;
		}

		if (SUCCEEDED(hr))
			nMaxDepth = USHRT_MAX;
		if (SUCCEEDED(hr))
			return pBuffer;

	}

	return NULL;
}

void KinectController::ChoiceFlattening(bool *flat)
{
	int c = cvWaitKey(1);
	if (c == 'f')
	{
		std::cout << "flattening" << std::endl;
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
			int depth = *pBuffer;
			if (flat == false)
				kModel.cDepthArr[i++] = depth;
			else
				depth -= (int)kModel.cDepthArr[i++];

			SetRGB(pRGBX, depth, flat);
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

void KinectController::SetRGB(RGBQUAD* pRGBX, int depth, bool flat)
{
	//if (flat == true  + modify && depth != 0)
	//   std::cout << "depth: " << depth << std::endl;
	int modify = -10;

	if (depth >= -9 + modify && depth <  4 + modify)
	{
		pRGBX->rgbBlue = 206;
		pRGBX->rgbGreen = 206;
		pRGBX->rgbRed = 206;
	}
	//회색
	else if (depth >= 4 + modify && depth < 22 + modify)
	{
		pRGBX->rgbBlue = 161;
		pRGBX->rgbGreen = 161;
		pRGBX->rgbRed = 161;
	}
	//검붉은색
	else if (depth >= 22 + modify && depth < 35 + modify)
	{
		pRGBX->rgbBlue = 30;
		pRGBX->rgbGreen = 30;
		pRGBX->rgbRed = 130;
	}
	//갈색
	else if (depth >= 35 + modify && depth < 48 + modify)
	{
		pRGBX->rgbBlue = 0;
		pRGBX->rgbGreen = 67;
		pRGBX->rgbRed = 161;
	}
	//맑은 갈색
	else if (depth >= 48 + modify && depth < 61 + modify)
	{
		pRGBX->rgbBlue = 49;
		pRGBX->rgbGreen = 151;
		pRGBX->rgbRed = 219;
	}
	//연한 황색
	else if (depth >= 61 + modify && depth <  74 + modify)
	{
		pRGBX->rgbBlue = 125;
		pRGBX->rgbGreen = 215;
		pRGBX->rgbRed = 232;
	}
	//풀색
	else if (depth >= 74 + modify && depth <  87 + modify)
	{
		pRGBX->rgbBlue = 23;
		pRGBX->rgbGreen = 185;
		pRGBX->rgbRed = 129;
	}
	//수박색
	else if (depth >= 87 + modify && depth < 100 + modify)
	{
		pRGBX->rgbBlue = 47;
		pRGBX->rgbGreen = 122;
		pRGBX->rgbRed = 16;
	}
	//청록색
	else if (depth >= 100 + modify && depth <  110 + modify)
	{
		pRGBX->rgbBlue = 71;
		pRGBX->rgbGreen = 97;
		pRGBX->rgbRed = 0;
	}
	//하늘색
	else if (depth >= 110 + modify && depth <  120 + modify)
	{
		pRGBX->rgbBlue = 250;
		pRGBX->rgbGreen = 206;
		pRGBX->rgbRed = 135;
	}
	//맑은 파랑
	else if (depth >= 120 + modify && depth <  130 + modify)
	{
		pRGBX->rgbBlue = 205;
		pRGBX->rgbGreen = 140;
		pRGBX->rgbRed = 24;
	}
	//어두운 청록
	else if (depth >= 130 + modify && depth <  140 + modify)
	{
		pRGBX->rgbBlue = 160;
		pRGBX->rgbGreen = 108;
		pRGBX->rgbRed = 19;
	}
	//매우 어두운 청록
	else if (depth >= 140 + modify && depth < 150 + modify)
	{
		pRGBX->rgbBlue = 102;
		pRGBX->rgbGreen = 50;
		pRGBX->rgbRed = 0;
	}
	//어두운 네이비
	else if (depth >= 150 + modify && depth <  160 + modify)
	{
		pRGBX->rgbBlue;
		pRGBX->rgbGreen = 30;
		pRGBX->rgbRed = 0;
	}
	//매우 어두운 네이비
	else if (depth >= 160 + modify && depth <  170 + modify)
	{
		pRGBX->rgbBlue = 80;
		pRGBX->rgbGreen = 0;
		pRGBX->rgbRed = 0;
	}
	else {

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
int KinectController::GetnWidth()
{
	return nWidth;
}
int KinectController::GetnHeight()
{
	return nHeight;
}
RGBQUAD* KinectController::GetRGBX()
{
	return kModel.DepthRGBX();
}

void KinectController::SetStandardDepth(UINT16 depth)
{
	standard_depth = depth; std::cout << depth << std::endl;
}