
#include "patch.h"

//¾ßÌåÈçºÎĞŞ¸´ÏñËØ£¿ĞŞ¸´ÏñËØ=Ô­Í¼ÏñËØ+Æ«ÖÃÏñËØ
Vec3b patch::GeneratePixel2(Mat Dst, Mat Src, Mat Mask, Mat Off, int row, int col)
{
	return Src.at < Vec3b>(row + Off.at < Vec2f >(row, col)[0], col + Off.at < Vec2f>(row, col)[1]);
}

//ĞŞ¸´µ±Ç°ÏñËØ
void patch::GenerateImage2(Mat &Dst, Mat Src, Mat Mask, Mat Off)
{
	for (int i = 0; i < Dst.rows; i++)
		for (int j = 0; j < Dst.cols; j++)
			if (Mask.at < uchar >(i, j) == hole)
				Dst.at < Vec3b >(i, j) = GeneratePixel2(Dst, Src, Mask, Off, i, j);
}
