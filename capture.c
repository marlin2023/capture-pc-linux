/*
 * capture_main.c
 *
 *  Created on: Feb 24, 2013
 *      Author: chris
 */

#include <stdio.h>

#include "capture.h"
#include "chris_global.h"
#include "chris_error.h"
#include "segment_yy.h"
#include "input_handle.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"

void audio_capture(Segment_U * seg_union){

//	//init audio device
//	init_audio_device(seg_union->input_ctx);
	while(av_read_frame(seg_union->input_ctx->audio_fmt_ctx ,
			&seg_union->input_ctx->pkt_audio) >= 0){

		while(seg_union->input_ctx->pkt_audio.size > 0){

			int got_frame = 0;
			int len = avcodec_decode_audio4(seg_union->input_ctx->audio_codec_ctx ,
					seg_union->input_ctx->audio_decode_frame ,&got_frame ,
					&seg_union->input_ctx->pkt_audio);

			if(len < 0){
				fprintf(stderr ,"Error while decoding audio frame.\n");
				break;
			}

			if(got_frame){

				do_audio_out(seg_union->output_ctx ,seg_union->input_ctx->audio_decode_frame ,
						48000);
			}else{
				printf("avcodec_decode_audio4 ,no data .\n");
				continue;
			}

			seg_union->input_ctx->pkt_audio.size -= len;
			seg_union->input_ctx->pkt_audio.data += len;
		}
	}


}

void video_capture(Segment_U * seg_union){

	//init video device
//	init_video_device(seg_union->input_ctx);
	AVCodec *codec = seg_union->input_ctx->video_codec;
	AVCodecContext *ctx = seg_union->input_ctx->video_codec_ctx;
	AVFormatContext *fmt_ctx = seg_union->input_ctx->video_fmt_ctx;
	while(av_read_frame(fmt_ctx ,
			&seg_union->input_ctx->pkt_video) >= 0){

		int got_picture = 0;

		avcodec_decode_video2(ctx ,seg_union->input_ctx->yuv_frame ,
				&got_picture ,&seg_union->input_ctx->pkt_video);

		if(got_picture){
			seg_union->picture_capture_no ++;
			sws_scale(seg_union->output_ctx->img_convert_ctx ,
												(const uint8_t * const *)seg_union->input_ctx->yuv_frame->data ,seg_union->input_ctx->yuv_frame->linesize ,
												0,
												ctx->height ,
												seg_union->output_ctx->encoded_yuv_pict->data ,seg_union->output_ctx->encoded_yuv_pict->linesize);


			seg_union->output_ctx->sync_ipts =
					(double)seg_union->picture_capture_no / CAPTURE_FRAME_RATE;

			encode_video_frame(seg_union->output_ctx ,seg_union->output_ctx->encoded_yuv_pict ,NULL);
		}

		av_free_packet(&seg_union->input_ctx->pkt_video);	//do not loss
	}


}

void video_encode(Segment_U *seg_union){

}
/*
 * in follow function ,run two child thread
 * 1) one thread use to read audio data from audio device,
 * 2) another one ,use to read video data from video device.
 * 3) main thread use to listening
 * */
void capture_encode(Segment_U * seg_union){

	pthread_t pid_audio_capture;
	pthread_t pid_video_capture;
	//pthread_t pid_video_encode;


	pthread_create(&pid_audio_capture ,NULL ,audio_capture ,seg_union);
	pthread_create(&pid_video_capture ,NULL ,video_capture ,seg_union);
	//pthread_create(&pid_video_encode ,NULL ,video_encode ,seg_union);

	printf("before audio capture thread ,and video capture thread over .\n");
	pthread_join(pid_audio_capture ,NULL);
	pthread_join(pid_video_capture ,NULL);
	while(1);
}


int main(int argc ,char * argv[]){

	//define a struct point variable
	Segment_U * seg_union = NULL;
	seg_union = (Segment_U *)malloc(sizeof(Segment_U));
	if(seg_union == NULL){
		printf("seg_union malloc failed .\n");
		exit(1);
	}

	parse_option_argument(seg_union ,argc ,argv );
	printf(" #chris : parse argument success ..\n");
	/*Segment union */
	init_seg_union(seg_union ,0);

	//write output encoder data
	seg_write_header(seg_union);

	/*	init the capture devices*/
	if( (seg_union->input_ctx = malloc (sizeof(Input_Context))) == NULL){

		printf("ptr_input_ctx malloc failed .\n");
		exit(MEMORY_MALLOC_FAIL);
	}
	//init audio device
	init_audio_device(seg_union->input_ctx);
	//init video device
	init_video_device(seg_union->input_ctx);
	//
	seg_union->yuv_video_buf = (yuv_video_buf_union *)malloc(sizeof(yuv_video_buf_union));
	if(seg_union->yuv_video_buf == NULL){
		printf("yuv video buf malloc failed.\n");
		exit(1);
	}

	seg_union->yuv_video_buf->yuv_data = (unsigned char *)malloc(
			seg_union->input_ctx->video_codec_ctx->width *
			seg_union->input_ctx->video_codec_ctx->height	*
			2); //?
	if(seg_union->yuv_video_buf->yuv_data == NULL){
		printf("yuv video data malloc failed .\n");
		exit(1);
	}

	seg_union->output_ctx->img_convert_ctx =
			sws_getContext(seg_union->input_ctx->video_codec_ctx->width ,
					seg_union->input_ctx->video_codec_ctx->height,	AV_PIX_FMT_YUYV422 ,
					seg_union->output_ctx->video_stream->codec->width ,
					seg_union->output_ctx->video_stream->codec->height ,AV_PIX_FMT_YUV420P ,
					SWS_BICUBIC ,NULL ,NULL ,NULL);
	pthread_mutex_init(&seg_union->yuv_video_buf->yuv_buf_mutex ,NULL);
	pthread_cond_init(&seg_union->yuv_video_buf->yuv_buf_cond ,NULL);
	seg_union->yuv_video_buf->have_data_mark = 0;
	//main work
	capture_encode(seg_union);

	/*free memory*/
	//free_seg_union(seg_union);
	return 0;
}


