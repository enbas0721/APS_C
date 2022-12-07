/*
   From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html
   Fixes rate and buffer problems
   sudo apt-get install libasound2-dev
   gcc -o record.out alsa-record-gain-control.c WavManager/audioio.c -lasound
   ./record.out hw:1
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"
#include <time.h>

#include <math.h>

#define SMPL 44100
#define BIT 16

static int set_gain_value()
{
	int err = 0;
	int keep_handle = 0;
	int argc = 3;
	char argv[2] = ["Mic", "Capture", "20%"];
	static snd_mixer_t *handle = NULL;
	snd_mixer_elem_t *elem;
	snd_mixer_selem_id_t *sid;
	snd_mixer_selem_id_alloca(&sid);
	int roflag = 0;

	if (argc < 1) {
		fprintf(stderr, "Specify a full control identifier: [[iface=<iface>,][name='name',][index=<index>,][device=<device>,][subdevice=<subdevice>]]|[numid=<numid>]\n");
		return -EINVAL;
	}
	if (parse_control_id(argv[0], id)) {
		fprintf(stderr, "Wrong control identifier: %s\n", argv[0]);
		return -EINVAL;
	}
	if (debugflag) {
		printf("VERIFY ID: ");
		show_control_id(id);
		printf("\n");
	}

	if (handle == NULL) {
		// snd_mixerのオープン
		if ((err = snd_mixer_open(&handle, 0)) < 0) {
			error("Mixer %s open error: %s\n", card, snd_strerror(err));
			return err;
		}
		// snd_mixer_attach
		if (smixer_level == 0 && (err = snd_mixer_attach(handle, card)) < 0) {
			error("Mixer attach %s error: %s", card, snd_strerror(err));
			snd_mixer_close(handle);
			handle = NULL;
			return err;
		}
		if ((err = snd_mixer_selem_register(handle, smixer_level > 0 ? &smixer_options : NULL, NULL)) < 0) {
			error("Mixer register error: %s", snd_strerror(err));
			snd_mixer_close(handle);
			handle = NULL;
			return err;
		}
		err = snd_mixer_load(handle);
		if (err < 0) {
			error("Mixer %s load error: %s", card, snd_strerror(err));
			snd_mixer_close(handle);
			handle = NULL;
			return err;
		}
	}
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		if (ignore_error)
			return 0;
		error("Unable to find simple control '%s',%i\n", snd_mixer_selem_id_get_name(sid), snd_mixer_selem_id_get_index(sid));
		snd_mixer_close(handle);
		handle = NULL;
		return -ENOENT;
	}
	if (!roflag) {
		/* enum control */
		if (snd_mixer_selem_is_enumerated(elem))
			err = sset_enum(elem, argc, argv);
		else
			err = sset_channels(elem, argc, argv);

		if (!err)
			goto done;
		if (err < 0) {
			error("Invalid command!");
			goto done;
		}
	}
	if (!quiet) {
		printf("Simple mixer control '%s',%i\n", snd_mixer_selem_id_get_name(sid), snd_mixer_selem_id_get_index(sid));
		show_selem(handle, sid, "  ", 1);
	}
done:
	if (!keep_handle) {
		snd_mixer_close(handle);
		handle = NULL;
	}
	return err < 0 ? 1 : 0;
}

int main (int argc, char *argv[])
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

	double recording_time = 10.0f;

	// Wavファイル作成用
	WAV_PRM prm;
	int16_t *record_data;
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
		fprintf (stdout, "cannot open audio device %s (%s)\n",
		         argv[1],
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

	snd_pcm_hw_params_free (hw_params);

	fprintf(stdout, "hw_params freed\n");

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fprintf (stdout, "cannot prepare audio interface for use (%s)\n",
		         snd_strerror (err));
		exit (1);
	}

	fprintf(stdout, "audio interface prepared\n");

	// record_data = calloc(prm.L, sizeof(double));
	// buffer = (double*)malloc(sizeof(double)*buffer_frames*snd_pcm_format_width(format));
	record_data = calloc(prm.L, sizeof(double));
	buffer = (int16_t*)malloc(sizeof(int16_t)*buffer_frames*snd_pcm_format_width(format));

	fprintf(stdout, "buffer allocated\n");

	// time_t start_time = time(NULL);
	// time_t elapsed_time = time(NULL) - start_time;
	int current_index = 0;
	while ((current_index + buffer_frames) < prm.L) {
		if ((err = snd_pcm_readi(capture_handle, (void*)buffer, buffer_frames)) != buffer_frames) {
			fprintf(stdout, "read from audio interface failed (%s)\n",err, snd_strerror(err));
			exit (1);
		}
		if (current_index >= (prm.L/2)) {
			set_gain_value();
		}
		fprintf(stdout, "Read buffer first 5: ");
		for(int j = 0; j < 5; j++) {
			fprintf(stdout, "%d ", buffer[j]);
		}
		printf("\n");
		for (int i = current_index; i < current_index + err; i++) {
			record_data[i] = buffer[i-current_index];
		}
		fprintf(stdout, "Record data first 5: ");
		for(int j = current_index; j < current_index + 5; j++) {
			fprintf(stdout, "%d ", record_data[j]);
		}
		printf("\n");
		current_index = current_index + err;
		// elapsed_time = time(NULL) - start_time;
	}

	fprintf(stdout, "Record data final 5: ");
	for(int j = current_index - 5; j < current_index; j++) {
		fprintf(stdout, "%d ", record_data[j]);
	}
	printf("\n");
	audio_write(record_data, &prm, filename);

	free(buffer);
	free(record_data);

	fprintf(stdout, "buffer freed\n");
	snd_pcm_close (capture_handle);
	fprintf(stdout, "audio interface closed\n");

	exit (0);
	return 0;
}