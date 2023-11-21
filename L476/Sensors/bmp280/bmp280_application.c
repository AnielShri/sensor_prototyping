/*
 * bmp280_application.c
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */


#include "main.h"
#include "bmp280_application.h"

extern UART_HandleTypeDef huart2;

bool bmp280_application_read_registers(const uint8_t memory_address, uint8_t *data_buffer, const uint16_t data_length)
{
//	printf("BMP280 - read registers: 0x%x, 0x%x, 0x%x\n", memory_address, data_buffer, data_length);

	uint8_t msg[64];
	uint16_t msg_len = sprintf(msg, "BMP280 - read registers: 0x%x, 0x%x, 0x%x\r\n", memory_address, data_buffer, data_length);

	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);
	return true;
}


bool bmp280_application_write_registers(const uint8_t memory_address, uint8_t *data_buffer, const uint16_t data_length)
{
	uint8_t msg[64];
	uint16_t msg_len = sprintf(msg, "BMP280 - write registers: 0x%x, 0x%x, 0x%x\r\n", memory_address, data_buffer, data_length);

	HAL_UART_Transmit(&huart2, msg, msg_len, HAL_MAX_DELAY);
	return true;
}


bool bmp280_application_initialize()
{
	bmp280_initialize(&bmp280_application_read_registers, &bmp280_application_write_registers);
	return false;
}
bool bmp280_application_get_altitude_delta(double *altitude_delta)
{
	return true;
}



