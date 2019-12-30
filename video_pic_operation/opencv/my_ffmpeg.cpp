#include "pch.h"
#include "my_ffmpeg.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>

extern "C" {
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include "SDL.h"
}

ffmpeg::ffmpeg()
{
	
}


ffmpeg::~ffmpeg()
{
}

static void CopyDate(AVFrame *pFrame, int width, int height, int time)
{
	if (time <= 0) time = 1;

	int		nChannels;
	int		stepWidth;
	uchar  *	pData;
	cv::Mat frameImage(cv::Size(width, height), CV_8UC3, cv::Scalar(0));
	stepWidth = frameImage.step;
	nChannels = frameImage.channels();
	pData = frameImage.data;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pData[i * stepWidth + j * nChannels + 0] = pFrame->data[0][i * pFrame->linesize[0] + j * nChannels + 2];
			pData[i * stepWidth + j * nChannels + 1] = pFrame->data[0][i * pFrame->linesize[0] + j * nChannels + 1];
			pData[i * stepWidth + j * nChannels + 2] = pFrame->data[0][i * pFrame->linesize[0] + j * nChannels + 0];
		}
	}

	cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
	cv::imshow("Video", frameImage);
	cv::waitKey(time);
}

static void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame)
{
	FILE *pFile;
	char szFilename[32];
	int  y;

	// Open file
	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile = fopen(szFilename, "wb");
	if (pFile == NULL)
		return;

	// Write header
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (y = 0; y < height; y++)
		fwrite(pFrame->data[0] + y * pFrame->linesize[0], 1, width * 3, pFile);

	// Close file
	fclose(pFile);
}

//ʹ��ffmpeg������Ƶ
int ffmpeg::test_ffmpeg()
{
	AVFormatContext *pFormatCtx = NULL;
	int             i, videoStream;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame         *pFrame;
	AVFrame         *pFrameRGB;
	AVPacket        packet;
	int             frameFinished;
	int             numBytes;
	uint8_t         *buffer;

	const char * file_name = "E:\\1.avi"; 
	//const char * file_name = "rtsp://admin:a1234567@192.168.1.65:554";   //���������ַ�����rtsp�����Ῠ����
	/*ע����ȫ�����ļ���ʽ�ͱ�������Ŀ�*/
	av_register_all();

	// ����Ƶ�ļ�
	/*���������ȡ�ļ���ͷ�����Ұ���Ϣ���浽���Ǹ���AVFormatContext�ṹ���С�
	 *�ڶ������� Ҫ�򿪵��ļ�·��
	 */
	if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {
		cout << "open video Error!" << endl;
		return -1; // Couldn't open file
	}
	// ��ȡ���ݰ���ȡ��ý���ļ�����Ϣ
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		cout << "avformat_find_stream_info Error!" << endl;
		return -1; // Couldn't find stream information
	}
	//��ӡ����������ʽ�ľ�����Ϣ,��ʱ�䡢������,Ϫ��,����,����,Ԫ����,��������,���������ʱ�䡣
	av_dump_format(pFormatCtx, 0, file_name, false);

	//���ҵ�һ����Ƶ��,�����������Ǹ�ɶ��
	videoStream = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}
	if (videoStream == -1) {//δ������Ƶ���˳�
		return -1;
	}
	//�����Ƶ���������������
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	// �ҵ���Ƶ������
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {//δ������Ƶ������
		return -1;
	}
	// ����Ƶ������
	if (avcodec_open2(pCodecCtx, pCodec, 0) < 0) {
		return -1; //��ʧ���˳�
	}
	// ���䱣����Ƶ֡�Ŀռ�
	pFrame = av_frame_alloc();   //��avcodec_alloc_frame��: �Ҳ�����ʶ����ԭ����������������ˣ�����av_frame_alloc()���ɡ�
	// ����һ��AVFrame�ṹ
	/*׼���������24λRGBɫ��PPM�ļ������Ǳ����֡�ĸ�ʽ��ԭ����ת��ΪRGB��
	FFMPEG��Ϊ��������Щת��*/
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL) {
		return -1;
	}
	/*��ʹ����������һ֡���ڴ棬��ת����ʱ��������Ȼ��Ҫһ���ط�������ԭʼ�����ݡ�
	 *����ʹ��avpicture_get_size�����������Ҫ�Ĵ�С��Ȼ���ֹ������ڴ�ռ䣺
	 */
	numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	// ����ָ����ͼ����,����ͼƬ�ֶ����ṩ��ͼ�����ݻ�������
	avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	/*��ȡ����֡
	 *ͨ����ȡ������ȡ������Ƶ����Ȼ����������֡����ú�ת����ʽ���ұ��档
	 */

	i = 0;
	long prepts = 0;
	while (av_read_frame(pFormatCtx, &packet) >= 0) {
		if (packet.stream_index == videoStream) {/*�ƶ϶�ȡ���Ƿ�Ϊ��Ҫ����Ƶ֡����*/
			// ������Ƶ֡
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			if (frameFinished) {
				static struct SwsContext *img_convert_ctx;

#if 0
				//�͵�ת��ģʽ�Ѿ��ϳ�
				img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, (AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
#endif

				if (img_convert_ctx == NULL) {
					int w = pCodecCtx->width;
					int h = pCodecCtx->height;
					//����ͷ���һ��SwsContext��
					//����Ҫִ������ / ת������ʹ��sws_scale()��
						img_convert_ctx = sws_getContext(w, h, pCodecCtx->pix_fmt, w, h, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
					if (img_convert_ctx == NULL) {
						fprintf(stderr, "Cannot initialize the conversion context!\n");
						exit(1);
					}
				}
				////ת��ͼ���ʽ������ѹ������YUV420P��ͼ��ת��ΪBRG24��ͼ��
				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);

				//����ǰ��֡����
				if (i++ <= 5) {
					SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
				}

				CopyDate(pFrameRGB, pCodecCtx->width, pCodecCtx->height, packet.pts - prepts);
				prepts = packet.pts;
			}
		}

		//�ͷſռ�
		av_free_packet(&packet);
	}

	//�ͷſռ�
	av_free(buffer);
	av_free(pFrameRGB);

	// �ͷ� YUV frame
	av_free(pFrame);

	//�رս�����
	avcodec_close(pCodecCtx);

	//�ر���Ƶ�ļ�
	avformat_close_input(&pFormatCtx);

	//system("Pause");
	return 0;
}


