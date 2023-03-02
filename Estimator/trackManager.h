#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <stdint.h>
#include "recordManager.h"
#include "thermo.h"

void write_result(char * filename, double * time, double * distances, int size);
void make_chirp_wave(int16_t* g);
void* track_start(record_info *info);

#endif