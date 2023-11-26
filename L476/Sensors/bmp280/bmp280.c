/*
 * bmp280.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */

#include <math.h>

#include "bmp280.h"
#include "usart.h"


//=============================================================================
//	external functions
//=============================================================================

bmp280_memory_operation *bmp280_read_registers;
bmp280_memory_operation *bmp280_write_registers;
bmp280_sleep_function   *bmp280_sleep;

//=============================================================================
//	static function declerations
//=============================================================================

static bool bmp280_read_trimming_parameters();
static bool bmp280_read_measurement_registers(uint8_t *measurement_data);

static bool bmp280_calculate_Temperature_100(uint8_t *measurement_data, int32_t *Temperature_100, int32_t *t_fine);
static bool bmp280_calculate_Pressure_256(uint8_t *measurement_data, uint32_t *Pressure_256, int32_t t_fine);

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

// reference parameters
double pressure_reference;
double temperature_reference_over_Lb;


//=============================================================================
//	function definitions
//=============================================================================


//=============================================================================
//	initialization
//=============================================================================
bool bmp280_initialize(bmp280_memory_operation *bmp280_read, bmp280_memory_operation *bmp280_write, bmp280_sleep_function *bmp280_sleep_fn)
{
	bool result = true;
	uint8_t chip_id;

	if (bmp280_read != 0x0 && bmp280_write != 0x0 && bmp280_sleep_fn != 0x0)
	{
		bmp280_read_registers = bmp280_read;
		bmp280_write_registers = bmp280_write;
		bmp280_sleep = bmp280_sleep_fn;
	}
	else
	{
		result = false;
	}

	// check CHIP ID

	if (result == true)
	{
		result = bmp280_read_registers(BMP280_ADDRESS_ID, &chip_id, 1);
	}
	if (result == true)
	{
		if (chip_id != BMP280_VALUE_ID)
		{
			result = false;
		}
	}

	// if correct CHIP => read calibration values
	if (result == true)
	{
		result = bmp280_read_trimming_parameters();
	}

	// set reference pressure/temperature at sea-level
	if (result == true)
	{
		pressure_reference = 101325; // at sea level
		temperature_reference_over_Lb = (273.15+15)/6.5e-3; // (15C => to K) / 6.5e-3
	}

	return result;
}


bool bmp280_set_configuration(bmp280_standby_time_enum standby_time, bmp280_filter_coefficient_enum filter, bmp280_spi3w_enabled_enum spi3w_enabled)
{
	bool result = true;
	uint8_t config_register = (uint8_t)standby_time | (uint8_t)filter | (uint8_t)spi3w_enabled;

	if (result == true)
	{
		result = bmp280_write_registers(BMP280_ADDRESS_CONFIG, &config_register, 1);
	}
	return result;
}

bool bmp280_set_measurement_control(bmp280_temperature_oversampling_enum temperature_oversampling, bmp280_pressure_oversampling_enum pressure_oversampling, bmp280_power_mode_enum power_mode)
{
	bool result = true;
	uint8_t measurement_ctrl_register = (uint8_t)temperature_oversampling | (uint8_t)pressure_oversampling | (uint8_t)power_mode;

	if (result == true)
	{
		result = bmp280_write_registers(BMP280_ADDRESS_MEASUREMENT_CONTROL, &measurement_ctrl_register, 1);
	}
	return result;
}

bool bmp280_get_temperature(double *temperature)
{
	bool result = true;
	uint8_t measurement_data[BMP280_LENGTH_MEASUREMENT_DATA];
	int32_t Temperature_100, t_fine;

	// read register data
	if (result == true)
	{
		result = bmp280_read_measurement_registers(measurement_data);
	}

	// calculate T_100
	if (result == true)
	{
		result = bmp280_calculate_Temperature_100(measurement_data, &Temperature_100, &t_fine);
	}

	// if all went well => convert to double
	if (result == true)
	{
		*temperature = Temperature_100/100;
	}

	return result;
}

bool bmp280_get_pressure(double *pressure)
{
	bool result = true;

	// read register data
	if (result == true)
	{
		double temperature = 0;
		result = bmp280_get_temperature_and_pressure(&temperature, pressure);
	}

	return result;
}

