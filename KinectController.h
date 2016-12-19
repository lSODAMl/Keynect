#pragma once
#include <fstream>
#include <iostream>
#include <windows.h>
#include "KinectModel.h"
#include "HandController.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
class KinectController
{
public:
	KinectController();
	~KinectController();

public:
	void		InitKinect();
	UINT16 *	UpdateKinect();
	void		ChoiceFlattening(bool *flat);
	void		MaintainKinect();
	void		SetDepth(const UINT16* pBuffer, int nWidth, int nHeight, bool flat);
	void		SetRGB(RGBQUAD* pRGBX, USHORT depth, bool flat);
	int			GetWidth();
	int			GetHeight();
	RGBQUAD*	GetRGBX();
	UINT16      GetStandardDepth() { return standard_depth; };
	std::ofstream writeDump;

	static void onMouse(int event, int x, int y, int flags, void*param);
	void		DecisionStandardDepth(const UINT16* pBuffer);
	void		SetStandardDepth(UINT16 depth);
	bool		switcherFlag = false;
private:
	KinectModel kModel;
	USHORT		nMaxDepth;
	USHORT		nMinDepth;
	UINT16		standard_depth;
	int			nHeight;
	int			nWidth;

};

