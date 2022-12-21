#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"

int write_record_data(int16_t *record_data, int size, char * filename);
int record_start(char *card, char *filename);

#endif
