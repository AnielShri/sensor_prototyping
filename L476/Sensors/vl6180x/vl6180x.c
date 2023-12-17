/*
 * vl6180x.c
 *
 *  Created on: Dec 17, 2023
 *      Author: Aniel
 */

#include "vl6180x.h"


//=============================================================================
//	external functions
//=============================================================================

vl6180x_register_operation *vl6180x_read_registers;
vl6180x_register_operation *vl6180x_write_registers;
vl6180x_sleep_function *vl6180x_sleep_ms;

//=============================================================================
//	internal functions
//=============================================================================
static bool vl6180x_has_valid_sensor_ID();


/******************************************************************************
 * @brief Initializes global variables and checks sensor ID
 * 
 * @param[in] vl6180x_read 			function pointer for register read operations
 * @param[in] vl6180x_write 		function pointer for register write operations
 * @param[in] vl6180x_sleep_fn_ms 	function pointer for sleep/delay operations 
 * 
 * @param[out] true if all succeeds
 */
bool vl6180x_initialize(vl6180x_register_operation *vl6180x_read, vl6180x_register_operation *vl6180x_write, vl6180x_sleep_function *vl6180x_sleep_fn_ms)
{
	bool result = true;

	// assign function pointers
	if (vl6180x_read != NULL && vl6180x_write != NULL && vl6180x_sleep_fn_ms != NULL)
	{
		vl6180x_read_registers = vl6180x_read;
		vl6180x_write_registers = vl6180x_write;
		vl6180x_sleep_ms = vl6180x_sleep_fn_ms;
	}
	else
	{
		result = false;
	}

	// check sensor ID
	if(result == true)
	{
		result = vl6180x_has_valid_sensor_ID();
	}

	return result;
}


//=============================================================================
//	static functions
//=============================================================================

/******************************************************************************
 * @brief compare sensor ID value with register value
 * 
 * @param[out] true if matches
*/
static bool vl6180x_has_valid_sensor_ID()
{
	bool result = true;
	uint8_t data;

	result = vl6180x_read_registers(VL6180X_REGISTER_IDENTIFICATION_MODEL_ID, &data, 1);

	if (result == true)
	{
		if(data != VL6180X_REGISTER_IDENTIFICATION_MODEL_ID_VALUE)
		{
			result = false;
		}
	}

	return result;
}