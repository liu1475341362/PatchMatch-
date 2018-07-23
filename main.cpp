
#include "patch.h"


int main()
{
	Mat Src = imread("1.jpg");

	//初始化Mask掩码图(灰度图，初始值0) 和 Search 图像,
	Mat Mask = Mat(Src.size(), CV_8UC1, Scalar::all(0));
	Mat Search = Mat(Src.size(), CV_8UC3, Scalar::all(0));

//初始化掩码图MASK

	for (int i = 0; i< Mask.rows; i++)
		for (int j = 0; j< Mask.cols; j++)
			if (Src.at<Vec3b>(i, j) == Vec3b(255, 255, 255))  //检测到空白处，标记为待修复区域
			{
				Mask.at<uchar>(i, j) = hole;
			}
			else
				Mask.at<uchar>(i, j) = search;	//其他标记为搜寻空间
		
	//此时的掩码图像Mask还是空白的，需要把原图像的完好区域的具体像素赋给搜寻空间
	for (int i = 0; i< Mask.rows; i++)
	{	for (int j = 0; j< Mask.cols; j++){
			if (Mask.at<uchar>(i, j) == search)
				Search.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
		}
	}

	int ps = 3;//损像素修复窗口半径,即是patchsize
	int pyr = 5;//图像金字塔的层数
	int ann = 3;//最低最近最邻域迭代次数 anniteration

	//修复过程中，mask0是修复的操作图像
	//把像素破损点扩充为块
	Mat Mask0 = Mask.clone(); 
	for (int i = 0; i< Mask.rows; i++)
	{ for (int j = 0; j< Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == hole)//如果是破损像素
			{	
				//扩充破损像素窗口为9*9
				int e_s = 3 * ps;//3*3

				//让这个窗口不越 图像边界
				//x方向
				int r_b = i - e_s / 2 >0  ?  i - e_s / 2  :  0;   //A? B:C  若A为真，取B，否则取C
				int r_e = i + e_s / 2<Mask.rows ? i + e_s / 2 : Mask.rows - 1;
				//y方向
				int c_b = j - e_s / 2>0 ? j - e_s / 2 : 0;
				int c_e = j + e_s / 2<Mask.cols - 1 ? j + e_s / 2 : Mask.cols - 1;

				for (int k = r_b; k< r_e; k++)
					for (int p = c_b; p< c_e; p++)
						Mask0.at<uchar>(k, p) = hole;//没有加具体通道，默认全设置 hole=255（白色）

			}
		}
	}


//初始化搜索图像
	Search.setTo(0);//MAT search 的值全部设置成0
	for (int i = 0; i< Mask.rows; i++)
		for (int j = 0; j< Mask.cols; j++)
			if (Mask0.at<uchar>(i, j) == search)
				Search.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

	imshow("src", Src);
	imshow("mask", Mask);
	imshow("Mask0", Mask0);
	imshow("search", Search);

	patch *test = new patch(Src, Mask0, ps, pyr, ann, "result.jpg"); //创建对象test
	test->Run();//调用类patch的成员函数run()运行修复程序.

	cout << "Finish！" << endl;
	waitKey();
	return 0;
}
