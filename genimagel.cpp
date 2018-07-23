
#include "patch.h"

//此函数是修复主函数调用的修复子函数
//（row,col）-在Dst_img中的扫描的像素点
//odd-当前迭代的次数
Vec3b patch::GeneratePixel(Mat Dst, Mat Src, Mat Mask, Mat Off, int row,
	int col, int odd)
{
	//获取坐标（row,col）为中心的长度为 patchsize 的边界框
	//这里值得一提的是，扫描的坐标和GetPatch的坐标一个在左上方，一个在中心
	Mat DstSize = GetPatch(Dst, row, col);

	int window_length = DstSize.rows * DstSize.cols;

	//初始化缓存数组，这些数组是用来存储patch里的图像的，用的是指针的存储方法
	//语句声明a是float型指针，并将其初始化为指向一个具有 window_length 个 float型元素数组的首地址
	float *a = new float[window_length];
	float *sim = new float[window_length];
	float *w = new float[window_length];
	Vec3b *c = new Vec3b[window_length];
	int nowlength = 0;

	//确定比例系数
	float ratio =
		sqrt(Dst.rows * Dst.rows + Dst.cols * Dst.cols * 1.0f) > 150 ?
		sqrt(Dst.rows * Dst.rows + Dst.cols * Dst.cols * 1.0f) / 150 : 1;

//偶数次迭代，扫描 右下 的patch
	if (odd % 2 == 0)
	{
		//以patch为单位，PatchSize=3
		for (int i = 0; i < PatchSize; i++)
		{ for (int j = 0; j < PatchSize; j++)
			{	
				//逐个扫描以（row,col）为中心，宽为patchsize的patch里，每一个像素（now_row，now_col）
				int now_row = row - (PatchSize / 2) + i;  
				int now_col = col - (PatchSize / 2) + j;

				//考虑到在patch里的像素点可能会在边界上，或者在back上，跳过
				if (now_row < PatchSize / 2 || now_col < PatchSize / 2
					|| now_row > Dst.rows - 1 - PatchSize / 2
					|| now_col > Dst.cols - 1 - PatchSize / 2
					|| Mask.at < uchar >(now_row, now_col) == back)
				{
					continue;//跳出当前像素循环
				}

	//传播
				Propagation(Dst, Src, Mask, Off, now_row, now_col, odd);//（now_row, now_col）：patch里的像素
	//随机扰动搜索
				RandomSearch(Dst, Src, Mask, Off, now_row, now_col);

				//提取偏置映射后的最优点（patch_row，patch_col）
				int patch_row = now_row + Off.at < Vec2f >(now_row, now_col)[0];
				int patch_col = now_col + Off.at < Vec2f >(now_row, now_col)[1];

				//提取 更新后块，原偏置块
				Mat DstPatch = GetPatch(Dst, now_row, now_col);
				Mat SrcPatch = GetPatch(Src, patch_row, patch_col);

				//c
				c[nowlength] = SrcPatch.at < Vec3b >(PatchSize - 1 - i, PatchSize - 1 - j);
				//按照距离该点的距离算
				a[nowlength] = sqrt((now_row - patch_row) * (now_row - patch_row) * 1.0	+ 
								(now_col - patch_col) * (now_col - patch_col) * 1.0)
								/ ratio;

				//通过得到2个块的差平方的值，判断的相似度
				sim[nowlength] = Distance(SrcPatch, DstPatch);

				//简单自增记录循环次数
				nowlength++;
			}
		}

		//获取了一个所有块差平方冒泡排序的0.75值
		float delta = GetDelta(sim, nowlength);

		//
		if (delta == 0)
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i]);//e的-sim[i]次方
				a[i] = pow(1.3f, -a[i]);//pow(a,b) ：a的b次方.
				w[i] = a[i] * sim[i];
			}
		else
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i] / (2 * delta));
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i] * sim[i];
			}
	}


//奇数次迭代，扫描 左上 的patch
	else
	{
		for (int i = PatchSize - 1; i >= 0; i--)
		{
			for (int j = PatchSize - 1; j >= 0; j--)
			{
				int now_row = row - (PatchSize / 2) + i;
				int now_col = col - (PatchSize / 2) + j;

				if (now_row < PatchSize / 2 || now_col < PatchSize / 2
					|| now_row > Dst.rows - 1 - PatchSize / 2
					|| now_col > Dst.cols - 1 - PatchSize / 2
					|| Mask.at < uchar >(now_row, now_col) == back)
				{
					continue;
				}

				Propagation(Dst, Src, Mask, Off, now_row, now_col, odd);
				RandomSearch(Dst, Src, Mask, Off, now_row, now_col);

				int patch_row = now_row + Off.at < Vec2f >(now_row, now_col)[0];
				int patch_col = now_col + Off.at < Vec2f >(now_row, now_col)[1];

				Mat DstPatch = GetPatch(Dst, now_row, now_col);//A图的块
				Mat SrcPatch = GetPatch(Src, patch_row, patch_col);//B图中的块

				//访问SrcPatch里的像素，最后随着循环的增加遍历整个块
				c[nowlength] = SrcPatch.at < Vec3b >(PatchSize - 1 - i, PatchSize - 1 - j);

				//按照距离该点的距离算
				a[nowlength] = sqrt(
					(now_row - patch_row) * (now_row - patch_row) * 1.0
					+ (now_col - patch_col) * (now_col - patch_col)
					* 1.0) / ratio;
				sim[nowlength] = Distance(SrcPatch, DstPatch);
				nowlength++;
			}
		}

		//sim记录A，B图块的像素颜色差
		float delta = GetDelta(sim, nowlength);

		if (delta == 0)//因为sim里存储的是差平方，只有在所有块都一致，冒泡排序的中间的差异值为0的时候，才会出现这个情况
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i]);
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i] * sim[i];
			}
		else
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i] / (2 * delta));
				a[i] = pow(1.3f, -a[i]);
				w[i] = a[i] * sim[i];
			}
	}

	//最后返回修复的像素平均值
	return GetMeanshift(w, c, nowlength, odd);
}


//此函数是修复的主函数。
//为了避免收敛的问题，采用偶次，奇数迭代次数不同的搜索方向；
//odd ：当前迭代的次数
//其实无论是偶数次还是奇数次是往右下，最重要是匹配一点：右下遍历修复，找的块一定往左上找，反之亦然
void patch::GenerateImage(Mat &Dst, Mat Src, Mat Mask, Mat Off, int odd)
{
	if (odd % 2 == 0)//偶次数迭代,遍历 右下 的像素点
		for (int i = 0; i < Dst.rows; i++)
			for (int j = 0; j < Dst.cols; j++)
			{
				if (Mask.at < uchar >(i, j) == hole)//确定（I，j）为破损像素
					Dst.at < Vec3b >(i, j) = GeneratePixel(Dst, Src, Mask, Off,
						i, j, odd);
			}
	else//奇次数迭代，遍历 左上 的像素点
		for (int i = Dst.rows - 1; i >= 0; i--)
			for (int j = Dst.cols - 1; j >= 0; j--)
			{
				if (Mask.at < uchar >(i, j) == hole)
					Dst.at < Vec3b >(i, j) = GeneratePixel(Dst, Src, Mask, Off,
						i, j, odd);
			}
}
