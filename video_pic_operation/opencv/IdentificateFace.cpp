#include "pch.h"
#include "IdentificateFace.h"


IdentificateFace::IdentificateFace()
{
}


IdentificateFace::~IdentificateFace()
{
}



//����ʶ�����
int IdentificateFace::face_identify()
{
	Mat img = imread(pic_path);
	//cout << img << endl;
	namedWindow("display");
	imshow("display", img);

	/*********************************** 1.�������������  ******************************/
	// ��������������
	CascadeClassifier cascade;
	// ����ѵ���õ� �����������.xml��
	if (!cascade.load(model_path))
	{
		cout << "cascade load failed!\n";
		return 1;
	}

	//��ʱ
	double t = 0;
	t = (double)getTickCount();
	/*********************************** 2.������� ******************************/
	vector<Rect> faces(0);
	cascade.detectMultiScale(img, faces, 1.1, 2, 0, Size(30, 30));

	cout << "detect face number is :" << faces.size() << endl;
	/********************************  3.��ʾ�������ο� ******************************/

	if (faces.size() > 0)
	{
		for (size_t i = 0; i < faces.size(); i++)
		{
			rectangle(img, faces[i], Scalar(150, 0, 0), 3, 8, 0);

		}
	}
	else cout << "δ��⵽����" << endl;

	t = (double)getTickCount() - t;  //getTickCount():  Returns the number of ticks per second.
	cout << "���������ʱ��" << t * 1000 / getTickFrequency() << "ms (���������ģ�͵�ʱ�䣩" << endl;

	namedWindow("face_detect");
	imshow("face_detect", img);
	while (waitKey(0) != 'k');
	destroyWindow("display");
	destroyWindow("face_detect");
}


//����ʶ�����
//
//Ptr<backgroundsubtractormog2> cv::createBackgroundSubtractorMOG2(
//	int 	history = 500,
//	double 	varThreshold = 16,
//	bool 	detectShadows = true
//)
//Ptr<BackgroundSubtractorMOG2> mog = createBackgroundSubtractorMOG2(100,25,false);   //����ѵ��������֡����Ĭ��Ϊ500֡






//��Ŀ��ʶ��
int IdentificateFace::sport_identify()
{
	VideoCapture video("E://target_file_2.mp4");    //��ȡ�ļ���ע��·����ʽ��VideoCapture video(0); ��ʾʹ�ñ��Ϊ0������ͷ"11.mp4"
	int frameNum = 1;
	Mat frame, mask, thresholdImage, output;
	if (!video.isOpened()) {
		cout << "fail to open!" << endl;
		return -1;
	}
		
	double totalFrameNumber = video.get(CAP_PROP_FRAME_COUNT); //Ӧ���ǵõ���Ƶ��֡��
	video >> frame;
	Ptr<BackgroundSubtractorMOG2> bgsubtractor = createBackgroundSubtractorMOG2();  //��ϸ�˹�ֲ���GMM���Ǳ�����ģ�еľ����㷨
	
	bgsubtractor->setVarThreshold(20);  //// ģ��ƥ����ֵ
	/*bgsubtractor->getBackgroundImage(output);  //��ȡ����
	if (!output.empty()) {
		imshow("mask2", output);
	}*/
	
	
	while (true) {
		if (totalFrameNumber == frameNum)
			break;
		//if (frameNum % 2 == 0) continue;
		video >> frame;
		resize(frame, thresholdImage, cv::Size(1280/4, 720/4), 0, 0, cv::INTER_LINEAR);	//ѹ��ͼƬ
		cvtColor(thresholdImage, output, CV_BGR2GRAY); //ת��BGR��ɫͼΪ�Ҷ�ͼ��
		blur(output, output, Size(7, 7));			   //ʹ��3��3�ں˽��Ҷ�ͼ���롣
		++frameNum;
		//bgSubtractor(frame, mask, 0.001);
		bgsubtractor->apply(output, mask, 0.0009);

		////��ʼ�����ͼ
		//Mat dstImage = Mat::zeros(mask.rows, mask.cols, CV_8UC3);
		////frame ȡ��ֵ����119���ǲ��֡�
		//mask = mask > 119;
		////imshow("ȡ��ֵ���ԭʼͼ", mask);
		////���������Ͳ�νṹ
		//vector<vector<Point>> contours;
		//vector<Vec4i> hierarchy;
		////��������
		//findContours(mask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		////�������ж���������� �������ɫ���Ƴ�ÿ�����������ɫ��
		//int index = 0;
		//for (; index >= 0; index = hierarchy[index][0]) {
		//	Scalar color(rand() & 255, rand() & 255, rand() & 255);
		//	drawContours(dstImage, contours, index, color, FILLED, 8, hierarchy);
		//}
		
		imshow("mask", mask);
		waitKey(10);
	}
error:
	return -1;
success:
	return 1;
}