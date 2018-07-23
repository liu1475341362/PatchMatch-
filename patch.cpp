
#include "patch.h"
#include <time.h>

//������patch�У���Ա����patch�����Ҫ����.   ::�����ⲿ������Ա����
patch::patch(Mat Src, Mat Mask, int ps, int pyr, int ANN, string result)
{
	srand((unsigned)time(NULL));//��Ϊ��α�����������Ҫʱ�̱仯��ʱ������Ϊ�������srand�ĳ�ʼ������

	//�ж��������������
	assert(Src.type() == CV_8UC3);
	assert(Mask.type() == CV_8UC1);

	SrcImg = Src.clone();
	DstImg = Src.clone();
	MaskImg = Mask.clone();

	OffsetImg = Mat(MaskImg.size(), CV_32FC2, Scalar::all(0));
	show = Mat::zeros(DstImg.size(), CV_8UC3);

	PatchSize = ps;
	//��randomsearch����������������
	MaxWindow = -1;
	MinWindow = 7 * PatchSize;

	//����������
	PryIteration = pyr;
	//�����������������
	ANNIteration = ANN;

	//���˥��
	RandomAttenuation = 0.5f;

	filename = result;

	searchrowratio = 0.5;//��˼�ǣ�������ʱ���ֻ����searchΪ���ģ� ����һ��2*ratio��������
}



//�޸�������
//������patch�У���Ա����Run. 
Mat patch::Run()
{	
	//ͼ�����飬����������
	Mat Srcs[30];
	Mat Dsts[30];
	Mat Masks[30];
	Mat Masks2[30];
	Mat Offs[30];

	Srcs[0] = SrcImg;
	Dsts[0] = DstImg;
	Masks[0] = MaskImg;
	Offs[0] = OffsetImg;//ƫ��ͼ��

//��һ����search���ַ�Ϊ����Ե�Ĳ��ɽ���black���� �� �ɽ���search����
	InitMask(Masks[0], Masks2[0]);

	int i = 0;//�������㼶����

	cout << "PyrDown Step " << i << endl;

//������4�����ʹ���
	while (i < PryIteration				//������Ϊ5
		&& Dsts[i].rows > MinWindow  	//��������ײ��ͼ��ߴ磨�������ܱ��趨����С���س��� 7*patchsize=21 ��С
		&& Dsts[i].cols > MinWindow)                         
	{
		cout << "PyrDown Step " << i + 1 << endl;

		pyrDown(Srcs[i], Srcs[i + 1]);//opencv�ṩ�Ľ���������
		pyrDown(Dsts[i], Dsts[i + 1]);

		PyrDownMask(Masks[i], Masks[i + 1]);//��������󽵲��������û�����Դ��ģ���Ϊ��������ʱ�򣬻��ø�˹�ں˾��һ�Σ������ͻ��ƻ� hole�ȵ�ֵ��

		InitMask(Masks[i + 1], Masks2[i + 1]);//���¸��㼶���������ֻ�Ϊblack��search

		i++;
	}

	cout << endl;

//��ʼ��ƫ�õ�ͼ,1
	InitOff(Masks2[i], Offs[i]);

//����ɳ�ʼ�����ͼ����ִ�С�
//patchmatchѭ���㷨 �������� �������������
	while (i >= 0)
	{
		cout << "PyrUp Step " << i << endl;

		//��¼��ǰͼƬ���Ŀ��
		MaxWindow = max(Srcs[i].rows, Srcs[i].cols);

		//�ʼ���ܴ��Ե��ó�ʼƫ�õ�ͼ�����ؼ��������޸�����1
		GenerateImage2(Dsts[i], Srcs[i], Masks2[i], Offs[i]);

		//show��չʾ��ͼ��ĺ���
		show.setTo(0);
		resize(Dsts[i], show, DstImg.size());//resize��ͼ�������һ����С����input��output��size��
		imshow("show", show);
		waitKey(100);

		//�޸���ǰ�������㼶ͼ��ANNIteration��������������3�Σ����������ANNIteration + i * 5
		//�ڴֲڵļ����ϣ����������Ρ�
		for (int j = 0; j< ANNIteration + i * 5; j++)
		{
			int odd = j;

			cout << "ANN " << j << " start" << endl;

//�޸�ͼ����ں���GenerateImage
//��ʱ��Ҳ���������������ǰ�ڵĲ����ĳ�ʼ������
			GenerateImage(Dsts[i], Srcs[i], Masks2[i], Offs[i], odd);
			cout << "ANN " << j << " end" << endl;

			show.setTo(0);
			resize(Dsts[i], show, DstImg.size());
			imshow("show", show);
			waitKey(100);
		}

		//��ƫ��ͼ������
		if (i > 0)
			PyrUpOff(Offs[i], Masks2[i - 1], Offs[i - 1]);

		cout << endl;

		i--;
	}

	waitKey(33);
	imwrite(filename.data(), DstImg);
	return DstImg;
}
