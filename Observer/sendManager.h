#ifndef SENDMANAGER_H
#define SENDMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <alsa/asoundlib.h>

/* PCMデフォルト設定 */
#define DEF_CHANNEL         1
#define DEF_FS              176200
#define DEF_BITPERSAMPLE    16
#define WAVE_FORMAT_PCM     1
#define SIGNAL_L			0.1
#define INITIAL_F			1600
#define FINAL_F				1700
#define BUF_SIZ				1024
#define INIT_WAIT_SEC       1
#define SEND_PERIOD         1
#define CHECK_CLOCK         0

typedef struct{
    int flag;
    int vol;
}send_info;

void make_chirp_wave(int16_t* data, int vol, int f0, int f1, int size);
void make_sin_wave(int16_t* data, int vol, int f, int size);
void* send_start(send_info *info);

#endif
