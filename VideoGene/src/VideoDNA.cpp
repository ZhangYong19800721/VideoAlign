/*
 * VideoDNA.cpp
 *
 *  Extract the video gene from a video file and write the gene to a *.gene file
 */

#include <stdio.h>
#include <math.h>
#include <fstream>
#include <bitset>
#include <iomanip>
#include <iostream>
#include "Extractor.h"

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#ifdef __cplusplus
}
;
#endif
#endif

int VideoDNA(char * input_filename, char * output_filename, double dDNAPS) {
	AVFormatContext *pFormatCtx;
	unsigned int i, fps;
	int videoindex;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	AVFrame *pFrame, *pFrameYUV;
	unsigned char *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	static int scale_down_rows = 32;
	static int scale_down_cols = 32;
	static Extractor extractor = Extractor("weight.mat");

	printf("input video file %s.\n", input_filename);
	char * filepath = input_filename;
	char * output_file_name = output_filename;

	// open a binary file for output
	std::ofstream output_file(output_file_name, std::ios::binary);
	if (!output_file.is_open()) {
		std::cout << "Can not open the output file!" << std::endl;
		return -1;
	}

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	// open the file
	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}

	// find the video steam information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("Couldn't find stream information.\n");
		return -1;
	}

	// find the video stream
	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	}

	if (videoindex == -1) { // the case if not any video stream can be found
		printf("Didn't find a video stream.\n");
		return -1;
	}

	// get the frame rate
	AVRational r_frame_rate = pFormatCtx->streams[videoindex]->r_frame_rate;
	fps = round(double(r_frame_rate.num) / r_frame_rate.den);
	fps = round(dDNAPS * fps);

	pCodecCtx = pFormatCtx->streams[videoindex]->codec; // get the codec context
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id); // get the codec
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	out_buffer = (unsigned char *) av_malloc(
			av_image_get_buffer_size(AV_PIX_FMT_YUV444P, scale_down_cols,
					scale_down_rows, 1));
	av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
			AV_PIX_FMT_YUV444P, scale_down_cols, scale_down_rows, 1);

	packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	//Output Info-----------------------------
	printf("--------------- File Information ----------------\n");
	av_dump_format(pFormatCtx, 0, filepath, 0);
	printf("-------------------------------------------------\n");

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
			pCodecCtx->pix_fmt, scale_down_cols, scale_down_rows,
			AV_PIX_FMT_YUV444P, SWS_BICUBIC, NULL, NULL, NULL);

	long frame_idx = 1;
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoindex) {
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,
					packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture) {
				printf("Succeed to decode %ld frame!", frame_idx);
				if (frame_idx % fps == 0) {
					sws_scale(img_convert_ctx,
							(const unsigned char* const *) pFrame->data,
							pFrame->linesize, 0, pCodecCtx->height,
							pFrameYUV->data, pFrameYUV->linesize);

					y_size = scale_down_cols * scale_down_rows;
					uint8_t * Y = pFrameYUV->data[0];
					uint8_t * U = pFrameYUV->data[1];
					uint8_t * V = pFrameYUV->data[2];

					itpp::mat mY = itpp::mat(scale_down_rows, scale_down_cols);
					itpp::mat mU = itpp::mat(scale_down_rows, scale_down_cols);
					itpp::mat mV = itpp::mat(scale_down_rows, scale_down_cols);

					// put data in row sequence
					for (int i = 0; i < scale_down_rows; ++i) {
						for (int j = 0; j < scale_down_cols; ++j) {
							mY(i, j) = Y[i * scale_down_cols + j];
							mU(i, j) = U[i * scale_down_cols + j];
							mV(i, j) = V[i * scale_down_cols + j];
						}
					}

					mY = itpp::reshape(mY, y_size, 1);
					mU = itpp::reshape(mU, y_size, 1);
					mV = itpp::reshape(mV, y_size, 1);
					itpp::mat mI = itpp::concat_vertical(mY,
							itpp::concat_vertical(mU, mV));
					itpp::mat DI = itpp::mat(mI.size(), 1);
					DI.ones();
					DI *= 255.0;
					mI = itpp::elem_div(mI, DI);

					itpp::mat mC = extractor.encode(mI);

					uint8_t gene[8];
					std::bitset < 8 > bs;
					for (int m = 0; m < 8; ++m) {
						for (int n = 0; n < 8; ++n) {
							bs[7 - n] = mC(m * 8 + n) > 0;
						}
						gene[i] = uint8_t(bs.to_ulong());
						output_file.put(gene[i]);
					}
					printf(" Extract Gene!");
				}
				++frame_idx;
				printf("\n");
			}
		}
		av_free_packet(packet);
	}
	//flush decoder
	//FIX: Flush Frames remained in Codec
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;

		printf("Succeed to decode %ld frame!", frame_idx);
		if (frame_idx % fps == 0) {
			sws_scale(img_convert_ctx,
					(const unsigned char* const *) pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
					pFrameYUV->linesize);

			y_size = scale_down_cols * scale_down_rows;
			uint8_t * Y = pFrameYUV->data[0];
			uint8_t * U = pFrameYUV->data[1];
			uint8_t * V = pFrameYUV->data[2];

			itpp::mat mY = itpp::mat(scale_down_rows, scale_down_cols);
			itpp::mat mU = itpp::mat(scale_down_rows, scale_down_cols);
			itpp::mat mV = itpp::mat(scale_down_rows, scale_down_cols);

			// put data in row sequence
			for (int i = 0; i < scale_down_rows; ++i) {
				for (int j = 0; j < scale_down_cols; ++j) {
					mY(i, j) = Y[i * scale_down_cols + j];
					mU(i, j) = U[i * scale_down_cols + j];
					mV(i, j) = V[i * scale_down_cols + j];
				}
			}

			mY = itpp::reshape(mY, y_size, 1);
			mU = itpp::reshape(mU, y_size, 1);
			mV = itpp::reshape(mV, y_size, 1);
			itpp::mat mI = itpp::concat_vertical(mY,
					itpp::concat_vertical(mU, mV));
			itpp::mat DI = itpp::mat(mI.size(), 1);
			DI.ones();
			DI *= 255.0;
			mI = itpp::elem_div(mI, DI);

			itpp::mat mC = extractor.encode(mI);

			uint8_t gene[8];
			std::bitset < 8 > bs;
			for (int m = 0; m < 8; ++m) {
				for (int n = 0; n < 8; ++n) {
					bs[7 - n] = mC(m * 8 + n) > 0;
				}
				gene[i] = uint8_t(bs.to_ulong());
				output_file.put(gene[i]);
			}
			printf(" Extract Gene!");
		}
		++frame_idx;
		printf("\n");
	}

	sws_freeContext(img_convert_ctx);

	output_file.close();

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

