
#include "patch.h"

//进一步把search部分分为：边缘的不可进入black区域 和 可进入search部分
//这个是可以理解的，这个是为了不让搜索的框超出原图边缘
void patch::InitMask(Mat Mask, Mat &Mask2)
{
	Mask2 = Mask.clone();

	//将边界处的search设为back
	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = 0; j< Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == search)
			{
				Mat MaskPatch = GetPatch(Mask, i, j);//获取了以（i，j）像素为中心，半径为patchsize/2的边界框
				if (MaskPatch.rows*MaskPatch.cols != PatchSize*PatchSize) //如果以（i，j）为中心的块长宽不跟设定的完整块长宽一致，
																		//那么证明（i，j）恰好位于图像边缘内（宽为patchsize/2），把search标记为back，
					Mask2.at<uchar>(i, j) = back;							

			}
		}
	}
}

//初始化偏置图像
void patch::InitOff(Mat Mask, Mat &Off)
{
	//为方便起见，将所有的都附上，要求不能赋值到非搜索区域
	//初始化格式
	Off = Mat(Mask.size(), CV_32FC2, Scalar::all(0));//2维无符号32位精度浮点数

	for (int i = 0; i< Mask.rows; i++)
	{ for (int j = 0; j< Mask.cols; j++)
		{
			//不考虑search区域，没有破损，他们的最佳偏移向量当然是0，自己
			if (Mask.at<uchar>(i, j) == search)
			{
				Off.at<Vec2f>(i, j)[0] = 0;  //<Vec2f> 向量，2维，浮点数
				Off.at<Vec2f>(i, j)[1] = 0;
			}
			else//处理hole，采用随机偏置  
			{
				//先初始化2个偏置数r_col,r_row
				int r_col = rand() % Mask.cols; //rand（）产生随机数，主要是产生一个偏置的初始值
				int r_row = rand()  % Mask.rows;
				r_col = r_col + j < Mask.cols ? r_col : r_col - Mask.cols;//边界检测
				r_row = r_row + i < Mask.rows ? r_row : r_row - Mask.rows;
				
				//为什么要有这个循环？因为一次的随机赋值，很可能会出现偏置后的块跑到破损区域，或者是超出限定搜索框的边界
				while (
					   ! (Mask.at<uchar>(r_row + i, r_col + j) == search	//这里加上I，j，是因为他是A投影到B中的搜索偏置
						 && abs(r_row) < searchrowratio*Mask.rows))	//searchrowratio=0.5，搜索的时候,确保r_row偏置不会太远，一定是在原图像的大小里	
				{
					r_col = rand() % Mask.cols;
					r_row = rand() % Mask.rows;

					//边界检测
					r_col = r_col + j < Mask.cols ? r_col : r_col - Mask.cols;
					r_row = r_row + i < Mask.rows ? r_row : r_row - Mask.rows;
				}

				//赋偏置值
				Off.at<Vec2f>(i, j)[0] = r_row;
				Off.at<Vec2f>(i, j)[1] = r_col;
			}
		}
	}
}

