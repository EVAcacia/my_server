
#include"pch.h"
#include "opencv_test.h"



//��ʾMat�� ����
int picture_my::value_Mat()
{
	//��ͨ��
	Mat Z = Mat::zeros(2, 3, CV_8UC1);
	cout << "Z = " << endl << " " << Z << endl;
	Mat O = Mat::ones(2, 3, CV_32F);
	cout << "O = " << endl << " " << O << endl;
	Mat E = Mat::eye(2, 3, CV_64F);
	cout << "E = " << endl << " " << E << endl;

	//��������Ϊ 3������Ϊ 2��ͨ����Ϊ 5 ��ͼ��
	Mat M(600, 800, CV_8UC(3));
	imshow("test", M);
	return 0;
}

//��ȡһ���ı��β������ͼƬ
int picture_my::save_pic()
{
	Mat srcImage = imread("E:\\1.jpg");
	Mat dstImage = imread("E:\\2.jpg");
	Mat dst;
	if (!srcImage.data) {
		cout << "Error1" << endl;
		return 1;
	}
	if (!dstImage.data) {
		cout << "Error2" << endl;
		return 1;
	}
	/*Mat roi = Mat::zeros(srcImage.size(), CV_8U);
	vector<vector<Point>> contour;
	vector<Point> pts;
	pts.push_back(Point(30, 45));
	pts.push_back(Point(100, 15));
	pts.push_back(Point(300, 145));
	pts.push_back(Point(330, 240));
	pts.push_back(Point(50, 250));
	contour.push_back(pts);
	drawContours(roi, contour, 0, Scalar::all(255), -1);
	srcImage.copyTo(dst, roi);*/

	//����һ��Mat���ͣ���������ROI����
	Mat imageROI = srcImage(Rect(200, 250, dstImage.cols, dstImage.rows));
	imshow("old", imageROI);
	imwrite("./save_pic.jpg", imageROI);
	waitKey(0);
	return 0;
}

//ROI����
int picture_my::roi_test()
{
	int div = 50;

	Mat srcImage = imread("E:\\1.jpg");
	Mat dstImage = imread("E:\\2.jpg");
	if (!srcImage.data) {
		cout << "Error1" << endl;
		return 1;
	}
	if (!dstImage.data) {
		cout << "Error2" << endl;
		return 1;
	}

	//����һ��Mat���ͣ���������ROI����
	Mat imageROI = srcImage(Rect(200, 250, dstImage.cols, dstImage.rows));
	//������ģ
	Mat mask = imread("E:\\2.jpg", 0);   //��ͼƬ���س���ģ
	//����ģ���Ƶ�ROI
	dstImage.copyTo(imageROI, mask);


	imshow("old", srcImage);

	imshow("new", srcImage);
	waitKey(0);
	return 0;
}

//��ͼƬ���д���
int picture_my::fs_picture()
{
	int div = 50;

	Mat srcImage = imread("E:\\1.jpg");
	Mat dstImage;

	int nl = srcImage.rows;  //ԭͼ������
	int nc = srcImage.cols * srcImage.channels(); // ÿ�е�Ԫ�ظ�����ÿ�е�������*��ɫͨ����
		
	imshow("old", srcImage);
	//��ʴ
	/*Mat fs = getStructuringElement(MORPH_RECT, Size(15, 15));
	erode(srcImage, dstImage, fs);*/
	//�˲�/ģ��
	/*blur(srcImage, dstImage, Size(15, 15));*/

	//�����ο�
	for (int j = 0; j < nl; j++) {
		if ((j > 100 && j < 120) || (j > 250 && j < 270)) {
			uchar* data = srcImage.ptr<uchar>(j);
			for (int i = 0; i < nc; i++) {
				if ((i > 90 && i<110) || (i>220 && i<240)) {
					data[i] = 0;
				}
			}
		}
	}

	////��Ե����
	//Mat im = imread("E:\\1.jpg", 0); //�����ļ���ת�ɵ�ͨ��ģʽ��
	////��һ������Ƿ�ɹ���ͼ
	//if (im.empty())
	//{
	//	cout << "Can not load image." << endl;
	//	return -1;
	//}
	////���� Canny ����������������� result
	//Mat result;
	//Canny(im, result, 50, 150);
	
	imshow("new", srcImage);
	waitKey(0);
	return 0;
}



