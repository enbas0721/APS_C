/*
   From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html
   Fixes rate and buffer problems
   sudo apt-get install libasound2-dev
   gcc -o record.out alsa-record-example.c WavManager/audioio.c -lasound
   ./record.out hw:1
 */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"
#include <time.h>

#include <math.h>

#define SMPL 44100
#define BIT 16

int main (int argc, char *argv[])
{
	// バッファ系の変数
	int i;
	int err;
	double *buffer;
	int buffer_frames = 1024;
	unsigned int rate = SMPL;
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	double recording_time = 4.0f;

	// Wavファイル作成用
	WAV_PRM prm;
	double *record_data;
	char filename[64] = "output.wav";

	// if (argc != 2) {
	// 	printf("Not enough argument(s). This needs 1 arguments.\n");
	// 	exit(1);
	// }

	// for (i = 0; n < sizeof(argv[1]); n++) {
	// 	filename[n] = argv[1][n];
	// }

	// パラメータコピー
	prm.fs = SMPL;
	prm.bits = BIT;
	prm.L =  prm.fs * recording_time;

	if ((err = snd_pcm_open (&capture_handle, argv[1], SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stderr, "cannot open audio device %s (%s)\n",
		         argv[1],
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "audio interface opened\n");

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params allocated\n");

	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params initialized\n");

	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params access setted\n");

	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params format setted\n");

	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
		fprintf (stderr, "cannot set sample rate (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params rate setted\n");

	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params channels setted\n");

	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "hw_params setted\n");

	snd_pcm_hw_params_free (hw_params);

	fprintf(stdout, "hw_params freed\n");

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "audio interface prepared\n");

	// record_data = calloc(prm.L, sizeof(double));
	// buffer = (double*)malloc(sizeof(double)*buffer_frames*snd_pcm_format_width(format));
	record_data = calloc(prm.L, sizeof(double));
	buffer = (double*)malloc(sizeof(double)*buffer_frames*snd_pcm_format_width(format));

	fprintf(stdout, "buffer allocated\n");

	time_t start_time = time(NULL);
	time_t elapsed_time = time(NULL) - start_time;
	int current_index = 0;
	int j = 0;
	while ((current_index + 1024) < prm.L) {
		fprintf(stdout, "%d\n", j);
		j++;
		if ((err = snd_pcm_readn(capture_handle, (void**)buffer, buffer_frames)) != buffer_frames) {
			for (int i = 0; i < 5; i++) {
				fprintf(stdout, "%f ", buffer[i]);
			}
			fprintf(stdout, "\n");
			fprintf (stderr, "read from audio interface failed (%s)\n",
			         err, snd_strerror (err));
			exit (1);
		} else {
			for (int i = current_index; i < current_index + err; i++) {
				record_data[i] = buffer[i-current_index];
			}
			current_index = current_index + err;
		}
		elapsed_time = time(NULL) - start_time;
	}

	audio_write(record_data, &prm, filename);

	for (int i = 0; i < 5; i++) {
		fprintf(stdout, "%f ", record_data[i]);
	}
	fprintf(stdout, "\n");

	free(buffer);
	free(record_data);

	fprintf(stdout, "buffer freed\n");
	snd_pcm_close (capture_handle);
	fprintf(stdout, "audio interface closed\n");

	exit (0);
	return 0;
}
