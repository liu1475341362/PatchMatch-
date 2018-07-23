
#include "patch.h"
#include <time.h>

//定义类patch中，成员函数patch里的主要参数.   ::在类外部声明成员函数
patch::patch(Mat Src, Mat Mask, int ps, int pyr, int ANN, string result)
{
	srand((unsigned)time(NULL));//因为是伪随机函数，需要时刻变化的时间来作为随机函数srand的初始化种子

	//判断输入的类型无误
	assert(Src.type() == CV_8UC3);
	assert(Mask.type() == CV_8UC1);

	SrcImg = Src.clone();
	DstImg = Src.clone();
	MaskImg = Mask.clone();

	OffsetImg = Mat(MaskImg.size(), CV_32FC2, Scalar::all(0));
	show = Mat::zeros(DstImg.size(), CV_8UC3);

	PatchSize = ps;
	//在randomsearch函数里面重新设置
	MaxWindow = -1;
	MinWindow = 7 * PatchSize;

	//金字塔级数
	PryIteration = pyr;
	//最近最邻域搜索次数
	ANNIteration = ANN;

	//随机衰减
	RandomAttenuation = 0.5f;

	filename = result;

	searchrowratio = 0.5;//意思是：搜索的时候会只在以search为中心， 上下一共2*ratio里面搜索
}



//修复主程序
//定义类patch中，成员函数Run. 
Mat patch::Run()
{	
	//图像数组，缓存迭代结果
	Mat Srcs[30];
	Mat Dsts[30];
	Mat Masks[30];
	Mat Masks2[30];
	Mat Offs[30];

	Srcs[0] = SrcImg;
	Dsts[0] = DstImg;
	Masks[0] = MaskImg;
	Offs[0] = OffsetImg;//偏置图像

//进一步把search部分分为：边缘的不可进入black区域 和 可进入search部分
	InitMask(Masks[0], Masks2[0]);

	int i = 0;//金字塔层级计数

	cout << "PyrDown Step " << i << endl;

//降采样4种类型窗口
	while (i < PryIteration				//层数设为5
		&& Dsts[i].rows > MinWindow  	//金字塔最底层的图像尺寸（长宽）不能比设定的最小像素长度 7*patchsize=21 还小
		&& Dsts[i].cols > MinWindow)                         
	{
		cout << "PyrDown Step " << i + 1 << endl;

		pyrDown(Srcs[i], Srcs[i + 1]);//opencv提供的降采样函数
		pyrDown(Dsts[i], Dsts[i + 1]);

		PyrDownMask(Masks[i], Masks[i + 1]);//对掩码矩阵降采样，这个没法用自带的，因为降采样的时候，会用高斯内核卷积一次，这样就会破坏 hole等的值了

		InitMask(Masks[i + 1], Masks2[i + 1]);//对下个层级的掩码矩阵分化为black和search

		i++;
	}

	cout << endl;

//初始化偏置地图,1
	InitOff(Masks2[i], Offs[i]);

//在完成初始化后的图像中执行↓
//patchmatch循环算法 ———— 传播，随机搜索
	while (i >= 0)
	{
		cout << "PyrUp Step " << i << endl;

		//记录当前图片最大的宽度
		MaxWindow = max(Srcs[i].rows, Srcs[i].cols);

		//最开始，很粗略地用初始偏置地图在像素级别上来修复像素1
		GenerateImage2(Dsts[i], Srcs[i], Masks2[i], Offs[i]);

		//show是展示的图像的函数
		show.setTo(0);
		resize(Dsts[i], show, DstImg.size());//resize把图像调整到一定大小，（input，output，size）
		imshow("show", show);
		waitKey(100);

		//修复当前金字塔层级图像，ANNIteration：基础迭代次数3次；扩充次数：ANNIteration + i * 5
		//在粗糙的级别上，多搜索几次。
		for (int j = 0; j< ANNIteration + i * 5; j++)
		{
			int odd = j;

			cout << "ANN " << j << " start" << endl;

//修复图像入口函数GenerateImage
//这时候也仅仅是完成了所有前期的参数的初始化工作
			GenerateImage(Dsts[i], Srcs[i], Masks2[i], Offs[i], odd);
			cout << "ANN " << j << " end" << endl;

			show.setTo(0);
			resize(Dsts[i], show, DstImg.size());
			imshow("show", show);
			waitKey(100);
		}

		//将偏置图升采样
		if (i > 0)
			PyrUpOff(Offs[i], Masks2[i - 1], Offs[i - 1]);

		cout << endl;

		i--;
	}

	waitKey(33);
	imwrite(filename.data(), DstImg);
	return DstImg;
}
