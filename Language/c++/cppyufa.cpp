

#include "cppYuFa.h"


void CppYuFa::putDeque(char* que)
{
	if (seqImages.size() < frameNumCache)
	{
		seqImages.push_back(que);
	}
	else
	{
		char *tmp = seqImages.front();  //��ʼԪ�ء�
		seqImages.pop_front();//ɾ�����е�һ��Ԫ�ء�
		assert(seqImages.size() < frameNumCache);
		seqImages.push_back(que);
	}
}