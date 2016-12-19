#include "KinectController.h"
#include "KinectView.h"
#include "Calibration.h"

int main()
{
	KinectController kController;
	KinectView   kView;
	Calibration calib;
	//HandController hController;

	UINT16* pBuffer;
	UINT16* frame = new UINT16[512 * 424];
	UINT16* temp;


	bool flat = false;
	bool flag = true;
	kController.InitKinect();

	while (1)
	{
		int s = cvWaitKey(1);

		// 평탄화 여부
		kController.ChoiceFlattening(&flat);
		// 데이터 업데이트(키넥트로부터 프레임을 받아온다)
		pBuffer = kController.UpdateKinect();

		temp = frame;

		if (s == '1')
			flag = true;

		else if (s == '2')
		{
			flag = false;

			//frame에 pBuffer담기.
			for (int i = 0 ;i < 512 * 424;i++)
			{
				temp[i] = pBuffer[i];
			}
		}

		if (flag == true)
		{
			// 1.기준이 되는 깊이를 결정(마우스 클릭을 통해 이루어 진다)
			kController.DecisionStandardDepth(pBuffer);
		}

		else if (flag == false)
		{
			temp = frame;

			//frame과 pBuffer 비교, data 바꿔주기.
			for (int i = 0 ;i < 512 * 424;i++)
			{
				std::cout << "차이: " << temp[i] - pBuffer[i] << std::endl;
				if ((temp[i]<pBuffer[i] - 50 || temp[i]>pBuffer[i] + 50) && (temp[i] > pBuffer[i] - 3000 && temp[i] < pBuffer[i] + 3000))
				{
					temp[i] = pBuffer[i];
				}
			}


			// 2.흑백화면에 색깔을 입혀줌
			if (pBuffer != NULL)
				kController.SetDepth(frame, kController.GetWidth(), kController.GetHeight(), flat);
		}


		// 메트릭스 생성
		Mat depthImage(kController.GetHeight(), kController.GetWidth(), CV_8UC4, kController.GetRGBX());
		// 원하는 범위 지정
		Mat temp = calib.RoiSetUp(depthImage);
		kView.ShowScreen(&temp);
		// 키넥트 갱신
		kController.MaintainKinect();
	}
	return 0;
}