#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <stdio.h>

typedef struct
{
	int fs;
	int bits;
	int L;
}WAV_PRM;

double *audio_read(WAV_PRM *prm, char *filename);
void audio_write(char *data, WAV_PRM *prm, char *filename);

#endif
