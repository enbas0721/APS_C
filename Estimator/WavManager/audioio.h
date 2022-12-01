#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <stdio.h>
#include <stdint.h>

typedef struct
{
	int fs;
	int bits;
	int L;
}WAV_PRM;

double *audio_read(WAV_PRM *prm, char *filename);
void audio_write(int16_t *data, WAV_PRM *prm, char *filename);

#endif
