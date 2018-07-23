
#include "patch.h"

//����������
void patch::PyrDownMask(Mat InM, Mat &OutM)//ʵ�����β�
{	
	//����������к��� ���ϲ��һ��
	int rows = (InM.rows + 1) / 2; //���һ�У���ֹ��/��Ĩȥ��Ч��Ϣ������mask��������src��ͼ��ͬ
	int cols = (InM.cols + 1) / 2;
	OutM = Mat(rows, cols, InM.type());
	 
	//�ѽ��������hole��black����search����Ӧ�ã�������ȼ���hole>back>search
	for (int i = 0; i< rows; i++)
		for (int j = 0; j< cols; j++)
		{	
			//�����ǣ�0��0��
			int m = i;
			int n = j;

			int m2 = 2 * i;		//0
			int m21 = 2 * i + 1;//1

			int n2 = 2 * j;     //0
			int n21 = 2 * j + 1;//1

			//�����������Կ�����Ͻ�Ϊ����ԭ�㣬�������ϣ����£����£������ĸ����ص�ԭ�������
			//��ͼ���ĵ����
			if (m21 < InM.rows && n21 < InM.cols)//������¶���image���棬���Ͽ϶�Ҳ������
			{
				if (InM.at<uchar>(m2, n2) == hole
					|| InM.at<uchar>(m2, n21) == hole
					|| InM.at<uchar>(m21, n2) == hole
					|| InM.at<uchar>(m21, n21) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back
					|| InM.at<uchar>(m2, n21) == back
					|| InM.at<uchar>(m21, n2) == back
					|| InM.at<uchar>(m21, n21) == back)
					OutM.at<uchar>(m, n) = back;
				else
					OutM.at<uchar>(m, n) = search;
			}
			else if (m21 < InM.rows //�ڱ�Ե�����
				&&n21 == InM.cols)
			 {
				if (InM.at<uchar>(m2, n2) == hole
					|| InM.at<uchar>(m21, n2) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back
					|| InM.at<uchar>(m21, n2) == back)
					OutM.at<uchar>(m, n) = back;
				else
					OutM.at<uchar>(m, n) = search;
			}
			else if (m21 == InM.rows//�ڱ�Ե�����
				&&n21 < InM.cols)
			{
				if (InM.at<uchar>(m2, n2) == hole
					|| InM.at<uchar>(m2, n21) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back
					|| InM.at<uchar>(m2, n21) == back)
					OutM.at<uchar>(m, n) = back;
				else
					OutM.at<uchar>(m, n) = search;
			}
			else//�ڱ�Ե�����
			{
				if (InM.at<uchar>(m2, n2) == hole)
					OutM.at<uchar>(i, j) = hole;
				else if (InM.at<uchar>(m2, n2) == back)
					OutM.at<uchar>(m, n) = back;
				else
					OutM.at<uchar>(m, n) = search;
			}
		}
}



//����������
void patch::PyrUpOff(Mat InO, Mat InM, Mat &OutO)
{
	int rows = InM.rows;
	int cols = InM.cols;

	OutO = Mat(rows, cols, InO.type());

	//ɨ������ͼ��
	for (int i = 0; i< InO.rows; i++)
		for (int j = 0; j< InO.cols; j++)
		{
			int m = i;
			int n = j;

			int m2 = 2 * i;
			int m21 = 2 * i + 1;

			int n2 = 2 * j;
			int n21 = 2 * j + 1;

			if (m21 < OutO.rows
				&&n21 < OutO.cols)
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m21, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m21, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m2, n21)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n21)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m21, n21)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m21, n21)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
			else if (m21 < OutO.rows
				&&n21 == OutO.cols)
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m21, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m21, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
			else if (m21 == OutO.rows
				&&n21 < OutO.cols)
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
				OutO.at<Vec2f>(m2, n21)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n21)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
			else
			{
				OutO.at<Vec2f>(m2, n2)[0] = InO.at<Vec2f>(m, n)[0] * 2;
				OutO.at<Vec2f>(m2, n2)[1] = InO.at<Vec2f>(m, n)[1] * 2;
			}
		}
}
