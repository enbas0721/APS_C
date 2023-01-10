#ifndef TRACKMANAGER_H
#define TRACKMANAGER_H

#include <stdint.h>
#include "recordManager.h"

int write_result(char * filename);
void* track_start(record_info *info);

#endif