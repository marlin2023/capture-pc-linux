#ifndef _INPUT_CTX_H
#define _INPUT_CTX_H

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

#define AUDIO_INPUT_DEVICE	"hw:0,0"
#define VIDEO_INPUT_DEVICE	"/dev/video0"

typedef struct {

	/*	audio alsa	*/
	AVFormatContext *audio_fmt_ctx;
	AVCodecContext *audio_codec_ctx;
	AVCodec 	*audio_codec;

	AVPacket pkt_audio;
	AVFrame 	*audio_decode_frame;
	int 		audio_size;				//audio size ,the output invoke avcodec_decode_audio4 one time

	/*	video /dev/video0	*/
	AVFormatContext *video_fmt_ctx;
	AVCodecContext *video_codec_ctx;
	AVCodec 	*video_codec;
	AVPacket pkt_video;
	AVFrame 	*yuv_frame;

	/*	decode mark	*/
	int 		mark_have_frame;
}Input_Context;



/*
 * function : init_input
 * @param:	ptr_input_ctx 	 	a structure contain the inputfile information
 * @param:	input_file			the input file name
 *
 * */
int init_input(Input_Context *ptr_input_ctx, char* input_file);

/*
 * function : free input ,in order to close codecs and input streams
 * @param:	ptr_input_ctx 	 	a structure contain the inputfile information
 *
 * */
void free_input(Input_Context *ptr_input_ctx);


/*
 * function : manual malloc some memory
 * @param:	ptr_input_ctx 	 	a structure contain the inputfile information
 *
 * */
void malloc_input_memory(Input_Context *ptr_input_ctx);


void free_input_memory(Input_Context *ptr_input_ctx);
/*
 * function : init_input
 * @param:	ptr_input_ctx 	 	a structure contain the inputfile information
 * @param:	pkt					the packet read from the AVFormatContext
 *
 * */
void decode_frame(Input_Context *ptr_input_ctx ,AVPacket *pkt);

/*
 * function : init audio device and video device
 * */
void init_input_devices(Input_Context *ptr_input_ctx);


void init_audio_device(Input_Context *ptr_input_ctx);

void init_video_device(Input_Context *ptr_input_ctx);
#endif