int picture_my::draw_test(Mat Camera_CImg, char* cstr)
{
	//ʱ���ַ���
	char time_tmp[64] = "0";

	if (Camera_CImg.empty()) {
		throw("Faild open file.");
		return -1;
	}

	//�����ַ�����ʼ��
	cv::Point p = cv::Point(50, 100);

	//��ȡʱ�䡣
	time_t tt = time(NULL);//��䷵�ص�ֻ��һ��ʱ��cuo
	tm* t = localtime(&tt);
	sprintf(time_tmp, "%d-%02d-%02d %02d:%02d:%02d", t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);

	cv::putText(Camera_CImg, time_tmp, p, cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(255, 200, 200), 2, CV_AA);
	//��ͼ���ϼ��ַ�
	//��һ������ΪҪ���ַ���Ŀ�꺯��
	//�ڶ�������ΪҪ�ӵ��ַ�
	//����������Ϊ����
	//���ĸ�����Ϊ�ӵĴ�ϸ
	//���������Ϊ�ַ�����ɫ

	cv::imwrite(cstr, Camera_CImg);
	cout << cstr << endl;
	//cv::waitKey();
	return 0;
}

picture_my::picture_my()
{
	const char* filename = "E:\\1.jpg";
	const char* filename2 = "E:\\2.jpg";
	mat1 = cv::imread(filename);
	if (mat1.empty()) {
		throw("Faild open file.");
	}

	mat2 = cv::imread(filename2);
	if (mat2.empty()) {
		throw("Faild open file2.");
	}
}

int picture_my::draw_text_to_picture()
{
	const char* filename = "E:\\1.jpg";

	cv::Mat mat = cv::imread(filename);
	if (mat.empty()) {
		throw("Faild open file.");
		return -1;
	}

	cv::Point p = cv::Point(50, 50);
	//�����ַ�����ʼ��
	cv::putText(mat, "Hello OpenCV", p, cv::FONT_HERSHEY_TRIPLEX, 0.8, cv::Scalar(255, 200, 200), 2, CV_AA);
	//��ͼ���ϼ��ַ�
	//��һ������ΪҪ���ַ���Ŀ�꺯��
	//�ڶ�������ΪҪ�ӵ��ַ�
	//����������Ϊ����
	//���ĸ�����Ϊ�ӵĴ�ϸ
	//���������Ϊ�ַ�����ɫ

	//cv::imshow("mat", mat);
	cv::imwrite("E:\\1_xg.jpg", mat);

	cv::waitKey();

	return 0;
}


display::display()
{
	cap.open(VIDEO);
}

int display::realtime_display()
{

	Mat Camera_GImg;
	Mat Camera_CImg;

	int n = 1;
	while (1)
	{
		cap >> Camera_CImg;
		if (Camera_CImg.empty())
			break;

		//ת�Ҷ�ͼ
		cvtColor(Camera_CImg, Camera_GImg, CV_BGR2GRAY);

		//ʵʱ��ʾ��Ƶ
		imshow("input", Camera_GImg);

		if (cvWaitKey(10) == 'q')
			break;
	}
	cap.release();
	return 0;
}



int display::region_display()
{
	outputVideo.open("E:\\1.avi", CV_FOURCC('M', 'P', 'E', 'G'), 30, Size(900, 600), true);
	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: " << endl;
		return -1;
	}

	//ROI
	Rect box;
	box.width = 900;
	box.height = 600;
	box.x = 500;
	box.y = 100;

	while (char(waitKey(1)) != 'q' && cap.isOpened())
	{
		Mat frame;
		cap >> frame;
		// check if video is over
		if (frame.empty())
		{
			cout << "video over" << endl;
			break;
		}
		Mat crop(frame, box);
		outputVideo << crop;
		imshow("video", frame);
		imshow("crop_video", crop);
	}
	return 0;
}

