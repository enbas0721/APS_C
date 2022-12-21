#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"

typedef struct{
    char card[16];
    char filename[16];
}record_info;

int write_record_data(int16_t *record_data, int size, char * filename);
void* record_start(record_info (void*)info);

#endif
