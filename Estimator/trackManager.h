#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <stdint.h>

typedef struct{
    int  flag;
    char filename[16];
    int last_index;
    int16_t *record_data;
}track_info;

#endif