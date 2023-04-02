#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <alsa/asoundlib.h>
#include "WavManager/audioio.h"

#define SMPL            48000
#define BUF_SIZ         2048
#define BIT             16
#define EPS             100
#define TAU             1
#define INIT_POS        0.5
#define CRSS_WNDW_SIZ   4800
#define INIT_FREQ       1600
#define FINAL_FREQ      1700
#define SIGNAL_L        0.1

typedef struct{
    int  flag;
    char filename[16];
    int last_index;
    int16_t *record_data;
}record_info;

int write_record_data(int16_t *record_data, unsigned int rate, int size, char * filename);
void* record_start(record_info *info);

#endif
