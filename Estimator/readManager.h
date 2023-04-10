#ifndef READMANAGER_H
#define REARDMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "recordManager.h"
#include "WavManager/audioio.h"

int write_record_data(int16_t *record_data, unsigned int rate, int size, char * filename);
void* read_start(record_info *info);

#endif