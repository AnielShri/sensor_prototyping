/*
 * vl6180x_application.h
 *
 *  Created on: Nov 30, 2023
 *      Author: Aniel
 */

#ifndef VL6180X_VL6180X_APPLICATION_H_
#define VL6180X_VL6180X_APPLICATION_H_

#include "vl6180x.h"

bool vl6180x_application_initialize_device();
bool vl6180x_application_poll_measurement(uint8_t *distance_mm);

#endif /* VL6180X_VL6180X_APPLICATION_H_ */
