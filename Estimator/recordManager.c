/*
   sudo apt-get install libasound2-dev
   gcc -o record.out alsa-record-example.c WavManager/audioio.c -lasound
   ./record.out hw:1
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <math.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"
#include "recordManager.h"
#include "fir_filter.h"

int write_record_data(int16_t * record_data, unsigned int rate, int size, char * filename){
	// Wavファイル作成
	WAV_PRM prm;
	// Wavファイル用パラメータコピー
	prm.fs = rate;
	prm.bits = BIT;
	prm.L = size;
	
	audio_write(record_data, &prm, filename);
}

void* record_start(record_info *info)
{
	int i,n,m;
	// バッファ用の変数
	int err;
	int16_t *buffer;
	unsigned int rate = SMPL;

	int gain_value = 12;

	// フィルタ用変数
	double fe, fe1, fe2, delta, *b, *w;
	int delayer_num;

	// For sound pcm setting
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	char card[16];
	strcpy(card, "hw:1");

	// For sound mixer setting
	static int smixer_level = 0;
	static struct snd_mixer_selem_regopt smixer_options;
	static snd_mixer_t *mixer_handle = NULL;
	snd_mixer_elem_t *elem;
	snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_channel_id_t chn = SND_MIXER_SCHN_FRONT_LEFT;
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, "Mic");

	if ((err = snd_pcm_open (&capture_handle, card, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf (stdout, "cannot open audio device %s (%s)\n",
		         card,
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

	// Setting sound mixer
	if (mixer_handle == NULL) {
		if ((err = snd_mixer_open(&mixer_handle, 0)) < 0) {
			fprintf(stderr, "Mixer %s open error\n", card);
			exit (1);
		}
		if (smixer_level == 0 && (snd_mixer_attach(mixer_handle, card)) < 0) {
			fprintf(stderr, "Mixer attach %s error\n", card);
			snd_mixer_close(mixer_handle);
			mixer_handle = NULL;
			exit (1);
		}
		if ((snd_mixer_selem_register(mixer_handle, smixer_level > 0 ? &smixer_options : NULL, NULL)) < 0) {
			fprintf(stderr, "Mixer register error\n");
			snd_mixer_close(mixer_handle);
			mixer_handle = NULL;
			exit (1);
		}
		err = snd_mixer_load(mixer_handle);
		if (err < 0) {
			fprintf(stderr, "Mixer %s load error", card);
			snd_mixer_close(mixer_handle);
			mixer_handle = NULL;
			exit (1);
		}
	}
	elem = snd_mixer_find_selem(mixer_handle, sid);
	if (!elem) {
		fprintf(stderr, "Unable to find simple control '%s',%i\n", snd_mixer_selem_id_get_name(sid), snd_mixer_selem_id_get_index(sid));
		snd_mixer_close(mixer_handle);
		mixer_handle = NULL;
	}

	err = snd_mixer_selem_set_capture_volume(elem, chn, gain_value);
	if (err < 0) {
		fprintf(stderr, "Setting %s capture volume error", card);
		snd_mixer_close(mixer_handle);
		mixer_handle = NULL;
		exit (1);
	}

	int data_size = rate*240;

	buffer = (int16_t*)malloc(BUF_SIZ*snd_pcm_format_width(format));
	info->record_data = calloc(data_size, sizeof(int16_t));

	fprintf(stdout, "buffer allocated\n");

	// fe = 3500.0 / rate;
	fe1 = 500 / (double)rate;
	fe2 = 3000 / (double)rate;
	delta = 500.0 / (double)rate;
	
	delayer_num = (int)(3.1 / delta + 0.5) - 1; /*遅延器の数*/
	if (delayer_num % 2 == 1){
		delayer_num++;
	}
	b = calloc((delayer_num + 1), sizeof(double));
	w = calloc((delayer_num + 1), sizeof(double));

	Hanning_window(w, (delayer_num + 1));
	
	// FIR_LPF(fe, delayer_num, b, w);
	FIR_BPF(fe1, fe2, delayer_num, b, w);

	int current_index = 0;
	int16_t *x;
	x = calloc((BUF_SIZ + delayer_num), sizeof(int16_t));

	while (info->flag) {
		if ((err = snd_pcm_readi(capture_handle, (void*)buffer, BUF_SIZ)) != BUF_SIZ) {
			fprintf(stdout, "read from audio interface failed (%s)\n",err, snd_strerror(err));
			exit (1);
		}
		filtering(info->record_data, buffer, b, x, current_index, BUF_SIZ, delayer_num);
		for (i = current_index; i < current_index + err; i++) {
			info->record_data[i] = buffer[i-current_index];
		}
		current_index = current_index + err;
		info->last_index = current_index - 1;
		if (current_index + BUF_SIZ > data_size){
			data_size = data_size + rate * 30;
			info->record_data = realloc(info->record_data, data_size*sizeof(int16_t));
		}
		// 以下でゲイン調整可能
		// snd_mixer_selem_set_capture_volume(elem, chn, gain_value);
	}
	
	snd_pcm_close(capture_handle);
	fprintf(stdout, "audio interface closed\n");

	char filename[64];
    strcpy(filename,info->filename);
    strcat(filename, ".wav");
	write_record_data(info->record_data, rate, current_index, filename);

	free(buffer);
	free(info->record_data);
	free(b); /* メモリの解放 */
  	free(w); /* メモリの解放 */
	free(x); /* メモリの解放 */
	fprintf(stdout, "buffer freed\n");

	snd_mixer_close(mixer_handle);
	mixer_handle = NULL;
	fprintf(stdout, "mixer_handle freed");

	return 0;
}

