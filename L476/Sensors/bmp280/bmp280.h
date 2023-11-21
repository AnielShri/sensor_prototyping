/*
 * bmp280.h
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */

#ifndef BMP280_BMP280_H_
#define BMP280_BMP280_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// function pointer for memory read/write
typedef bool (bmp280_memory_operation)(const uint8_t memory_address, uint8_t *data_buffer, uint16_t data_length);

bool bmp280_initialize(bmp280_memory_operation *bmp280_read, bmp280_memory_operation *bmp280_write);
bool bmp280_calibrate();

bool bmp280_set_configuration(const uint8_t standby_time, const uint8_t filter, const uint8_t spi3w_enabled);
bool bmp280_set_measurement_control(const uint8_t temperature_oversampling, const uint8_t pressure_oversampling, const uint8_t power_mode);

bool bmp280_get_temperature(double *temperature);
bool bmp280_get_pressure(double *pressure);
bool bmp280_get_temperature_and_pressure(double *temperature, double *pressure);

bool bmp280_get_altitude_delta(double *altitude_delta);

#endif /* BMP280_BMP280_H_ */
