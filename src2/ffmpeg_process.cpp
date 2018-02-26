
#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
	//Windows
	extern "C"
	{
		#include "libavcodec/avcodec.h"
		#include "libavformat/avformat.h"
		#include "libswscale/swscale.h"
		#include "SDL2/SDL.h"
	};
#else
	//Linux...
	#ifdef __cplusplus
	extern "C"
	{
	#endif
		#include <libavcodec/avcodec.h>
		#include <libavformat/avformat.h>
		#include <libswscale/swscale.h>
		#include <SDL2/SDL.h>
	#ifdef __cplusplus
	};
	#endif
#endif

//#define USE_JPEG


#include <time.h>

/**
 * 将AVFrame(YUV420格式)保存为JPEG格式的图片
 *
 * @param width YUV420的宽
 * @param height YUV42的高
 *
 */
#define MAX_PATH 32
int MyWriteJPEG(AVFrame* pFrame, int width, int height, int iIndex)
{
	// 输出文件路径
	char out_file[MAX_PATH] = {0};
	sprintf(out_file, "%s%d.jpg", "test_", iIndex);

	// 分配AVFormatContext对象
	AVFormatContext* pFormatCtx = avformat_alloc_context();

	// 设置输出文件格式
	pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
	// 创建并初始化一个和该url相关的AVIOContext
	if( avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
		printf("Couldn't open output file.");
		return -1;
	}

	// 构建一个新stream
	AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
	if( pAVStream == NULL ) {
		return -1;
	}

	// 设置该stream的信息
	AVCodecContext* pCodecCtx = pAVStream->codec;

	pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = PIX_FMT_YUVJ420P;
	pCodecCtx->width = width;
	pCodecCtx->height = height;
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = 25;

	// Begin Output some information
	av_dump_format(pFormatCtx, 0, out_file, 1);
	// End Output some information

	// 查找解码器
	AVCodec* pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
	if( !pCodec ) {
		printf("Codec not found.");
		return -1;
	}
	// 设置pCodecCtx的解码器为pCodec
	if( avcodec_open2(pCodecCtx, pCodec, NULL) < 0 ) {
		printf("Could not open codec.");
		return -1;
	}

	//Write Header
	avformat_write_header(pFormatCtx, NULL);

	int y_size = pCodecCtx->width * pCodecCtx->height;

	//Encode
	// 给AVPacket分配足够大的空间
	AVPacket pkt;
	av_new_packet(&pkt, y_size * 3);

	// 
	int got_picture = 0;
	int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
	if( ret < 0 ) {
		printf("Encode Error.\n");
		return -1;
	}
	if( got_picture == 1 ) {
		//pkt.stream_index = pAVStream->index;
		ret = av_write_frame(pFormatCtx, &pkt);
	}

	av_free_packet(&pkt);

	//Write Trailer
	av_write_trailer(pFormatCtx);

	printf("Encode Successful.\n");

	if( pAVStream ) {
		avcodec_close(pAVStream->codec);
	}
	avio_close(pFormatCtx->pb);
	avformat_free_context(pFormatCtx);
	return 0;
}

int main(int argc, char* argv[])
{

	AVFormatContext *pFormatCtx;
	int             i, videoindex;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	uint8_t *out_buffer;
	AVPacket *packet;
	int ret, got_picture;


	struct SwsContext *img_convert_ctx;
	// 改成你自己的 URL
	//char filepath[] = "rtsp://admin:xxxxx@192.168.10.180:554/ch1/main/av_stream";
	char filepath[]="ws.mp4";

	av_register_all();//注册所有组件
	avformat_network_init();//初始化网络
	pFormatCtx = avformat_alloc_context();//初始化一个AVFormatContext

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)////打开网络流或文件流
	{
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL)<0)//获取视频文件信息
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);//查找解码器
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
	{
		printf("Could not open codec.\n");
		return -1;
	}
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

#ifndef USE_JPEG
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	//Output Info---输出一些文件（RTSP）信息  
	printf("---------------- File Information ---------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
			pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
#else

	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height);

	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx,0,filepath,0);
	printf("-------------------------------------------------\n");
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
			pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUVJ420P, SWS_BICUBIC, NULL, NULL, NULL); 
#endif

	FILE *fp_yuv=fopen("output.yuv","wb+");  
	FILE *fp_h264=fopen("output.h264","wb+");
	int y_size;

	time_t ts, te, dur;
	ts=time(NULL);

	packet=(AVPacket *)av_malloc(sizeof(AVPacket));
	while(av_read_frame(pFormatCtx, packet)>=0){//读取一帧压缩数据
		if(packet->stream_index==videoindex){

			fwrite(packet->data,1,packet->size,fp_h264); //把H264数据写入fp_h264文件

			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);//解码一帧压缩数据
			if(ret < 0){
				printf("Decode Error.\n");
				return -1;
			}

            if(got_picture){

                static int cnt=0;
                cnt++;
                MyWriteJPEG(pFrame, pCodecCtx->width, pCodecCtx->height, cnt);

                if (cnt%100==0){
                    te=time(NULL);
                    dur = te-ts;
                    ts = te;
                    printf("##@@## fps=%ld\n", long(100.0/dur));
                }
#ifdef USE_JPEG
                char fileout[20];
                sprintf(fileout, "test%d.jpeg", cnt);
                FILE *jpeg_file=fopen(fileout,"wb+");
                y_size=pCodecCtx->width*pCodecCtx->height;
                fwrite(pFrame->data[0],1,y_size,jpeg_file);
                fclose(jpeg_file);
                printf("   pFrame: linesize[0]=%d, linesize[1]=%d!\n", pFrame->linesize[0], pFrame->linesize[1]);
                printf("   pFrameYUV: linesize[0]=%d, linesize[1]=%d!\n", pFrameYUV->linesize[0], pFrameYUV->linesize[1]);
#endif

                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                        pFrameYUV->data, pFrameYUV->linesize);
#ifndef USE_JPEG
                y_size=pCodecCtx->width*pCodecCtx->height;
                fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
                fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
                fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
#endif


                printf("Succeed to decode 1 frame, wxh=%dx%d!\n", pCodecCtx->width, pCodecCtx->height);

            }
		}
		av_free_packet(packet);
	}
	//flush decoder
	/*当av_read_frame()循环退出的时候，实际上解码器中可能还包含剩余的几帧数据。
	  因此需要通过“flush_decoder”将这几帧数据输出。
	  “flush_decoder”功能简而言之即直接调用avcodec_decode_video2()获得AVFrame，而不再向解码器传递AVPacket。*/
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
				pFrameYUV->data, pFrameYUV->linesize);

		int y_size=pCodecCtx->width*pCodecCtx->height;
		fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
		fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
		fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V

		printf("Flush Decoder: Succeed to decode 1 frame!\n");
	}

	sws_freeContext(img_convert_ctx);

	//关闭文件以及释放内存
	fclose(fp_yuv);
	fclose(fp_h264);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

    return 0;
}
