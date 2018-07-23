
#include "patch.h"

//以块为单位，用所有像素点的相同颜色通道的差平方来简单判断相似度
float patch::Distance(Mat Dst, Mat Src)
{
	float distance = 0;

	for (int i = 0; i < Dst.rows; i++)
	{
		for (int j = 0; j < Dst.cols; j++)
		{
			for (int k = 0; k < 3; k++)//K=3个颜色通道
			{
				int tem = Src.at < Vec3b >(i, j)[k] - Dst.at < Vec3b >(i, j)[k];
				distance += tem * tem;//差平方
			}
		}
	}

	return distance;
}


//获取长度为 patchsize=3 的边界框,(row,col)代表的是中心像素点坐标，考虑了像素点在边缘上的情况
//这个框就是修复的标准框
Mat patch::GetPatch(Mat Src, int row, int col)
{	
	int row_begin = row - (PatchSize / 2) >= 0 ? row - (PatchSize / 2) : 0;
	int row_end =
		row + (PatchSize / 2) <= Src.rows - 1 ?
		row + (PatchSize / 2) : Src.rows - 1;

	int col_begin = col - (PatchSize / 2) >= 0 ? col - (PatchSize / 2) : 0;
	int col_end =
		col + (PatchSize / 2) <= Src.cols - 1 ?
		col + (PatchSize / 2) : Src.cols - 1;

	return Src(Range(row_begin, row_end + 1), Range(col_begin, col_end + 1)); //opencv 提供的边界确定函数
}
