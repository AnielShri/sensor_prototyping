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


#include "bmp280_definitions.h"

// function pointer for memory read/write
typedef bool (bmp280_memory_operation)(const uint8_t memory_address, uint8_t *data_buffer, uint16_t data_length);

bool bmp280_initialize(bmp280_memory_operation *bmp280_read, bmp280_memory_operation *bmp280_write);
bool bmp280_calibrate();

bool bmp280_set_configuration(bmp280_standby_time_enum standby_time, bmp280_filter_coefficient filter, bmp280_spi3w_enabled_enum spi3w_enabled);
bool bmp280_set_measurement_control(bmp280_temperature_oversampling_enum temperature_oversampling, bmp280_pressure_oversampling_enum pressure_oversampling, bmp280_power_mode_enum power_mode);

bool bmp280_get_temperature(double *temperature);
bool bmp280_get_pressure(double *pressure);
bool bmp280_get_temperature_and_pressure(double *temperature, double *pressure);

bool bmp280_get_altitude_delta(double *altitude_delta);

#endif /* BMP280_BMP280_H_ */
