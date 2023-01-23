#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"

#define SMPL 22000
#define BIT 16
#define EPS 0.9
#define TAU 1

typedef struct{
    int  flag;
    char card[16];
    char filename[16];
    int last_index;
    int16_t *record_data;
}record_info;

int write_record_data(int16_t *record_data, int size, char * filename);
void* record_start(record_info *info);

#endif
