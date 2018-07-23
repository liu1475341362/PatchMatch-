
#include "patch.h"

//�˺������޸����������õ��޸��Ӻ���
//��row,col��-��Dst_img�е�ɨ������ص�
//odd-��ǰ�����Ĵ���
Vec3b patch::GeneratePixel(Mat Dst, Mat Src, Mat Mask, Mat Off, int row,
	int col, int odd)
{
	//��ȡ���꣨row,col��Ϊ���ĵĳ���Ϊ patchsize �ı߽��
	//����ֵ��һ����ǣ�ɨ��������GetPatch������һ�������Ϸ���һ��������
	Mat DstSize = GetPatch(Dst, row, col);

	int window_length = DstSize.rows * DstSize.cols;

	//��ʼ���������飬��Щ�����������洢patch���ͼ��ģ��õ���ָ��Ĵ洢����
	//�������a��float��ָ�룬�������ʼ��Ϊָ��һ������ window_length �� float��Ԫ��������׵�ַ
	float *a = new float[window_length];
	float *sim = new float[window_length];
	float *w = new float[window_length];
	Vec3b *c = new Vec3b[window_length];
	int nowlength = 0;

	//ȷ������ϵ��
	float ratio =
		sqrt(Dst.rows * Dst.rows + Dst.cols * Dst.cols * 1.0f) > 150 ?
		sqrt(Dst.rows * Dst.rows + Dst.cols * Dst.cols * 1.0f) / 150 : 1;

//ż���ε�����ɨ�� ���� ��patch
	if (odd % 2 == 0)
	{
		//��patchΪ��λ��PatchSize=3
		for (int i = 0; i < PatchSize; i++)
		{ for (int j = 0; j < PatchSize; j++)
			{	
				//���ɨ���ԣ�row,col��Ϊ���ģ���Ϊpatchsize��patch�ÿһ�����أ�now_row��now_col��
				int now_row = row - (PatchSize / 2) + i;  
				int now_col = col - (PatchSize / 2) + j;

				//���ǵ���patch������ص���ܻ��ڱ߽��ϣ�������back�ϣ�����
				if (now_row < PatchSize / 2 || now_col < PatchSize / 2
					|| now_row > Dst.rows - 1 - PatchSize / 2
					|| now_col > Dst.cols - 1 - PatchSize / 2
					|| Mask.at < uchar >(now_row, now_col) == back)
				{
					continue;//������ǰ����ѭ��
				}

	//����
				Propagation(Dst, Src, Mask, Off, now_row, now_col, odd);//��now_row, now_col����patch�������
	//����Ŷ�����
				RandomSearch(Dst, Src, Mask, Off, now_row, now_col);

				//��ȡƫ��ӳ�������ŵ㣨patch_row��patch_col��
				int patch_row = now_row + Off.at < Vec2f >(now_row, now_col)[0];
				int patch_col = now_col + Off.at < Vec2f >(now_row, now_col)[1];

				//��ȡ ���º�飬ԭƫ�ÿ�
				Mat DstPatch = GetPatch(Dst, now_row, now_col);
				Mat SrcPatch = GetPatch(Src, patch_row, patch_col);

				//c
				c[nowlength] = SrcPatch.at < Vec3b >(PatchSize - 1 - i, PatchSize - 1 - j);
				//���վ���õ�ľ�����
				a[nowlength] = sqrt((now_row - patch_row) * (now_row - patch_row) * 1.0	+ 
								(now_col - patch_col) * (now_col - patch_col) * 1.0)
								/ ratio;

				//ͨ���õ�2����Ĳ�ƽ����ֵ���жϵ����ƶ�
				sim[nowlength] = Distance(SrcPatch, DstPatch);

				//��������¼ѭ������
				nowlength++;
			}
		}

		//��ȡ��һ�����п��ƽ��ð�������0.75ֵ
		float delta = GetDelta(sim, nowlength);

		//
		if (delta == 0)
			for (int i = 0; i < nowlength; i++)
			{
				sim[i] = exp(-sim[i]);//e��-sim[i]�η�
				a[i] = pow(1.3f, -a[i]);//pow(a,b) ��a��b�η�.
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


//�����ε�����ɨ�� ���� ��patch
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

				Mat DstPatch = GetPatch(Dst, now_row, now_col);//Aͼ�Ŀ�
				Mat SrcPatch = GetPatch(Src, patch_row, patch_col);//Bͼ�еĿ�

				//����SrcPatch������أ��������ѭ�������ӱ���������
				c[nowlength] = SrcPatch.at < Vec3b >(PatchSize - 1 - i, PatchSize - 1 - j);

				//���վ���õ�ľ�����
				a[nowlength] = sqrt(
					(now_row - patch_row) * (now_row - patch_row) * 1.0
					+ (now_col - patch_col) * (now_col - patch_col)
					* 1.0) / ratio;
				sim[nowlength] = Distance(SrcPatch, DstPatch);
				nowlength++;
			}
		}

		//sim��¼A��Bͼ���������ɫ��
		float delta = GetDelta(sim, nowlength);

		if (delta == 0)//��Ϊsim��洢���ǲ�ƽ����ֻ�������п鶼һ�£�ð��������м�Ĳ���ֵΪ0��ʱ�򣬲Ż����������
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

	//��󷵻��޸�������ƽ��ֵ
	return GetMeanshift(w, c, nowlength, odd);
}


//�˺������޸�����������
//Ϊ�˱������������⣬����ż�Σ���������������ͬ����������
//odd ����ǰ�����Ĵ���
//��ʵ������ż���λ����������������£�����Ҫ��ƥ��һ�㣺���±����޸����ҵĿ�һ���������ң���֮��Ȼ
void patch::GenerateImage(Mat &Dst, Mat Src, Mat Mask, Mat Off, int odd)
{
	if (odd % 2 == 0)//ż��������,���� ���� �����ص�
		for (int i = 0; i < Dst.rows; i++)
			for (int j = 0; j < Dst.cols; j++)
			{
				if (Mask.at < uchar >(i, j) == hole)//ȷ����I��j��Ϊ��������
					Dst.at < Vec3b >(i, j) = GeneratePixel(Dst, Src, Mask, Off,
						i, j, odd);
			}
	else//��������������� ���� �����ص�
		for (int i = Dst.rows - 1; i >= 0; i--)
			for (int j = Dst.cols - 1; j >= 0; j--)
			{
				if (Mask.at < uchar >(i, j) == hole)
					Dst.at < Vec3b >(i, j) = GeneratePixel(Dst, Src, Mask, Off,
						i, j, odd);
			}
}
