/*
   From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html
   Fixes rate and buffer problems
   sudo apt-get install libasound2-dev
   gcc -o record.out alsa-record-example.c WavManager/audioio.c -lasound
   ./record.out hw:1
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"
#include "recordManager.h"

#define SMPL 44100
#define BIT 16

int write_record_data(int16_t * record_data, int size, char * filename){
	// Wavファイル作成
	WAV_PRM prm;
	// Wavファイル用パラメータコピー
	prm.fs = SMPL;
	prm.bits = BIT;
	prm.L = size;
	
	audio_write(record_data, &prm, filename);
}

void* record_start(record_info *info)
{
	// バッファ系の変数
	int i;
	int err;
	int16_t *buffer;
	int buffer_frames = 1024;
	unsigned int rate = SMPL;
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	if ((err = snd_pcm_open (&capture_handle, info->card, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stdout, "cannot open audio device %s (%s)\n",
		         info->card,
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "audio interface opened\n");

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stdout, "cannot allocate hardware parameter structure (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params allocated\n");

	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
		fprintf (stdout, "cannot initialize hardware parameter structure (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params initialized\n");

	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stdout, "cannot set access type (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params access setted\n");

	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
		fprintf (stdout, "cannot set sample format (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params format setted\n");

	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stdout, "cannot set sample rate (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params rate setted\n");

	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
		fprintf (stdout, "cannot set channel count (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params channels setted\n");

	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
		fprintf (stdout, "cannot set parameters (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params setted\n");

	snd_pcm_hw_params_free(hw_params);

	fprintf(stdout, "hw_params freed\n");

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stdout, "cannot prepare audio interface for use (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "audio interface prepared\n");

	int data_size = SMPL*240;

	buffer = (int16_t*)malloc(sizeof(int16_t)*buffer_frames*snd_pcm_format_width(format));
	info->record_data = calloc(data_size, sizeof(int16_t));

	fprintf(stdout, "buffer allocated\n");

	int current_index = 0;

	while (info->flag) {
		if ((err = snd_pcm_readi(capture_handle, (void*)buffer, buffer_frames)) != buffer_frames) {
			fprintf(stdout, "read from audio interface failed (%s)\n",err, snd_strerror(err));
			exit (1);
		}
		for (int i = current_index; i < current_index + err; i++) {
			info->record_data[i] = buffer[i-current_index];
		}
		current_index = current_index + err;
		info->last_index = current_index - 1;
		if (current_index + buffer_frames > data_size){
			data_size = data_size + SMPL * 30;
			info->record_data = realloc(info->record_data, data_size*sizeof(int16_t));
		}
	}
	
	char filename[64];
    strcpy(filename,info->filename);
    strcat(filename, ".wav")
	write_record_data(info->record_data, current_index, filename);

	free(buffer);
	free(info->record_data);

	fprintf(stdout, "buffer freed\n");
	snd_pcm_close(capture_handle);
	fprintf(stdout, "audio interface closed\n");

	return 0;
}

