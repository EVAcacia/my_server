#pragma once

#ifndef CPPYUFA
#define CPPYUFA

#include <deque>
#include <string>
#include <assert.h>
using namespace std;

class CppYuFa
{
public:
	//�����ʼ��
	//��ʼ������ʱ������ʡ�ԵȺţ�c++11
	double test1[4]{ 1.2e4, 1.2e4, 1.2e4, 1.2e4 };
	//���ڴ������ڰ����κζ�������Ĭ������Ϊ0
	float test2[100]{};
	//��ʼ����ֹ��խ���������� ���� ��long test3[3] {25, 25, 0.4}    0.4Ϊ�����͡�  


	void putDeque(char* que);//�����
	int frameNumCache = 160;
	std::deque<char*> seqImages;
};


#endif