//ÿ��һ���ӱ���10�����Ƶ
int display::save_video()
{
	double rate = cap.get(CV_CAP_PROP_FPS);

	long nFrame = static_cast<long>(cap.get(CV_CAP_PROP_FRAME_COUNT)); // ��ȡ��֡��
	cout << "FPS:" << rate << endl;

	//ROI
	Rect box;
	box.width = 900;
	box.height = 600;
	box.x = 500;
	box.y = 100;
	int frame_cnt = 0;
	bool if_stop = 0;

	for (int i = 1; i < 5; i++) {
		if_stop = 0;
		frame_cnt = 0;

		char tmp[32] = "0";
		sprintf(tmp, "%s%d%s", "E:\\", i, ".avi");
		outputVideo.open(tmp, CV_FOURCC('X', 'V', 'I', 'D'), 25, Size(900, 600), true);
		if (!outputVideo.isOpened()) {
			cout << "Could not open the output video for write: " << endl;
			return -1;
		}

		while (char(waitKey(1)) != 'q' && cap.isOpened()) {
			if (if_stop == 1) {
				break;
			}

			Mat frame;
			cap >> frame;
			// check if video is over
			if (frame.empty()) {
				cout << "video over" << endl;
				break;
			}

			/*if (frame_cnt % 250 >= 0  && frame_cnt % 250 <= 249)*/
			if (frame_cnt >= 1500 && frame_cnt <= 1750) {
				Mat crop(frame, box);
				outputVideo << crop;
				imshow("crop_video", crop);
				frame_cnt++;
			}
			else if (frame_cnt > 1750) {
				if_stop = 1;
				break;
			}
			else {
				frame_cnt++;
				imshow("crop_video", frame);
			}
		}
	}
	return 0;
}

//�Ƚ�������Ƭ�����ƶ�
bool picture_my::cvMatEQ(const cv::Mat& data1, const cv::Mat& data2)
{
	bool success = true;
	// check if is multi dimensional
	if (data1.dims > 2 || data2.dims > 2)
	{
		if (data1.dims != data2.dims || data1.type() != data2.type())
		{
			return false;
		}
		for (int dim = 0; dim < data1.dims; dim++) {
			if (data1.size[dim] != data2.size[dim]) {
				return false;
			}
		}
	}
	else
	{
		if (data1.size() != data2.size() || data1.channels() != data2.channels() || data1.type() != data2.type()) {
			return false;
		}
	}
	int nrOfElements = data1.total()*data1.elemSize1();
	//bytewise comparison of data
	int cnt = 0;
	for (cnt = 0; cnt < nrOfElements && success; cnt++)
	{
		if (data1.data[cnt] != data2.data[cnt]) {
			success = false;
		}
	}
	return success;
}


//�Ƚ�������Ƭ�����ƶ�
int comp_picture()
{
	string strSrcImageName = "E:\\1.jpg";
	string strSrcImageName2 = "E:\\2.jpg";

	cv::Mat matSrc, matSrc1, matSrc2;

	matSrc1 = cv::imread(strSrcImageName, CV_LOAD_IMAGE_COLOR);
	CV_Assert(matSrc1.channels() == 3);
	matSrc2 = cv::imread(strSrcImageName2, CV_LOAD_IMAGE_COLOR);
	CV_Assert(matSrc2.channels() == 3);

	/*cv::resize(matSrc, matSrc1, cv::Size(357, 419), 0, 0, cv::INTER_NEAREST);
	cv::resize(matSrc, matSrc2, cv::Size(2177, 3233), 0, 0, cv::INTER_LANCZOS4);*/

	cv::Mat matDst1, matDst2;

	cv::resize(matSrc1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
	cv::resize(matSrc2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
	//update 20181206 for the bug cvtColor
	cv::Mat temp1 = matDst1;
	cv::Mat temp2 = matDst2;
	cv::cvtColor(temp1, matDst1, CV_BGR2GRAY);
	cv::cvtColor(temp2, matDst2, CV_BGR2GRAY);

	imshow("1", temp1);
	imshow("2", temp2);

	int iAvg1 = 0, iAvg2 = 0;
	int arr1[64], arr2[64];

	for (int i = 0; i < 8; i++)
	{
		uchar* data1 = matDst1.ptr<uchar>(i);
		uchar* data2 = matDst2.ptr<uchar>(i);

		int tmp = i * 8;

		for (int j = 0; j < 8; j++)
		{
			int tmp1 = tmp + j;

			arr1[tmp1] = data1[j] / 4 * 4;
			arr2[tmp1] = data2[j] / 4 * 4;

			iAvg1 += arr1[tmp1];
			iAvg2 += arr2[tmp1];
		}
	}

	iAvg1 /= 64;
	iAvg2 /= 64;

	for (int i = 0; i < 64; i++)
	{
		arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
		arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
	}

	int iDiffNum = 0;

	for (int i = 0; i < 64; i++)
		if (arr1[i] != arr2[i])
			++iDiffNum;

	cout << "iDiffNum = " << iDiffNum << endl;

	if (iDiffNum <= 7)
		cout << "two images are very similar!" << endl;
	else if (iDiffNum > 20)
		cout << "they are two different images!" << endl;
	else
		cout << "two image are somewhat similar!" << endl;

	getchar();
	return 0;
}
