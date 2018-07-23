
#include "patch.h"

//获取了一个所有块差平方冒泡排序的中间值
//把随机搜索的所有块按照 相似性 从头到尾从大到小排序
//sim:2个块的差平方
//length:当前迭代次数指数
float patch::GetDelta(float *sim, int length)
{
	//简单copy，不改sim
	float *dist = new float[length];
	for (int i = 0; i< length; i++)
		dist[i] = sim[i];

	//冒泡排序,最后的序列是：从大到小
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

	//获取了一个所有块差平方冒泡排序的中间值
	return dist[(int)(length*0.75)];
}


//获取了B图块中像素的平均值与具体像素的差的平方（3个颜色通道）的和，也就是delta
float GetWindowSize(Vec3b * c, int length)
{
	//获取c存储的块里所有的通道的像素平均
	float allb = 0, allg = 0, allr = 0; //
	for (int i = 0; i< length; i++)
	{
		allb += c[i][0];
		allg += c[i][1];
		allr += c[i][2];
	}
	allb = allb / length;//像素平均
	allg = allg / length;
	allr = allr / length;


	float delta = 0;

	//获取了块中像素的平均值与具体像素的差的平方（3个颜色通道）的和delta
	for (int i = 0; i< length; i++)
	{
		delta +=
			(allb - c[i][0])*(allb - c[i][0]) +
			(allg - c[i][1])*(allg - c[i][1]) +
			(allr - c[i][2])*(allr - c[i][2]);
	}

	return delta;
}

/*利用在多次传播和搜索中找到的块相似差值，来判断块的颜色往最好的移动方向
//进而用这些移动方向，来判断一个“中心点”，这个中心点，就是本算法meansshift发掘的最好的颜色
//返回像素点的3个颜色通道
//c：具体的待修复像素点所在块所有像素的数组存储
//w：一个复杂的参数：w[i] = a[i] * sim[i];
//					sim[i] = exp(-sim[i] / (2 * delta));
//					a[i] = pow(1.3f, -a[i]);
						其中-a[nowlength] = -sqrt(
						(now_row - patch_row) * (now_row - patch_row) * 1.0
						+ (now_col - patch_col) * (now_col - patch_col)
						* 1.0) / ratio;
*/
Vec3b patch::GetMeanshift(float *w, Vec3b * c, int now_length, int odd)
{
	
	float delta = GetWindowSize(c, now_length);
	
	//记录中心点偏移的次数
	float iter = 1;

	//中心点（有3个颜色通道）
	Vec3b centre;

	//初始化“中心点”，odd是搜索的奇偶次数
	if (odd % 2 == 0)
		centre = c[0];//偶数次，块的左上像素
	else
		centre = c[now_length - 1];//块的右下像素


	float *w1 = w;
	Vec3b * c1 = c;
	int length1 = now_length;//当前块的像素点总数

/*此循环的退出条件有2：
	（1）顶峰
	（2）逼近顶峰处
*/
while (true)
{
		float *w2 = new float[length1];
		Vec3b * c2 = new Vec3b[length1];
		int length2 = 0;

		//把符合要求的参数数组（c[],w[]）传递进去
		//这个要求是：差值（具体点 - “中心点”）< delat值
		//在这个循环中，唯一的变量是：centre，也就是“中心点”
		//中心点不断改变，改变的时候把符合条件的周围像素点选入新数组，进行下一步的centre更新
		for (int i = 0; i< length1; i++)
		{
			if ((c1[i][0] - centre[0])*(c1[i][0] - centre[0])
				+ (c1[i][1] - centre[1])*(c1[i][1] - centre[1])
				+ (c1[i][2] - centre[2])*(c1[i][2] - centre[2])
				< iter*iter*delta
				)
			{	
				//获取了符合条件的新数组
				w2[length2] = w1[i];
				c2[length2] = c1[i];
				length2++;//记录新数组的个数，也是下面if的判断依据
			}
		}

		//如果发现不需要更新“中心点”，也就是！到达了上升梯度的峰值处，找不到偏移方向了，退出while循环
		if (length2 == 0)
			break;

		float allb = 0, allg = 0, allr = 0;

		//获取颜色差平均颜色值（c[]数组的像素点 - “中心点”）
		for (int i = 0; i< length2; i++)
		{
			allb += c2[i][0] - centre[0];
			allg += c2[i][1] - centre[1];
			allr += c2[i][2] - centre[2];
		}

		allb = allb / length2;
		allg = allg / length2;
		allr = allr / length2;


		//更新“中心点”，原理是：原来的“中心点”+颜色偏移方向
		Vec3b newcentre = Vec3b(allb + centre[0], allg + centre[1], allr + centre[2]);

		c1 = c2;
		w1 = w2;
		length1 = length2;

		//如果旧的“中心点”和更新的“像素点”差别不大，就不继续找新的“中心点”，直接退出while循环
		if ((newcentre[0] - centre[0])*(newcentre[0] - centre[0])
			+ (newcentre[0] - centre[0])*(newcentre[0] - centre[0])
			+ (newcentre[0] - centre[0])*(newcentre[0] - centre[0])
			<1
			)
			break;
		else
			centre = newcentre;

}//while循环到此结束


	float allp = 0, allb = 0, allg = 0, allr = 0;

	for (int i = 0; i< length1; i++)
	{
		allp += w1[i];
		allb += ((int)c1[i][0])*w1[i];
		allg += ((int)c1[i][1])*w1[i];
		allr += ((int)c1[i][2])*w1[i];
	}

	//返回最后的“中心点”
	return Vec3b((uchar)(allb / allp), (uchar)(allg / allp), (uchar)(allr / allp));
}