int ffmpeg::display_ffmpeg()
{
	// Init
	//avcodec_register_all()ע���˺ͱ�������йص������Ӳ����������
	//����������������Parser��Bitstream Filter��
	//av_register_all()���˵���avcodec_register_all()֮�⣬��ע���˸��������⸴������Э�鴦������

	//avcodec_register_all();  
	avdevice_register_all();   //ע�������豸������豸��
	av_register_all();     //ffmpegע�Ḵ����,ֻ�е����˸ú���������ʹ�ø��������������ȡ�
	avformat_network_init();


	AVFormatContext *formatContext = avformat_alloc_context();      // Format I/O context. Allocate an AVFormatContext.��Ҫ�洢����Ƶ��װ��ʽ�а�������Ϣ
	//AVInputFormat   *inputFormat = av_find_input_format("dshow");    // or "video4linux2", �洢��������Ƶʹ�õķ�װ��ʽ��ÿ������Ƶ��װ��ʽ����Ӧһ��AVInputFormat�ṹ��

	//if (inputFormat == nullptr) { 
	//	printf("inputFormat  Error");
	//	return -1;
	//}

	//���ý�������Ϊtcp
	AVDictionary *optionsDict = NULL;
	av_dict_set(&optionsDict, "rtsp_transport", "tcp", 0);		//����tcp����

	char rtspUrl[] = "rtsp://admin:a1234567@192.168.1.65:554";
	// Open an input stream and read the header.
	if (avformat_open_input(&formatContext, rtspUrl, NULL, &optionsDict) < 0) {
		fprintf(stderr, "could not open input file\n");
		return -2;
	}

	if (avformat_find_stream_info(formatContext, nullptr) < 0) return -3;

	// Print detailed information about the input or output format.
	av_dump_format(formatContext, 0, rtspUrl, 0);

	// Ѱ����Ƶ���Ͷ�Ӧ�Ľ�����(������ܲ�ֹһ���������������Ƶ����)
	int videoStreamIndex = 0;
	AVCodec *codec;
	if ((videoStreamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0)) < 0) return -4;

	// ��ȡ��Ӧ��Ƶ���Ľ��������Ϣ
	AVCodecContext  *codecContext = formatContext->streams[videoStreamIndex]->codec;
	// Initialize the AVCodecContext to use the given AVCodec.
	if (avcodec_open2(codecContext, codec, nullptr) < 0) return -5;

	// ����ռ�洢ͼ������
	AVFrame *frame = av_frame_alloc();
	AVPicture picture{};

	//// Allocate RGB picture���������������picture��allocʱ��ָ�����ȼ��߶�
	avpicture_alloc(&picture, AV_PIX_FMT_BGR24, codecContext->width, codecContext->height);

	int unfinished;
	AVPacket packet{};
	struct SwsContext * img_convert_ctx = nullptr;
	while (av_read_frame(formatContext, &packet) >= 0)  //��ȡ֡��
	{
		if (packet.stream_index == videoStreamIndex) {
			// Decode the video frame: packet => frame

			//����һ֡��Ƶ���ݡ�����һ��ѹ������Ľṹ��AVPacket�����һ�������Ľṹ��AVFrame
			avcodec_decode_video2(codecContext, frame, &unfinished, &packet);

			if (unfinished) {
				// Convert to BGR24.
				img_convert_ctx = sws_getCachedContext(
					img_convert_ctx,
					codecContext->width, codecContext->height, codecContext->pix_fmt,   //ʼ���ݵĿ�ߺ�ԭʼ���ظ�ʽ(YUV420)
					//Ŀ���Ŀ��ߣ�Ŀ������ظ�ʽ(����Ŀ�߿������ֻ���Ļ�ֱ��ʣ�RGBA8888)�����ﲻ���������˳ߴ��ת�������ظ�ʽ��ת��
					codecContext->width, codecContext->height, AV_PIX_FMT_BGR24,
					SWS_BICUBIC,// flag �ṩ��һϵ�е��㷨���������ԣ���ֵ�����󣬲�ͬ���㷨����Ҳ��ͬ�����������㷨������Խϸߡ�ֻ��Գߴ�ı任�������ظ�ʽת���޴�����
					nullptr, nullptr, nullptr);
				sws_scale(img_convert_ctx,
					reinterpret_cast<const uint8_t *const *>(frame->data), frame->linesize,
					0, codecContext->height,
					picture.data, picture.linesize);

				// Use OpenCV to display the picture.
				cv::Mat img(frame->height, frame->width, CV_8UC3, picture.data[0]);
				cv::imshow("display", img);
				cv::waitKey(10);
			}
			av_free_packet(&packet);
		}
	}

	// Destroy
	sws_freeContext(img_convert_ctx);
	av_free_packet(&packet);
	avcodec_close(codecContext);
	av_free(frame);
	avpicture_free(&picture);
	avformat_close_input(&formatContext);
}



