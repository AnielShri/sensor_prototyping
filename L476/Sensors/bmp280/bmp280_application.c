/*
 * bmp280_application.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */


#include "main.h"
#include "i2c.h"
#include "usart.h"


#include "bmp280_application.h"


const uint8_t bm280_device_i2c_address = BMP280_I2C_DEVICE_ADDRESS;

bool bmp280_application_read_registers(const uint8_t memory_address, uint8_t *data_buffer, const uint16_t data_length)
{
	bool result = true;
	uint8_t msg[92];
	uint16_t msg_len;

	HAL_StatusTypeDef retval = HAL_I2C_Mem_Read(&hi2c3, bm280_device_i2c_address, memory_address, I2C_MEMADD_SIZE_8BIT, data_buffer, data_length, HAL_MAX_DELAY);

	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "BMP280 - read registers: addr: 0x%x | len: %2d | result: %d [OK]\r\n", memory_address, data_length,  retval);
	}
	else
	{
		msg_len = (uint16_t)sprintf((char*)msg, "BMP280 - read registers: addr: 0x%x | len: %2d | result: %d [FAILED]\r\n", memory_address, data_length, retval);
		result = false;
	}

	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);

	return result;
}


bool bmp280_application_write_registers(const uint8_t memory_address, uint8_t *data_buffer, const uint16_t data_length)
{
	bool result = true;
	uint8_t msg[92];
	uint16_t msg_len;

	HAL_StatusTypeDef retval = HAL_I2C_Mem_Write(&hi2c3, bm280_device_i2c_address, memory_address, I2C_MEMADD_SIZE_8BIT, data_buffer, data_length, HAL_MAX_DELAY);

	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "BMP280 - write registers: addr: 0x%x | len: %2d | result: %d [OK]\r\n", memory_address, data_length,  retval);
	}
	else
	{
		msg_len = (uint16_t)sprintf((char*)msg, "BMP280 - write registers: addr: 0x%x | len: %2d | result: %d [FAILED]\r\n", memory_address, data_length, retval);
		result = false;
	}

	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);
	return result;
}

bool bmp280_application_sleep(const uint32_t timeout_ms)
{
	HAL_Delay(timeout_ms);
	return true;
}


bool bmp280_application_initialize()
{
	bool result = true;

	uint8_t msg[64];
	uint16_t msg_len;

	// initialize
	if (result == true)
	{
		result = bmp280_initialize(&bmp280_application_read_registers, &bmp280_application_write_registers, &bmp280_application_sleep);
	}
	if (result == true)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "BMP280 - bmp280_initialize: OK\r\n");
		HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);
	}

	// calibrate
	if (result == true)
	{
		bmp280_calibrate();
	}

	// sensor configuration
	bmp280_power_mode_enum power_mode = BMP280_POWER_MODE_NORMAL;
	bmp280_pressure_oversampling_enum pressure_oversampling = BMP280_PRESSURE_OVERSAMPLING_16X_ULTRA_HIGH_RESOLUTION;
	bmp280_temperature_oversampling_enum temperature_oversampling = BMP280_TEMPERATURE_OVERSAMPLING_2X;

	bmp280_filter_coefficient_enum filter = BMP280_FILTER_OFF;
	bmp280_standby_time_enum standby_time = BMP280_STANDBY_TIME_0_5_MS;
	bmp280_spi3w_enabled_enum spi3w_enabled = BMP280_SPI3W_DISABLED;

	// store configuration values on sensor
	if (result == true)
	{
		result = bmp280_set_configuration(standby_time, filter, spi3w_enabled);
	}
	if (result == true)
	{
		result = bmp280_set_measurement_control(temperature_oversampling, pressure_oversampling, power_mode);
	}

	return result;
}

bool bmp280_application_get_altitude_delta(double *altitude_delta)
{
	bool result = true;

	if (result == true)
	{
		result = bmp280_get_altitude_delta(altitude_delta);
	}
	return result;
}



