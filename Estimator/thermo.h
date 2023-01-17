#ifndef THERMO_H
#define THERMO_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h> 
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <stdint.h>

static uint16_t _calc_crc16(const uint8_t *buf, size_t len);
static uint16_t _combine_bytes(uint8_t msb, uint8_t lsb);
int am2321(float *out_temperature, float *out_humidity) ;
double temp_measure(double temperature);

#endif