bool bmp280_get_temperature_and_pressure(double *temperature, double *pressure)
{
	bool result = true;
	uint8_t measurement_data[BMP280_LENGTH_MEASUREMENT_DATA];
	int32_t Temperature_100, t_fine;
	uint32_t Pressure_256;

	// read register data
	if (result == true)
	{
		result = bmp280_read_measurement_registers(measurement_data);
	}

	// calculate T_100
	if (result == true)
	{
		result = bmp280_calculate_Temperature_100(measurement_data, &Temperature_100, &t_fine);
	}

	// calculate P_256
	if (result == true)
	{
		result = bmp280_calculate_Pressure_256(measurement_data, &Pressure_256, t_fine);
	}

	// if all went well => convert to double
	if (result == true)
	{
		*temperature = Temperature_100/100;
		*pressure = Pressure_256/256;
	}

	return result;
}

//=============================================================================
//	application logic
//=============================================================================

bool bmp280_calibrate()
{
	bool result = true;
	uint8_t previous_config;
	uint8_t previous_measurement_control;

	const uint8_t number_samples = 20;
	double pressure_list[number_samples];
	double temperature_list[number_samples];

	bmp280_power_mode_enum power_mode = BMP280_POWER_MODE_NORMAL;
	bmp280_pressure_oversampling_enum pressure_oversampling = BMP280_PRESSURE_OVERSAMPLING_16X_ULTRA_HIGH_RESOLUTION;
	bmp280_temperature_oversampling_enum temperature_oversampling = BMP280_TEMPERATURE_OVERSAMPLING_2X;

	bmp280_filter_coefficient_enum filter = BMP280_FILTER_COEFFIENT_16X;
	bmp280_standby_time_enum standby_time = BMP280_STANDBY_TIME_0_5_MS;
	bmp280_spi3w_enabled_enum spi3w_enabled = BMP280_SPI3W_DISABLED;

	// retrieve current values
	if (result == true)
	{
		result = bmp280_read_registers(BMP280_ADDRESS_CONFIG, &previous_config, 1);
	}
	if (result == true)
	{
		result = bmp280_read_registers(BMP280_ADDRESS_MEASUREMENT_CONTROL, &previous_measurement_control, 1);
	}

	// set configuration for "indoor navigation"
	if (result == true)
	{
		result = bmp280_set_configuration(standby_time, filter, spi3w_enabled);
	}
	if (result == true)
	{
		result = bmp280_set_measurement_control(temperature_oversampling, pressure_oversampling, power_mode);
	}

	// throw away first set of samples to stabilize internal filter
	for (uint8_t n = 0; n < 10 && result == true; n++)
	{
		result = bmp280_get_temperature_and_pressure(&(temperature_list[0]), &(pressure_list[0]));
		bmp280_sleep(40);
	}
	// use second set of samples
	for (uint8_t n = 0; n < number_samples && result == true; n++)
	{
		result = bmp280_get_temperature_and_pressure(&(temperature_list[n]), &(pressure_list[n]));
		bmp280_sleep(40);
	}

	// average out
	if (result == true)
	{
		temperature_reference_over_Lb = 0;
		pressure_reference = 0;

		for(uint8_t n = 0; n < number_samples; n++)
		{
			temperature_reference_over_Lb += temperature_list[n] / number_samples;
			pressure_reference += pressure_list[n] / number_samples;
		}
		temperature_reference_over_Lb = (temperature_reference_over_Lb + 273.15) / 6.5e-3;
	}

	// restore previous settings
	if (result == true)
	{
		result = bmp280_write_registers(BMP280_ADDRESS_MEASUREMENT_CONTROL, &previous_measurement_control, 1);
	}
	if (result == true)
	{
		result = bmp280_write_registers(BMP280_ADDRESS_CONFIG, &previous_config, 1);
	}

	return result;
}

bool bmp280_get_altitude_delta(double *altitude_delta)
{
	// https://en.wikipedia.org/wiki/Pressure_altitude
	// https://physics.stackexchange.com/questions/333475/how-to-calculate-altitude-from-current-temperature-and-pressure
	// https://github.com/adafruit/Adafruit_BMP280_Library/blob/master/Adafruit_BMP280.cpp#L321C56-L321C63
	// https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
	// https://forum.arduino.cc/t/barometric-pressure-to-altitude/297866/5
	// https://en.wikipedia.org/wiki/Barometric_formula

	bool result = true;

	double pressure;
	const double pow_const = 0.19026643566373183;

	// get current pressure
	if (result == true)
	{
		result = bmp280_get_pressure(&pressure);
	}

	// calculate altitude delta
	if (result == true)
	{
		*altitude_delta = temperature_reference_over_Lb * (1 - pow(pressure / pressure_reference, pow_const));
	}

	return result;
}

//=============================================================================
//	static function definitions
//=============================================================================

