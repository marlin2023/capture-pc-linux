/*
 * input_handle.c
 *
 *  Created on: Sep 24, 2012
 *      Author: chris
 */
#include <stdio.h>
#include <stdlib.h>

#include "input_handle.h"
#include "chris_error.h"
//ffmpeg header
#include <libavutil/avutil.h>


void init_audio_device(Input_Context *ptr_input_ctx){
	ptr_input_ctx->audio_fmt_ctx = NULL;

	AVInputFormat *input_fmt = av_find_input_format("alsa");
	if(input_fmt == NULL){
		printf("did not find this audio input devices\n");
		exit(2);
	}
	printf("audio in here ...\n");
	if( avformat_open_input(&ptr_input_ctx->audio_fmt_ctx, AUDIO_INPUT_DEVICE, input_fmt, NULL) != 0){
		printf("inputfile init ,avformat_open_input failed .\n");
		exit(AV_OPEN_INPUT_FAIL);
	}

	printf("after avfomat open input function ...\n");


	//find stream information
	if(avformat_find_stream_info(ptr_input_ctx->audio_fmt_ctx ,NULL) < 0){
		printf("avformat_find_stream_info failed..\n");
		exit(1);
	}

	av_dump_format(ptr_input_ctx->audio_fmt_ctx ,-1 ,NULL ,0);

	int audio_index = -1;
	int i = 0;
	for(i = 0 ;i < ptr_input_ctx->audio_fmt_ctx->nb_streams ; i++){
		printf("i = %d ..\n" ,i);

		if(ptr_input_ctx->audio_fmt_ctx->streams[i]->codec->codec_type ==
				AVMEDIA_TYPE_AUDIO){
			audio_index = i;
			break;
		}
	}

	if(audio_index == -1){
		printf("not find audio stream..\n");
		exit(1);
	}
	ptr_input_ctx->audio_codec_ctx = ptr_input_ctx->audio_fmt_ctx->streams[audio_index]->codec;
	ptr_input_ctx->audio_codec = avcodec_find_decoder(ptr_input_ctx->audio_codec_ctx->codec_id);

	if(ptr_input_ctx->audio_codec == NULL){
		printf("not find audio decoder.\n");
		exit(1);
	}

	if(avcodec_open2(ptr_input_ctx->audio_codec_ctx ,ptr_input_ctx->audio_codec ,NULL) < 0){
		printf("do not open the audio decoder.\n");
		exit(1);
	}

	printf("after open audio decoder..\n");

	ptr_input_ctx->audio_decode_frame = avcodec_alloc_frame();
	if(ptr_input_ctx->audio_decode_frame == NULL){

		printf("audio_decode_frame allocate failed .%s ,%d\n" ,__FILE__ ,__LINE__);
		exit(MEMORY_MALLOC_FAIL);
	}
}

void init_video_device(Input_Context *ptr_input_ctx){
	//open video device
		ptr_input_ctx->video_fmt_ctx = NULL;

		AVInputFormat *input_fmt = av_find_input_format("v4l2"); //video4linux2,v4l2
		if(input_fmt == NULL){
			printf("did not find this input devices\n");
			exit(2);
		}
		printf("in here ...\n");
		if( avformat_open_input(&ptr_input_ctx->video_fmt_ctx, VIDEO_INPUT_DEVICE, input_fmt, NULL) != 0){
			printf("inputfile init ,avformat_open_input failed .\n");
			exit(AV_OPEN_INPUT_FAIL);
		}

		printf("after avfomat open input function ...\n");


		//find stream information
		if(avformat_find_stream_info(ptr_input_ctx->video_fmt_ctx ,NULL) < 0){
			printf("avformat_find_stream_info failed..\n");
			exit(1);
		}

		av_dump_format(ptr_input_ctx->video_fmt_ctx ,-1 ,NULL ,0);

		int video_index = -1;
		int i = 0;
		for(i = 0 ;i < ptr_input_ctx->video_fmt_ctx->nb_streams ; i++){
			printf("i = %d ..\n" ,i);

			if(ptr_input_ctx->video_fmt_ctx->streams[i]->codec->codec_type ==
					AVMEDIA_TYPE_VIDEO){
				video_index = i;
				break;
			}
		}

		if(video_index == -1){
			printf("not find video stream..\n");
			exit(1);
		}
		ptr_input_ctx->video_codec_ctx = ptr_input_ctx->video_fmt_ctx->streams[video_index]->codec;
		ptr_input_ctx->video_codec = avcodec_find_decoder(ptr_input_ctx->video_codec_ctx->codec_id);

		if(ptr_input_ctx->video_codec == NULL){
			printf("not find video decoder.\n");
			exit(1);
		}

		if(avcodec_open2(ptr_input_ctx->video_codec_ctx ,ptr_input_ctx->video_codec ,NULL) < 0){
			printf("do not open the video decoder.\n");
			exit(1);
		}

		printf("after open video decoder..\n");

		ptr_input_ctx->yuv_frame = avcodec_alloc_frame();
		if(ptr_input_ctx->yuv_frame == NULL){

			printf("yuv_frame allocate failed %s ,%d line \n" ,__FILE__ ,__LINE__);
			exit(MEMORY_MALLOC_FAIL);
		}

}
void init_input_devices(Input_Context *ptr_input_ctx){



//	init_audio_devices(ptr_input_ctx);
	while(1);
}



#if 0
void free_input(Input_Context *ptr_input_ctx){

	avformat_close_input(&ptr_input_ctx->ptr_format_ctx);

	avcodec_close(ptr_input_ctx->video_codec_ctx);

	avcodec_close(ptr_input_ctx->audio_codec_ctx);

}


void malloc_input_memory(Input_Context *ptr_input_ctx){
	/*	malloc memory 	*/
	ptr_input_ctx->yuv_frame = avcodec_alloc_frame();
	if(ptr_input_ctx->yuv_frame == NULL){
		printf("yuv_frame allocate failed %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(MEMORY_MALLOC_FAIL);
	}

	//audio frame
	ptr_input_ctx->audio_decode_frame = avcodec_alloc_frame();
	if(ptr_input_ctx->audio_decode_frame == NULL){
		printf("audio_decode_frame allocate failed %s ,%d line\n" ,__FILE__ ,__LINE__);
		exit(MEMORY_MALLOC_FAIL);
	}
}

void free_input_memory(Input_Context *ptr_input_ctx){

	if(ptr_input_ctx->yuv_frame)
		av_free(ptr_input_ctx->yuv_frame);

	if(ptr_input_ctx->audio_decode_frame)
		av_free(ptr_input_ctx->audio_decode_frame);
}


#endif
