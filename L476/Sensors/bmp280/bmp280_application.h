/*
 * bmp280_application.h
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */

#ifndef BMP280_BMP280_APPLICATION_H_
#define BMP280_BMP280_APPLICATION_H_

#include "bmp280.h"

bool bmp280_application_initialize();
bool bmp280_application_get_altitude_delta(double *altitude_delta);

#endif /* BMP280_BMP280_APPLICATION_H_ */