static bool bmp280_read_trimming_parameters()
{
	bool result = true;

	uint8_t calibration_data[BMP280_LENGTH_CALIBRATION];

	if (result == true)
	{
		result = bmp280_read_registers(BMP280_ADDRESS_CALIBRATION_START, calibration_data, BMP280_LENGTH_CALIBRATION);
	}

	// bit manipulation could be a bit more clean
	if (result == true)
	{
		// temperature
		dig_T1  = (uint16_t)(calibration_data[1] << 8) | (calibration_data[0]);
		dig_T2  =  (int16_t)(calibration_data[3] << 8) | (calibration_data[2]);
		dig_T3  =  (int16_t)(calibration_data[5] << 8) | (calibration_data[4]);

		// pressure
		dig_P1  =  (uint16_t)(calibration_data[7] << 8) | (calibration_data[6]);
		dig_P2  =   (int16_t)(calibration_data[9] << 8) | (calibration_data[8]);
		dig_P3  =  (int16_t)(calibration_data[11] << 8) | (calibration_data[10]);
		dig_P4  =  (int16_t)(calibration_data[13] << 8) | (calibration_data[12]);
		dig_P5  =  (int16_t)(calibration_data[15] << 8) | (calibration_data[14]);
		dig_P6  =  (int16_t)(calibration_data[17] << 8) | (calibration_data[16]);
		dig_P7  =  (int16_t)(calibration_data[19] << 8) | (calibration_data[18]);
		dig_P8  =  (int16_t)(calibration_data[21] << 8) | (calibration_data[20]);
		dig_P9  =  (int16_t)(calibration_data[23] << 8) | (calibration_data[22]);
	}

	return result;
}

static bool bmp280_read_measurement_registers(uint8_t *measurement_data)
{
	// NOTE: since `measurement_data` is ONLY passed around in internal functions, no DATA_LENGTH checks are made
	// 		 and the assumption is that the DATA_LENGTH was defined properly in the calling function
	bool result = true;

	if (result == true)
	{
		result = bmp280_read_registers(BMP280_ADDRESS_MEASUREMENT_DATA_START, measurement_data, BMP280_LENGTH_MEASUREMENT_DATA);
	}

	return result;
}
//-----------------------------------------------------------------------------
//	proprietary code taken from datasheet
//-----------------------------------------------------------------------------
static bool bmp280_calculate_Temperature_100(uint8_t *measurement_data, int32_t *Temperature_100, int32_t *t_fine)
{
	bool result = true;

	int32_t ADC_Temperature = (int32_t) (measurement_data[3] << 12) | (measurement_data[4] << 4) | (measurement_data[5] >> 4);

	if (result == true)
	{
		int32_t t_var1, t_var2;
		t_var1 = ((((ADC_Temperature >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
		t_var2 = (((((ADC_Temperature >> 4) - ((int32_t)dig_T1)) * ((ADC_Temperature >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
		*t_fine = t_var1 + t_var2;
		*Temperature_100 = ((*t_fine) * 5 + 128) >> 8;
	}

	return result;
}

static bool bmp280_calculate_Pressure_256(uint8_t *measurement_data, uint32_t *Pressure_256, int32_t t_fine)
{
	bool result = true;
	int64_t p_var1, p_var2, p_fine;

	int32_t ADC_Pressure = (int32_t) (measurement_data[0] << 12) | (measurement_data[1] << 4) | (measurement_data[2] >> 4);

	if (result == true)
	{
		p_var1 = ((int64_t)t_fine) - 128000;
		p_var2 = p_var1 * p_var1 * (int64_t)dig_P6;
		p_var2 = p_var2 + ((p_var1 * (int64_t)dig_P5) << 17);
		p_var2 = p_var2 + (((int64_t)dig_P4) << 35);
		p_var1 = ((p_var1 * p_var1 * (int64_t)dig_P3) >> 8) + ((p_var1 * (int64_t)dig_P2) << 12);
		p_var1 = (((((int64_t)1) << 47) + p_var1)) * ((int64_t)dig_P1) >> 33;

		// check for devision by zero
		if (p_var1 == 0)
		{
			result = false;
		}
	}

	if (result == true)
	{
		p_fine = 1048576 - ADC_Pressure;
		p_fine = (((p_fine << 31) - p_var2) * 3125) / p_var1;
		p_var1 = (((int64_t)dig_P9) * (p_fine >> 13) * (p_fine >> 13)) >> 25;
		p_var2 = (((int64_t)dig_P8) * p_fine) >> 19;
		p_fine = ((p_fine + p_var1 + p_var2) >> 8) + (((int64_t)dig_P7) << 4);
		*Pressure_256 = (uint32_t)p_fine;
	}

	return result;
}


