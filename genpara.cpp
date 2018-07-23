
#include "patch.h"

//��ȡ��һ�����п��ƽ��ð��������м�ֵ
//��������������п鰴�� ������ ��ͷ��β�Ӵ�С����
//sim:2����Ĳ�ƽ��
//length:��ǰ��������ָ��
float patch::GetDelta(float *sim, int length)
{
	//��copy������sim
	float *dist = new float[length];
	for (int i = 0; i< length; i++)
		dist[i] = sim[i];

	//ð������,���������ǣ��Ӵ�С
	for (int i = 0; i< length - 1; i++)
	{
		for (int j = 0; j< length - 1 - i; j++)
		{
			if (dist[j] > dist[j + 1])
			{
				float tmp = dist[j + 1];
				dist[j + 1] = dist[j];
				dist[j] = tmp;
			}
		}
	}

	//��ȡ��һ�����п��ƽ��ð��������м�ֵ
	return dist[(int)(length*0.75)];
}


//��ȡ��Bͼ�������ص�ƽ��ֵ��������صĲ��ƽ����3����ɫͨ�����ĺͣ�Ҳ����delta
float GetWindowSize(Vec3b * c, int length)
{
	//��ȡc�洢�Ŀ������е�ͨ��������ƽ��
	float allb = 0, allg = 0, allr = 0; //
	for (int i = 0; i< length; i++)
	{
		allb += c[i][0];
		allg += c[i][1];
		allr += c[i][2];
	}
	allb = allb / length;//����ƽ��
	allg = allg / length;
	allr = allr / length;


	float delta = 0;

	//��ȡ�˿������ص�ƽ��ֵ��������صĲ��ƽ����3����ɫͨ�����ĺ�delta
	for (int i = 0; i< length; i++)
	{
		delta +=
			(allb - c[i][0])*(allb - c[i][0]) +
			(allg - c[i][1])*(allg - c[i][1]) +
			(allr - c[i][2])*(allr - c[i][2]);
	}

	return delta;
}

/*�����ڶ�δ������������ҵ��Ŀ����Ʋ�ֵ�����жϿ����ɫ����õ��ƶ�����
//��������Щ�ƶ��������ж�һ�������ĵ㡱��������ĵ㣬���Ǳ��㷨meansshift�������õ���ɫ
//�������ص��3����ɫͨ��
//c������Ĵ��޸����ص����ڿ��������ص�����洢
//w��һ�����ӵĲ�����w[i] = a[i] * sim[i];
//					sim[i] = exp(-sim[i] / (2 * delta));
//					a[i] = pow(1.3f, -a[i]);
						����-a[nowlength] = -sqrt(
						(now_row - patch_row) * (now_row - patch_row) * 1.0
						+ (now_col - patch_col) * (now_col - patch_col)
						* 1.0) / ratio;
*/
Vec3b patch::GetMeanshift(float *w, Vec3b * c, int now_length, int odd)
{
	
	float delta = GetWindowSize(c, now_length);
	
	//��¼���ĵ�ƫ�ƵĴ���
	float iter = 1;

	//���ĵ㣨��3����ɫͨ����
	Vec3b centre;

	//��ʼ�������ĵ㡱��odd����������ż����
	if (odd % 2 == 0)
		centre = c[0];//ż���Σ������������
	else
		centre = c[now_length - 1];//�����������


	float *w1 = w;
	Vec3b * c1 = c;
	int length1 = now_length;//��ǰ������ص�����

/*��ѭ�����˳�������2��
	��1������
	��2���ƽ����崦
*/
while (true)
{
		float *w2 = new float[length1];
		Vec3b * c2 = new Vec3b[length1];
		int length2 = 0;

		//�ѷ���Ҫ��Ĳ������飨c[],w[]�����ݽ�ȥ
		//���Ҫ���ǣ���ֵ������� - �����ĵ㡱��< delatֵ
		//�����ѭ���У�Ψһ�ı����ǣ�centre��Ҳ���ǡ����ĵ㡱
		//���ĵ㲻�ϸı䣬�ı��ʱ��ѷ�����������Χ���ص�ѡ�������飬������һ����centre����
		for (int i = 0; i< length1; i++)
		{
			if ((c1[i][0] - centre[0])*(c1[i][0] - centre[0])
				+ (c1[i][1] - centre[1])*(c1[i][1] - centre[1])
				+ (c1[i][2] - centre[2])*(c1[i][2] - centre[2])
				< iter*iter*delta
				)
			{	
				//��ȡ�˷���������������
				w2[length2] = w1[i];
				c2[length2] = c1[i];
				length2++;//��¼������ĸ�����Ҳ������if���ж�����
			}
		}

		//������ֲ���Ҫ���¡����ĵ㡱��Ҳ���ǣ������������ݶȵķ�ֵ�����Ҳ���ƫ�Ʒ����ˣ��˳�whileѭ��
		if (length2 == 0)
			break;

		float allb = 0, allg = 0, allr = 0;

		//��ȡ��ɫ��ƽ����ɫֵ��c[]��������ص� - �����ĵ㡱��
		for (int i = 0; i< length2; i++)
		{
			allb += c2[i][0] - centre[0];
			allg += c2[i][1] - centre[1];
			allr += c2[i][2] - centre[2];
		}

		allb = allb / length2;
		allg = allg / length2;
		allr = allr / length2;


		//���¡����ĵ㡱��ԭ���ǣ�ԭ���ġ����ĵ㡱+��ɫƫ�Ʒ���
		Vec3b newcentre = Vec3b(allb + centre[0], allg + centre[1], allr + centre[2]);

		c1 = c2;
		w1 = w2;
		length1 = length2;

		//����ɵġ����ĵ㡱�͸��µġ����ص㡱��𲻴󣬾Ͳ��������µġ����ĵ㡱��ֱ���˳�whileѭ��
		if ((newcentre[0] - centre[0])*(newcentre[0] - centre[0])
			+ (newcentre[0] - centre[0])*(newcentre[0] - centre[0])
			+ (newcentre[0] - centre[0])*(newcentre[0] - centre[0])
			<1
			)
			break;
		else
			centre = newcentre;

}//whileѭ�����˽���


	float allp = 0, allb = 0, allg = 0, allr = 0;

	for (int i = 0; i< length1; i++)
	{
		allp += w1[i];
		allb += ((int)c1[i][0])*w1[i];
		allg += ((int)c1[i][1])*w1[i];
		allr += ((int)c1[i][2])*w1[i];
	}

	//�������ġ����ĵ㡱
	return Vec3b((uchar)(allb / allp), (uchar)(allg / allp), (uchar)(allr / allp));
}