#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit = 0;
int thread_pause = 0;

int sfp_refresh_thread(void *opaque) {
	thread_exit = 0;
	thread_pause = 0;

	while (!thread_exit) {
		if (!thread_pause) {
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(40);
	}
	thread_exit = 0;
	thread_pause = 0;
	//Break
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}

int ffmpeg::display_my_ffmpeg()
{
	//// Init 
	////avdevice_register_all();   //ע�������豸������豸��
	//av_register_all();     //ffmpegע�Ḵ����,ֻ�е����˸ú���������ʹ�ø��������������ȡ�
	//avformat_network_init();


	//AVFormatContext *formatContext;
	//AVFormatContext *fmt_ctx = NULL;

	//if (avformat_open_input(&fmt_ctx, "rtsp", NULL, NULL) < 0) {
	//	cout << "Error!" << endl;
	//	return 1;
	//}

	//if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
	//	cout << "Error1!" << endl;
	//	return 1;
	//}

	//av_dump_format

	//AVPacket pkt = { 0 };
	//av_init_packet(&pkt);

	//pkt.data = NULL;
	//pkt.size = 0;

	//int ret = -1;
	//while (av_read_frame(fmt_ctx, &pkt) >= 0) {
	//	AVPacket orig_pkt = pkt;
	//	do {
	//		ret = avcodec_decode_audio4()
	//	}while(pkt.size > 0)
	//}


	//rtsp://admin:a1234567@192.168.1.65:554
	int result = 0;
	av_register_all(); // ע������FFmpeg����֧�ֵ��ļ���ʽ��codec  
	//avcodec_register_all();

	AVFormatContext* pFormatCtx = NULL;
	const char* filename = "rtsp://admin:a1234567@192.168.1.65:554"; //�����ļ���  

	// step1: ��ý���ļ�,���2������������ָ���ļ���ʽ��buffer��С�͸�ʽ���������ó�NULL�Ļ���libavformat����Զ�ȥ̽������  
	result = avformat_open_input(&pFormatCtx, filename, NULL, NULL);
	if (result != 0)
	{
		cout << "open file fail" << endl;
		return -1;
	}
	cout << "open file succ" << endl;

	// step2: ������Ϣ������Ϣ  
	result = avformat_find_stream_info(pFormatCtx, NULL);
	if (result != 0)
	{
		cout << "find stream fail" << endl;
		return -1;
	}
	cout << "find stream succ" << endl;

	// step3: ��ӡ��Ϣ  
	//av_dump_format(pFormatCtx, 0, filename, 0);  

	// step4���ҵ�video������  
	int i = 0;
	int videoStream = -1;
	AVCodecContext* pCodecCtx = NULL;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;
			break;
		}
	}

	if (videoStream == -1)
	{
		cout << "find stream video fail" << endl;
		return -1;
	}
	cout << "find stream video succ."  << endl;

	// �õ�video�����ʽ  
	pCodecCtx = pFormatCtx->streams[videoStream]->codec;

	// step5: �õ�������  
	AVCodec* pCodec = NULL;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	//pCodec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO); 
	if (pCodec == NULL)
	{
		cout << "find decoder fail" << endl;
		return -1;
	}
	cout << "find decoder succ" << endl;

	result = avcodec_open2(pCodecCtx, pCodec, NULL);
	if (result != 0)
	{
		cout << "open codec fail" << endl;
		return -1;
	}
	cout << "open codec succ" << endl;

	// step6: ����ԭʼ����֡ �� RGB֡�ڴ�  
	AVFrame* pFrame = NULL;
	AVFrame* pFrameRGB = NULL;
	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();
	if (pFrame == NULL || pFrameRGB == NULL)
	{
		return -1;
	}

	int numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	cout << "pCodecCtx->width: " << pCodecCtx->width << "\npCodecCtx->height: " << pCodecCtx->height << "\nnumBytes is :" << numBytes << endl;
	uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture*)pFrameRGB, buffer, AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	int frameFinishsed = 0;
	AVPacket packet;
	i = 0;

	// step7: ������ʽת���ı�  
	SwsContext * pSwxCtx = sws_getContext(
		pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB24,
		SWS_BILINEAR, 0, 0, 0);

	cv::Mat image(pCodecCtx->height, pCodecCtx->width, CV_8UC3);  
	int b = 0;
	int g = 1;
	int r = 2;

	while (true)
	{
		// �õ����ݰ�  
		result = av_read_frame(pFormatCtx, &packet);
		if (result != 0)
		{
			break;
		}
		if (packet.stream_index == videoStream)
		{
			// ����  
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinishsed, &packet);
			if (frameFinishsed)
			{
				// ת��  
				sws_scale(pSwxCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);

				 for (int m = 0; m < pCodecCtx->height; m++)  
				 {  
				     for (int n = 0; n < pCodecCtx->width; n++)  
				     {  
				         image.at<Vec3b>(m, n)[r] = pFrameRGB->data[0][3 * (m*pCodecCtx->width + n) + 0];  
				         image.at<Vec3b>(m, n)[g] = pFrameRGB->data[0][3 * (m*pCodecCtx->width + n) + 1];  
				         image.at<Vec3b>(m, n)[b] = pFrameRGB->data[0][3 * (m*pCodecCtx->width + n) + 2];  
				     }  
				 }  
				 cv::imshow("pic", image);  
				 cv::waitKey(1);  
			}
		}

		av_free_packet(&packet);
	}

	avformat_close_input(&pFormatCtx);
	return 1;
}

