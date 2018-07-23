
#include "patch.h"


int main()
{
	Mat Src = imread("1.jpg");

	//��ʼ��Mask����ͼ(�Ҷ�ͼ����ʼֵ0) �� Search ͼ��,
	Mat Mask = Mat(Src.size(), CV_8UC1, Scalar::all(0));
	Mat Search = Mat(Src.size(), CV_8UC3, Scalar::all(0));

//��ʼ������ͼMASK

	for (int i = 0; i< Mask.rows; i++)
		for (int j = 0; j< Mask.cols; j++)
			if (Src.at<Vec3b>(i, j) == Vec3b(255, 255, 255))  //��⵽�հ״������Ϊ���޸�����
			{
				Mask.at<uchar>(i, j) = hole;
			}
			else
				Mask.at<uchar>(i, j) = search;	//�������Ϊ��Ѱ�ռ�
		
	//��ʱ������ͼ��Mask���ǿհ׵ģ���Ҫ��ԭͼ����������ľ������ظ�����Ѱ�ռ�
	for (int i = 0; i< Mask.rows; i++)
	{	for (int j = 0; j< Mask.cols; j++){
			if (Mask.at<uchar>(i, j) == search)
				Search.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);
		}
	}

	int ps = 3;//�������޸����ڰ뾶,����patchsize
	int pyr = 5;//ͼ��������Ĳ���
	int ann = 3;//������������������� anniteration

	//�޸������У�mask0���޸��Ĳ���ͼ��
	//���������������Ϊ��
	Mat Mask0 = Mask.clone(); 
	for (int i = 0; i< Mask.rows; i++)
	{ for (int j = 0; j< Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == hole)//�������������
			{	
				//�����������ش���Ϊ9*9
				int e_s = 3 * ps;//3*3

				//��������ڲ�Խ ͼ��߽�
				//x����
				int r_b = i - e_s / 2 >0  ?  i - e_s / 2  :  0;   //A? B:C  ��AΪ�棬ȡB������ȡC
				int r_e = i + e_s / 2<Mask.rows ? i + e_s / 2 : Mask.rows - 1;
				//y����
				int c_b = j - e_s / 2>0 ? j - e_s / 2 : 0;
				int c_e = j + e_s / 2<Mask.cols - 1 ? j + e_s / 2 : Mask.cols - 1;

				for (int k = r_b; k< r_e; k++)
					for (int p = c_b; p< c_e; p++)
						Mask0.at<uchar>(k, p) = hole;//û�мӾ���ͨ����Ĭ��ȫ���� hole=255����ɫ��

			}
		}
	}


//��ʼ������ͼ��
	Search.setTo(0);//MAT search ��ֵȫ�����ó�0
	for (int i = 0; i< Mask.rows; i++)
		for (int j = 0; j< Mask.cols; j++)
			if (Mask0.at<uchar>(i, j) == search)
				Search.at<Vec3b>(i, j) = Src.at<Vec3b>(i, j);

	imshow("src", Src);
	imshow("mask", Mask);
	imshow("Mask0", Mask0);
	imshow("search", Search);

	patch *test = new patch(Src, Mask0, ps, pyr, ann, "result.jpg"); //��������test
	test->Run();//������patch�ĳ�Ա����run()�����޸�����.

	cout << "Finish��" << endl;
	waitKey();
	return 0;
}
