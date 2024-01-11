/*
 * vl6180x_application.c
 *
 *  Created on: Nov 30, 2023
 *      Author: Aniel
 */

#include "i2c.h"
#include "usart.h"

#include "vl6180x_application.h"



//=============================================================================
//	static function declerations
//=============================================================================
static bool vl6180x_application_read_registers(const vl6180x_register_address_enum register_address, uint8_t *data_buffer, const uint16_t data_length);
static bool vl6180x_application_write_registers(const vl6180x_register_address_enum register_address, uint8_t *data_buffer, const uint16_t data_length);
static bool vl6180x_application_sleep(const uint32_t timeout_ms);


//=============================================================================
//	global variables
//=============================================================================
const uint8_t vl6180x_device_i2c_address = VL6180X_I2C_DEVICE_ADDRESS;

//=============================================================================
//	client functions
//=============================================================================

/******************************************************************************
 * @brief client API to initialize device
 * 
 * @param[out] true if succeeded
*/
bool vl6180x_application_initialize_device()
{
	bool result = true;
	uint8_t data;


//	vl6180x_application_read_registers(VL6180X_REGISTER_SYSTEM_FRESH_OUT_OF_RESET, &data, 1);

	result = vl6180x_initialize(&vl6180x_application_read_registers, &vl6180x_application_write_registers, &vl6180x_application_sleep);

	if (result == true)
	{
		result = vl6180x_start_continuous_measurements();
	}

	uint8_t msg[92];
	uint16_t msg_len;
	if (result == true)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - device initialization [OK]\r\n");
	}
	else
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - device initialization [FAIL]\r\n");
	}
	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);


	vl6180x_application_read_registers(VL6180X_REGISTER_IDENTIFICATION_MODEL_ID, &data, 1);
	vl6180x_application_read_registers(VL6180X_REGISTER_SYSTEM_MODE_GPIO0, &data, 1);
	vl6180x_application_read_registers(VL6180X_REGISTER_SYSTEM_MODE_GPIO1, &data, 1);
	vl6180x_application_read_registers(VL6180X_REGISTER_SYSTEM_FRESH_OUT_OF_RESET, &data, 1);
	vl6180x_application_read_registers(VL6180X_REGISTER_SYSRANGE_START, &data, 1);


	return result;
}


/******************************************************************************
 * @brief client API to retrieve measurement
*/
bool vl6180x_application_poll_measurement(uint8_t *distance_mm)
{
	bool result = true;

	result = vl6180x_wait_for_new_measurement(1000);

	if (result == true)
	{
		uint8_t error_flag;
		result = vl6180x_get_measurement_result(distance_mm, &error_flag);
	}

	return result;
}


//=============================================================================
//	callback functions
//=============================================================================

/******************************************************************************
 * @brief read I2C register. Uses underlying HAL 
 * 
 * @param[in] register_address
 * @param[in] data_buffer
 * @param[in] data_length
 * 
 * @param[out] true if succeeds
*/
static bool vl6180x_application_read_registers(const vl6180x_register_address_enum register_address, uint8_t *data_buffer, const uint16_t data_length)
{
	bool result = true;
	uint8_t msg[92];
	uint16_t msg_len;

	HAL_StatusTypeDef retval = HAL_I2C_Mem_Read(&hi2c3, vl6180x_device_i2c_address, (uint8_t)register_address, I2C_MEMADD_SIZE_16BIT, data_buffer, data_length, HAL_MAX_DELAY);

	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - read registers: addr: 0x%x | len: %2d | data[0]: %d [OK]\r\n", (uint8_t)register_address, data_length,  *data_buffer);
	}
	else
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - read registers: addr: 0x%x | len: %2d | result: %d [FAILED]\r\n", (uint8_t)register_address, data_length, retval);
		result = false;
	}

	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);

	return result;
}


/******************************************************************************
 * @brief write I2C register. Uses underlying HAL 
 * 
 * @param[in] register_address
 * @param[in] data_buffer
 * @param[in] data_length
 * 
 * @param[out] true if succeeds
*/
static bool vl6180x_application_write_registers(const vl6180x_register_address_enum register_address, uint8_t *data_buffer, const uint16_t data_length)
{
	bool result = true;
	uint8_t msg[92];
	uint16_t msg_len;

	HAL_StatusTypeDef retval = HAL_I2C_Mem_Write(&hi2c3, vl6180x_device_i2c_address, (uint8_t)register_address, I2C_MEMADD_SIZE_16BIT, data_buffer, data_length, HAL_MAX_DELAY);

	if (retval == HAL_OK)
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - write registers: addr: 0x%x | len: %2d | result: %d [OK]\r\n", (uint8_t)register_address, data_length,  retval);
	}
	else
	{
		msg_len = (uint16_t)sprintf((char*)msg, "VL6180X - write registers: addr: 0x%x | len: %2d | result: %d [FAILED]\r\n", (uint8_t)register_address, data_length, retval);
		result = false;
	}

	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);
	return result;
}


/******************************************************************************
 * @brief sleep function
 * 
 * @param[in] delay_ms
*/
static bool vl6180x_application_sleep(const uint32_t timeout_ms)
{
	HAL_Delay(timeout_ms);
	return true;
}