//int ffmpeg::display_ffmpeg()
//{
//	
//	
//	AVFormatContext	*pFormatCtx;
//	int				i, videoindex;
//	AVCodecContext	*pCodecCtx;
//	AVCodec			*pCodec;
//	AVFrame	*pFrame, *pFrameYUV;
//	unsigned char *out_buffer;
//	AVPacket *packet;
//	int ret, got_picture;
//
//	//------------SDL----------------
//	int screen_w, screen_h;
//	SDL_Window *screen;
//	SDL_Renderer* sdlRenderer;
//	SDL_Texture* sdlTexture;
//	SDL_Rect sdlRect;
//	SDL_Thread *video_tid;
//	SDL_Event event;
//
//	struct SwsContext *img_convert_ctx;
//
//	//char filepath[]="bigbuckbunny_480x272.h265";
//	char filepath[] = "rtsp://admin:a1234567@192.168.1.65:554/MPEG-4/ch1/main/av_stream/?tcp";
//
//	av_register_all();
//	avformat_network_init();
//	pFormatCtx = avformat_alloc_context();
//
//	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
//		printf("Couldn't open input stream.\n");
//		return -1;
//	}
//	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
//		printf("Couldn't find stream information.\n");
//		return -1;
//	}
//	videoindex = -1;
//	for (i = 0; i < pFormatCtx->nb_streams; i++)
//		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
//			videoindex = i;
//			break;
//		}
//	if (videoindex == -1) {
//		printf("Didn't find a video stream.\n");
//		return -1;
//	}
//	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
//	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
//	if (pCodec == NULL) {
//		printf("Codec not found.\n");
//		return -1;
//	}
//	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
//		printf("Could not open codec.\n");
//		return -1;
//	}
//	pFrame = av_frame_alloc();
//	pFrameYUV = av_frame_alloc();
//
//	out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
//	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
//		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
//
//	//Output Info-----------------------------
//	printf("---------------- File Information ---------------\n");
//	av_dump_format(pFormatCtx, 0, filepath, 0);
//	printf("-------------------------------------------------\n");
//
//	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
//
//
//	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
//		printf("Could not initialize SDL - %s\n", SDL_GetError());
//		return -1;
//	}
//	//SDL 2.0 Support for multiple windows
//	screen_w = pCodecCtx->width;
//	screen_h = pCodecCtx->height;
//	screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//		screen_w, screen_h, SDL_WINDOW_OPENGL);
//
//	if (!screen) {
//		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
//		return -1;
//	}
//	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
//	//IYUV: Y + U + V  (3 planes)
//	//YV12: Y + V + U  (3 planes)
//	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
//
//	sdlRect.x = 0;
//	sdlRect.y = 0;
//	sdlRect.w = screen_w;
//	sdlRect.h = screen_h;
//
//	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//
//	video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
//	//------------SDL End------------
//	//Event Loop
//
//	for (;;) {
//		//Wait
//		SDL_WaitEvent(&event);
//		if (event.type == SFM_REFRESH_EVENT) {
//			while (1) {
//				if (av_read_frame(pFormatCtx, packet) < 0)
//					thread_exit = 1;
//
//				if (packet->stream_index == videoindex)
//					break;
//			}
//			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
//			if (ret < 0) {
//				printf("Decode Error.\n");
//				return -1;
//			}
//			if (got_picture) {
//				sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
//				//SDL---------------------------
//				SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
//				SDL_RenderClear(sdlRenderer);
//				//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
//				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
//				SDL_RenderPresent(sdlRenderer);
//				//SDL End-----------------------
//			}
//			av_free_packet(packet);
//}
//		else if (event.type == SDL_KEYDOWN) {
//			//Pause
//			if (event.key.keysym.sym == SDLK_SPACE)
//				thread_pause = !thread_pause;
//		}
//		else if (event.type == SDL_QUIT) {
//			thread_exit = 1;
//		}
//		else if (event.type == SFM_BREAK_EVENT) {
//			break;
//		}
//
//	}
//
//	sws_freeContext(img_convert_ctx);
//
//	SDL_Quit();
//	//--------------
//	av_frame_free(&pFrameYUV);
//	av_frame_free(&pFrame);
//	avcodec_close(pCodecCtx);
//	avformat_close_input(&pFormatCtx);
//
//	return 0;
//}




