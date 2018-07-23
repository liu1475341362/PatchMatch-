
#include "patch.h"

//降采样过程
void patch::PyrDownMask(Mat InM, Mat &OutM)//实参与形参
{	
	//降采样后的行和列 是上层的一半
	int rows = (InM.rows + 1) / 2; //多给一行，防止“/”抹去有效信息，导致mask的行列与src等图像不同
	int cols = (InM.cols + 1) / 2;
	OutM = Mat(rows, cols, InM.type());
	 
	//把降采样后的hole，black，和search都对应好，标记优先级：hole>back>search
	for (int i = 0; i< rows; i++)
		for (int j = 0; j< cols; j++)
		{	
			//假设是（0，0）
			int m = i;
			int n = j;

			int m2 = 2 * i;		//0
			int m21 = 2 * i + 1;//1

			int n2 = 2 * j;     //0
			int n21 = 2 * j + 1;//1

			//整个过程是以块的左上角为坐标原点，考察左上，左下，右下，右下四个像素的原标记种类
			//在图中心的情况
			if (m21 < InM.rows && n21 < InM.cols)//如果右下都在image里面，左上肯定也在里面
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
			else if (m21 < InM.rows //在边缘的情况
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
			else if (m21 == InM.rows//在边缘的情况
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
			else//在边缘的情况
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



//升采样过程
void patch::PyrUpOff(Mat InO, Mat InM, Mat &OutO)
{
	int rows = InM.rows;
	int cols = InM.cols;

	OutO = Mat(rows, cols, InO.type());

	//扫描输入图像
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
