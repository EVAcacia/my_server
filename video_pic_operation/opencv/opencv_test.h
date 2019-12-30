
#ifndef TEST
#define TEST

#include"pch.h"
#include <stdio.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>

#include <opencv2/core/core.hpp>

#include<opencv2/video/video.hpp>

#include"opencv2/highgui/highgui.hpp"

#include <string>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>

#include <opencv2/imgproc/imgproc.hpp> 

#include <time.h>
#include <vector>

using namespace cv;
using namespace std;


//������ʾͼ�����
class display
{
public:
	VideoCapture cap;
	VideoWriter outputVideo;  //������������Ƶ��
	double fps;    //FPS�ǲ������ڱ��桢��ʾ��̬��Ƶ����Ϣ������ͨ����������ָÿ��仯�Ļ�������

	VideoWriter saveVideo;  //��Ҫ�������Ƶ

private:

public:
	display();
	//��ȡ��Ƶ��ָ������
	int region_display();
	//ʵʱ��ʾ��Ƶ��
	int realtime_display();
	//ÿ��һ���ӱ���10�����Ƶ
	int save_video();
	
};


//����Ƶ�����ͼƬ����
class picture_my
{
public:
	cv::Mat mat1;
	cv::Mat mat2;
private:


protected:

public:
	picture_my();
	//��һ��ͼƬ��������Ȼ������Զ����ַ�����
	int draw_text_to_picture();
	//����Ƶ����֡���� ��ĳһ֡��д�ֲ������ͼƬ��
	int draw_test(Mat Camera_CImg, char* cstr);
	//��ʴͼ
	int fs_picture();
	//��ӡMat���еľ���
	int value_Mat();
	//�Ƚ�����ͼƬ�Ƿ���ͬ��
	bool cvMatEQ(const cv::Mat& data1, const cv::Mat& data2);
	//ROIʹ�ò���
	int roi_test();
	int save_pic();
};

int comp_picture();   //�Ƚ�����ͼƬ�Ƿ���ͬ

#endif
