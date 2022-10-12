/*
   A Minimal Capture Program
   This program opens an audio interface for capture, configures it for
   stereo, 16 bit, 44.1kHz, interleaved conventional read/write
   access. Then its reads a chunk of random data from it, and exits. It
   isn't meant to be a real program.
   From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html
   Fixes rate and buffer problems
   sudo apt-get install libasound2-dev
   gcc -o alsa-record-example -lasound alsa-record-example.c && ./alsa-record-example hw:0
 */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <time.h>

#define SMPL 44100
#define BIT 16

void make_wave_header(FILE *fp, size_t size){
	size_t filesize;
	unsigned char head[44];

	/* RIFFヘッダ (8バイト) */
	memcpy(head, "RIFF", 4);
	filesize = sizeof(head) + size;
	head[4] = (filesize - 8) >> 0  & 0xff;
	head[5] = (filesize - 8) >> 8  & 0xff;
	head[6] = (filesize - 8) >> 16 & 0xff;
	head[7] = (filesize - 8) >> 24 & 0xff;

	/* WAVEヘッダ (4バイト) */
	memcpy(head + 8, "WAVE", 4);

	/* fmtチャンク (24バイト) */
	memcpy(head + 12, "fmt ", 4);
	head[16] = 16;
	head[17] = 0;
	head[18] = 0;
	head[19] = 0;
	head[20] = 1;
	head[21] = 0;
	head[22] = 1;
	head[23] = 0;
	head[24] = SMPL >> 0  & 0xff;
	head[25] = SMPL >> 8  & 0xff;
	head[26] = SMPL >> 16 & 0xff;
	head[27] = SMPL >> 24 & 0xff;
	head[28] = (SMPL * (BIT / 8)) >> 0  & 0xff;
	head[29] = (SMPL * (BIT / 8)) >> 8  & 0xff;
	head[30] = (SMPL * (BIT / 8)) >> 16 & 0xff;
	head[31] = (SMPL * (BIT / 8)) >> 24 & 0xff;
	head[32] = (BIT / 8) >> 0 & 0xff;
	head[33] = (BIT / 8) >> 8 & 0xff;
	head[34] = BIT >> 0 & 0xff;
	head[35] = BIT >> 8 & 0xff;

	/* dataチャンク (8 + size バイト) */
	memcpy(head + 36, "data", 4);
	head[40] = size >> 0  & 0xff;
	head[41] = size >> 8  & 0xff;
	head[42] = size >> 16 & 0xff;
	head[43] = size >> 24 & 0xff;

	fwrite(head, sizeof(head), 1, fp);
}

int main (int argc, char *argv[])
{
	int i;
	int err;
	char *buffer;
	int buffer_frames = 128;
	unsigned int rate = SMPL;
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	FILE *outputfile;
	unsigned char header[44];
	//10秒間分のファイルサイズを確保
	size_t size = 10 * (SMPL * (BIT/8));
	outputfile = fopen(argv[2], "w");
	//wavファイル用のheaderを書き出す
	make_wave_header(outputfile, size);

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

	if (outputfile == NULL)
	{
		fprintf(stdout, "Couldn't open file\n");
		exit(1);
	}

	// channel数が1なので、8*1
	buffer = (char*)malloc(1024 * snd_pcm_format_width(format) / 8 * 1);
	fprintf(stdout, "snd_pcm_format_width(format): %d",snd_pcm_format_width(format));

	fprintf(stdout, "buffer allocated\n");

	time_t start_time = time(NULL);
	time_t elapsed_time = time(NULL) - start_time;
	while (elapsed_time < 5.0f) {
		if ((err = snd_pcm_readi (capture_handle, buffer, buffer_frames)) != buffer_frames) {
			fprintf (stderr, "read from audio interface failed (%s)\n",
			         err, snd_strerror (err));
			exit (1);
		}
		// fprintf(stdout, "size of buffer  %d\n", sizeof(buffer));
		// for (int i = 0; i < sizeof(buffer); i++) {
			// fprintf(outputfile, "%s", buffer[i]);
			// fwrite(buffer, sizeof(buffer), 1, outputfile);
		// }
		fputs(buffer, outputfile);
		elapsed_time = time(NULL) - start_time;
	}

	free(buffer);

	fprintf(stdout, "buffer freed\n");

	fclose(outputfile);
	snd_pcm_close (capture_handle);
	fprintf(stdout, "audio interface closed\n");

	exit (0);
	return 0;
}
