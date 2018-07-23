
#include "patch.h"

//返回最小差值（最相似）的块的代表数字，用于后续判断
//GetMinPatch1的简化版
int patch::GetMinPatch2(Mat Dst, Mat Src1, Mat Src2)
{
	float dist1 = Distance(Dst, Src1);
	float dist2 = Distance(Dst, Src2);

	if (dist1 < dist2)
		return 1;
	else
		return 2;
}

//迭代第二步：随机搜索
//（row,col）=（now_row, now_col）：修复patch里的像素
void patch::RandomSearch(Mat Dst, Mat Src, Mat Mask, Mat &Off, int row, int col)
{
	Mat DstPatch = GetPatch(Dst, row, col);//获取修复基准框，在框内操作

	//迭代指数
	int attenuate = 0;

	while (true)
	{
		//获取随机参数，在 [-1;1] 间
		float divcol = rand() % 2000 / 1000.0f - 1.0f;//rand(): 0~RAND_MAX
		float divrow = rand() % 2000 / 1000.0f - 1.0f;

		//减小框大小的公式，𝑢_𝑖=𝑣_0+𝑤*𝛼^𝑖*𝑅_𝑖
		//行列分别处理，MaxWindow：原始框宽度；divcol：随机系数；pow(A,B):A的B次方。随迭代次数而变小的缩小系数；RandomAttenuation=0.5；
		float veccol = MaxWindow * pow(RandomAttenuation, attenuate)* divcol;
		float vecrow = MaxWindow * pow(RandomAttenuation, attenuate)* divrow;

		float length = sqrt(veccol * veccol + vecrow * vecrow);
		//如果低于1个像素，没有意义，直接结束整个循环，对下一个像素处理
		if (length < 1) 
			break;
		
		//x方向,前2项指向(row, col)的match块，后面是公式的后一项
		int nowrow = row + Off.at < Vec2f >(row, col)[0] + vecrow;
		//y方向
		int nowcol = col + Off.at < Vec2f >(row, col)[1] + veccol;
		
		//判断随机搜索的patch不越界，在search内
		if (nowcol >= 0 && nowcol <= Off.cols - 1 && nowrow >= 0
			&& nowrow <= Off.rows - 1
			&& Mask.at < uchar >(nowrow, nowcol) == search
			&& abs(nowrow - row) < searchrowratio * Mask.rows)//abs：绝对值
		{	
			//取出原来的match块
			Mat SrcPatch1 = GetPatch(Src, Off.at < Vec2f >(row, col)[0] + row,
									 Off.at < Vec2f >(row, col)[1] + col);
			//取出现在的随机match块
			Mat SrcPatch2 = GetPatch(Src, nowrow, nowcol);

			//对比相似性，找出最好的块
			int location = GetMinPatch2(DstPatch, SrcPatch1, SrcPatch2);

			//结合最好的相似块给像素新的偏置值
			switch (location)
			{
			case 2:
				Off.at < Vec2f >(row, col)[0] = nowrow - row;//现在的最优点x坐标-修复的像素点坐标
				Off.at < Vec2f >(row, col)[1] = nowcol - col;
				break;
			}
		}

		//迭代指数增加
		attenuate++;
	}
}


//返回最小差值（最相似）的块的代表数字，用于后续判断
int patch::GetMinPatch1(Mat Dst, Mat Src1, Mat Src2, Mat Src3)
{
	//patch块与每一个match块对比
	float dist1 = Distance(Dst, Src1);
	float dist2 = Distance(Dst, Src2);
	float dist3 = Distance(Dst, Src3);

	//返回最小差值（最相似）的块的代表数字，用于后续判断
	if (dist1 < dist2)
	{
		if (dist1 < dist3)
			return 1;
		else
			return 3;
	}
	else if (dist2 < dist3)
		return 2;
	else
		return 3;
}


//迭代第一步：传播
//（now_row, now_col）=(row, col)：patch里的像素
//odd：当前迭代次
void patch::Propagation(Mat Dst, Mat Src, Mat Mask, Mat &Off, int row, int col,
	int odd)
{
	Mat DstPatch = GetPatch(Dst, row, col);//获取长度为 patchsize = 3 的边界框, (row, col)代表的是中心像素点坐标

	if (odd % 2 == 0)//偶次迭代, 右下 顺序扫描像素
	{	
		//提取(row, col)的match块
		Mat SrcPatch = GetPatch(Src, row + Off.at < Vec2f >(row, col)[0],
			col + Off.at < Vec2f >(row, col)[1]);

		//提取(row, col-1)的match块
		Mat LSrcPatch = GetPatch(Src, row + Off.at < Vec2f >(row, col - 1)[0],
			col - 1 + Off.at < Vec2f >(row, col - 1)[1]);

		//提取(row-1, col)的match块
		Mat USrcPatch = GetPatch(Src,
			row - 1 + Off.at < Vec2f >(row - 1, col)[0],
			col + Off.at < Vec2f >(row - 1, col)[1]);

		//返回上面4个块最相似的块的代表数字，用于switch判断
		int location = GetMinPatch1(DstPatch, SrcPatch, LSrcPatch, USrcPatch);
		
		//利用上面的信息更新像素点的偏置地图
		switch (location)
		{
		case 2://这里的(row, col - 1)意思是：绿色块最好。为什么-1？
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row, col - 1)[0];
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row, col - 1)[1] - 1;//+1 问题出现在某个特定条件下的溢出
			
			
			break;
		case 3:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row - 1, col)[0] - 1;
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row - 1, col)[1];
			break;
		}
	}

	else//奇数次迭代
	{
		Mat SrcPatch = GetPatch(Src, row + Off.at < Vec2f >(row, col)[0],
			col + Off.at < Vec2f >(row, col)[1]);
		Mat RSrcPatch = GetPatch(Src, row + Off.at < Vec2f >(row, col + 1)[0],
			col + 1 + Off.at < Vec2f >(row, col + 1)[1]);
		Mat DSrcPatch = GetPatch(Src,
			row + 1 + Off.at < Vec2f >(row + 1, col)[0],
			col + Off.at < Vec2f >(row + 1, col)[1]);

		int location = GetMinPatch1(DstPatch, SrcPatch, RSrcPatch, DSrcPatch);
		switch (location)
		{
		case 2:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row, col + 1)[0];
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row, col + 1)[1] + 1;
			break;
		case 3:
			Off.at < Vec2f >(row, col)[0] = Off.at < Vec2f >(row + 1, col)[0] + 1;
			Off.at < Vec2f >(row, col)[1] = Off.at < Vec2f >(row + 1, col)[1];
			break;
		}
	}
}
