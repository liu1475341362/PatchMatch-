
#include "patch.h"

//��һ����search���ַ�Ϊ����Ե�Ĳ��ɽ���black���� �� �ɽ���search����
//����ǿ������ģ������Ϊ�˲��������Ŀ򳬳�ԭͼ��Ե
void patch::InitMask(Mat Mask, Mat &Mask2)
{
	Mask2 = Mask.clone();

	//���߽紦��search��Ϊback
	for (int i = 0; i< Mask.rows; i++)
	{
		for (int j = 0; j< Mask.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == search)
			{
				Mat MaskPatch = GetPatch(Mask, i, j);//��ȡ���ԣ�i��j������Ϊ���ģ��뾶Ϊpatchsize/2�ı߽��
				if (MaskPatch.rows*MaskPatch.cols != PatchSize*PatchSize) //����ԣ�i��j��Ϊ���ĵĿ鳤�����趨�������鳤��һ�£�
																		//��ô֤����i��j��ǡ��λ��ͼ���Ե�ڣ���Ϊpatchsize/2������search���Ϊback��
					Mask2.at<uchar>(i, j) = back;							

			}
		}
	}
}

//��ʼ��ƫ��ͼ��
void patch::InitOff(Mat Mask, Mat &Off)
{
	//Ϊ��������������еĶ����ϣ�Ҫ���ܸ�ֵ������������
	//��ʼ����ʽ
	Off = Mat(Mask.size(), CV_32FC2, Scalar::all(0));//2ά�޷���32λ���ȸ�����

	for (int i = 0; i< Mask.rows; i++)
	{ for (int j = 0; j< Mask.cols; j++)
		{
			//������search����û���������ǵ����ƫ��������Ȼ��0���Լ�
			if (Mask.at<uchar>(i, j) == search)
			{
				Off.at<Vec2f>(i, j)[0] = 0;  //<Vec2f> ������2ά��������
				Off.at<Vec2f>(i, j)[1] = 0;
			}
			else//����hole���������ƫ��  
			{
				//�ȳ�ʼ��2��ƫ����r_col,r_row
				int r_col = rand() % Mask.cols; //rand�����������������Ҫ�ǲ���һ��ƫ�õĳ�ʼֵ
				int r_row = rand()  % Mask.rows;
				r_col = r_col + j < Mask.cols ? r_col : r_col - Mask.cols;//�߽���
				r_row = r_row + i < Mask.rows ? r_row : r_row - Mask.rows;
				
				//ΪʲôҪ�����ѭ������Ϊһ�ε������ֵ���ܿ��ܻ����ƫ�ú�Ŀ��ܵ��������򣬻����ǳ����޶�������ı߽�
				while (
					   ! (Mask.at<uchar>(r_row + i, r_col + j) == search	//�������I��j������Ϊ����AͶӰ��B�е�����ƫ��
						 && abs(r_row) < searchrowratio*Mask.rows))	//searchrowratio=0.5��������ʱ��,ȷ��r_rowƫ�ò���̫Զ��һ������ԭͼ��Ĵ�С��	
				{
					r_col = rand() % Mask.cols;
					r_row = rand() % Mask.rows;

					//�߽���
					r_col = r_col + j < Mask.cols ? r_col : r_col - Mask.cols;
					r_row = r_row + i < Mask.rows ? r_row : r_row - Mask.rows;
				}

				//��ƫ��ֵ
				Off.at<Vec2f>(i, j)[0] = r_row;
				Off.at<Vec2f>(i, j)[1] = r_col;
			}
		}
	}
}

