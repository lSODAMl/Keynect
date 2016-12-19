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

		// ��źȭ ����
		kController.ChoiceFlattening(&flat);
		// ������ ������Ʈ(Ű��Ʈ�κ��� �������� �޾ƿ´�)
		pBuffer = kController.UpdateKinect();

		temp = frame;

		if (s == '1')
			flag = true;

		else if (s == '2')
		{
			flag = false;

			//frame�� pBuffer���.
			for (int i = 0 ;i < 512 * 424;i++)
			{
				temp[i] = pBuffer[i];
			}
		}

		if (flag == true)
		{
			// 1.������ �Ǵ� ���̸� ����(���콺 Ŭ���� ���� �̷�� ����)
			kController.DecisionStandardDepth(pBuffer);
		}

		else if (flag == false)
		{
			temp = frame;

			//frame�� pBuffer ��, data �ٲ��ֱ�.
			for (int i = 0 ;i < 512 * 424;i++)
			{
				std::cout << "����: " << temp[i] - pBuffer[i] << std::endl;
				if ((temp[i]<pBuffer[i] - 50 || temp[i]>pBuffer[i] + 50) && (temp[i] > pBuffer[i] - 3000 && temp[i] < pBuffer[i] + 3000))
				{
					temp[i] = pBuffer[i];
				}
			}


			// 2.���ȭ�鿡 ������ ������
			if (pBuffer != NULL)
				kController.SetDepth(frame, kController.GetWidth(), kController.GetHeight(), flat);
		}


		// ��Ʈ���� ����
		Mat depthImage(kController.GetHeight(), kController.GetWidth(), CV_8UC4, kController.GetRGBX());
		// ���ϴ� ���� ����
		Mat temp = calib.RoiSetUp(depthImage);
		kView.ShowScreen(&temp);
		// Ű��Ʈ ����
		kController.MaintainKinect();
	}
	return 0;
}