
#include "patch.h"

//�Կ�Ϊ��λ�����������ص����ͬ��ɫͨ���Ĳ�ƽ�������ж����ƶ�
float patch::Distance(Mat Dst, Mat Src)
{
	float distance = 0;

	for (int i = 0; i < Dst.rows; i++)
	{
		for (int j = 0; j < Dst.cols; j++)
		{
			for (int k = 0; k < 3; k++)//K=3����ɫͨ��
			{
				int tem = Src.at < Vec3b >(i, j)[k] - Dst.at < Vec3b >(i, j)[k];
				distance += tem * tem;//��ƽ��
			}
		}
	}

	return distance;
}


//��ȡ����Ϊ patchsize=3 �ı߽��,(row,col)��������������ص����꣬���������ص��ڱ�Ե�ϵ����
//���������޸��ı�׼��
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

	return Src(Range(row_begin, row_end + 1), Range(col_begin, col_end + 1)); //opencv �ṩ�ı߽�ȷ������
}
