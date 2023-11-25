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
static bool bmp280_read_measurement_registers(uint8_t *measurement_data);

static bool bmp280_calculate_Temperature_100(uint8_t *measurement_data, int32_t *Temperature_100, int32_t *t_fine);
static bool bmp280_calculate_Pressure_256(uint8_t *measurement_data, uint32_t *Pressure_256, int32_t t_fine);

//=============================================================================
//	variables
//=============================================================================

bool is_initialized;

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


//=============================================================================
//	initialization
//=============================================================================
bool bmp280_initialize(bmp280_memory_operation *bmp280_read, bmp280_memory_operation *bmp280_write)
{
	bool result = true;
	is_initialized = false;

	// check CHIP ID
	uint8_t chip_id;
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

	// store initialized value => will be used as start check for any other function
	is_initialized = result;
	return result;
}


bool bmp280_set_configuration(bmp280_standby_time_enum standby_time, bmp280_filter_coefficient filter, bmp280_spi3w_enabled_enum spi3w_enabled)
{
	bool result = is_initialized;
	uint8_t config_register = (uint8_t)standby_time | (uint8_t)filter | (uint8_t)spi3w_enabled;

	if (result == true)
	{
		result = bmp280_write_registers(BMP280_ADDRESS_CONFIG, &config_register, 1);
	}
	return result;
}

bool bmp280_set_measurement_control(bmp280_temperature_oversampling_enum temperature_oversampling, bmp280_pressure_oversampling_enum pressure_oversampling, bmp280_power_mode_enum power_mode)
{
	bool result = is_initialized;
	uint8_t measurement_ctrl_register = (uint8_t)temperature_oversampling | (uint8_t)pressure_oversampling | (uint8_t)power_mode;

	if (result == true)
	{
		result = bmp280_write_registers(BMP280_ADDRESS_MEASUREMENT_CONTROL, &measurement_ctrl_register, 1);
	}
	return result;
}

bool bmp280_get_temperature(double *temperature)
{
	bool result = is_initialized;
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
	bool result = is_initialized;

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
	bool result = is_initialized;
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
	bool result = is_initialized;

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
	bool result = is_initialized;

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
	bool result = is_initialized;

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
	bool result = is_initialized;
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


