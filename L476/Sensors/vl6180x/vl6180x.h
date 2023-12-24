/*
 * vl6180x.h
 *
 *  Created on: Nov 30, 2023
 *      Author: Aniel
 */

#ifndef VL6180X_VL6180X_H_
#define VL6180X_VL6180X_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "vl6180x_definitions.h"

// function pointer for memory read/write
typedef bool (vl6180x_register_operation)(const vl6180x_register_address_enum register_address, uint8_t *data_buffer, uint16_t data_length);
typedef bool (vl6180x_sleep_function)(const uint32_t sleep_ms);

// low-level sensor interface
bool vl6180x_initialize(vl6180x_register_operation *vl6180x_read, vl6180x_register_operation *vl6180x_write, vl6180x_sleep_function *vl6180x_sleep_fn);

bool vl6180x_request_single_measurement();
bool vl6180x_start_continuous_measurements();
bool vl6180x_stop_continous_measurements();

bool vl6180x_is_measurement_ready(uint8_t *error_flag);
bool vl6180x_wait_for_new_measurement();
bool vl6180x_get_measurement_result(uint8_t *distance_mm, uint8_t *error_flag);

#endif /* VL6180X_VL6180X_H_ */
