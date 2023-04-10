#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <stdint.h>
#include "recordManager.h"
#include "thermo.h"

void write_result(char * filename, double * time, double * distances, double * ideal, double * c_time, int size);
void make_chirp_wave(int th, int16_t* g);
void cross_correlation(double* fai, int16_t* data, int16_t* ideal_sig, int checking_index);
int get_max_index(double* S, size_t size);
void* track_start(record_info *info);

#endif