#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"

struct record_info{
    const char card[16];
    const char filename[16];
}

int write_record_data(int16_t *record_data, int size, const char * filename);
int record_start(const char *card, const char *filename);

#endif
