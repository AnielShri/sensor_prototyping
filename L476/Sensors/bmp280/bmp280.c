/*
 * bmp280.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */


#include "bmp280.h"

//=============================================================================
//	external functions
//=============================================================================

bmp280_memory_operation *bmp280_read_registers;
bmp280_memory_operation *bmp280_write_registers;

//=============================================================================
//	static function declerations
//=============================================================================

static bool bmp280_read_trimming_parameters();

//=============================================================================
//	variables
//=============================================================================

// trimming parameters - temperature
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

// trimming parameters - pressure
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;


//=============================================================================
//	function definitions
//=============================================================================

bool bmp280_initialize(bmp280_memory_operation *bmp280_read, bmp280_memory_operation *bmp280_write)
{
	uint8_t pdata[4] = {0, 1, 2, 4};

	bmp280_read_registers = bmp280_read;
	bmp280_write_registers = bmp280_write;

	bmp280_read_registers(0x0, pdata, 4);
	bmp280_write_registers(0x1, pdata, 3);
	return true;
}

bool bmp280_calibrate()
{
	return true;
}

bool bmp280_set_configuration(const uint8_t standby_time, const uint8_t filter, const uint8_t spi3w_enabled)
{
	return true;
}
bool bmp280_set_measurement_control(const uint8_t temperature_oversampling, const uint8_t pressure_oversampling, const uint8_t power_mode)
{
	return true;
}

bool bmp280_get_temperature(double *temperature)
{
	return true;
}
bool bmp280_get_pressure(double *pressure)
{
	return true;
}
bool bmp280_get_temperature_and_pressure(double *temperature, double *pressure)
{
	return true;
}

bool bmp280_get_altitude_delta(double *altitude_delta)
{
	return true;
}

//=============================================================================
//	static function definitions
//=============================================================================

static bool bmp280_read_trimming_parameters()
{
	return true;
}

