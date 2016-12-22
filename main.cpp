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

	bool flat = false;
	bool flag = true;  // ȭ�� ����

	int s = 0, count = 0;

	kController.InitKinect();

	while (1)
	{
		s = cvWaitKey(1);

		// ������ ������Ʈ(Ű��Ʈ�κ��� �������� �޾ƿ´�)
		pBuffer = kController.UpdateKinect();

		if (pBuffer == NULL)
		{
			continue;
		}

		if (s == '1')
			flag = true;

		else if (s == '2')
		{
			flag = false;

			//frame�� pBuffer���.
			for (int i = 0; i < 512 * 424; i++)
			{
				frame[i] = pBuffer[i];
			}
		}

		if (flag == true) //1
		{
			// 1.������ �Ǵ� ���̸� ����(���콺 Ŭ���� ���� �̷�� ����)
			kController.DecisionStandardDepth(pBuffer);
		}

		else if (flag == false)//2
		{
			count = 0;
			//frame�� pBuffer ��, data �ٲ��ֱ�.
			for (int i = calib.GetLeftTop().y * 512 + calib.GetLeftTop().x; i < calib.GetRightBottom().y * 512 + calib.GetRightBottom().x; i++)
			{
				if ((frame[i] + 5 < pBuffer[i] && frame[i] + 20 > pBuffer[i]) || (frame[i] - 20 < pBuffer[i] && frame[i] - 5 > pBuffer[i]))
				{
					if (count == 15)
						break;
					else
						count++;
				}
				else {
					count = 0;
				}
			}

			if (count == 15)
			{

				for (int i = calib.GetLeftTop().y * 512 + calib.GetLeftTop().x; i < calib.GetRightBottom().y * 512 + calib.GetRightBottom().x; i++)
				{
					frame[i] = pBuffer[i];

				}
			}
			else
			{

			}
			// 2.���ȭ�鿡 ������ ������
			kController.SetDepth(frame, kController.GetWidth(), kController.GetHeight(), flat);
			// ��źȭ ����
			kController.ChoiceFlattening(&flat);
		}

		// ��Ʈ���� ����
		Mat depthImage(kController.GetHeight(), kController.GetWidth(), CV_8UC4, kController.GetRGBX());

		// ���ϴ� ���� ����
		Mat temp = calib.RoiSetUp(depthImage);

		kView.ShowScreen(&temp);
		// Ű��Ʈ ����
		kController.MaintainKinect();
	}

	delete frame;
	return 0;
}