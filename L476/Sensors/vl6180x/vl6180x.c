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
vl6180x_sleep_function     *vl6180x_sleep_ms;

//=============================================================================
//	internal functions
//=============================================================================
static bool vl6180x_has_valid_sensor_ID();

static bool vl6180x_has_startup_flag();
static bool vl6180x_clear_startup_flag();

static bool vl6180x_load_SR03_settings();
static bool vl6180x_load_recommended_configuration();


/******************************************************************************
 * @brief Initializes global variables and checks sensor ID
 * 
 * @param[in] vl6180x_read        function pointer for register read operations
 * @param[in] vl6180x_write       function pointer for register write operations
 * @param[in] vl6180x_sleep_fn_ms function pointer for sleep/delay operations 
 * 
 * @param[out] true               if all succeeds
 */
bool vl6180x_initialize(vl6180x_register_operation *vl6180x_read, vl6180x_register_operation *vl6180x_write, vl6180x_sleep_function *vl6180x_sleep_fn_ms)
{
	bool result = true;

	// assign function pointers
	if (vl6180x_read != NULL && vl6180x_write != NULL && vl6180x_sleep_fn_ms != NULL)
	{
		vl6180x_read_registers  = vl6180x_read;
		vl6180x_write_registers = vl6180x_write;
		vl6180x_sleep_ms        = vl6180x_sleep_fn_ms;
	}
	else
	{
		result = false;
	}

	// check sensor ID
	if (result == true)
	{
		result = vl6180x_has_valid_sensor_ID();
	}

	// do init sequence - AN4545, page 5
	if (result == true)
	{
		result = vl6180x_has_startup_flag();
	}

	if (result == true)
	{
		result = vl6180x_load_SR03_settings();
	}

	if (result == true)
	{
		result = vl6180x_load_recommended_configuration();
	}

	if (result == true)
	{
		result = vl6180x_clear_startup_flag();
	}

	return result;
}





//=============================================================================
//	static functions
//=============================================================================

/******************************************************************************
 * @brief Compare sensor ID value with register value
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


/******************************************************************************
 * @brief Check if the device has powered up
 * 
 * @param[out] true of just powered up
*/
static bool vl6180x_has_startup_flag()
{
	bool result = true;
	uint8_t data;

	// fresh_out_of_reset: Fresh out of reset bit, default of 1, user can set this to 0 after initial boot and
	// can therefore use this to check for a reset condition
	result = vl6180x_read_registers(VL6180X_REGISTER_SYSTEM_FRESH_OUT_OF_RESET, &data, 1);

	if(result == true && data == 0x1)
	{
		result = true;
	}
	else
	{
		result = false;
	}

	return result;
}


/******************************************************************************
 * @brief Clears the "fresh out of reset" flag to indicate init sequence completed
 * 
 * @param[out] true if register change succeeded
*/

static bool vl6180x_clear_startup_flag()
{
	bool result = true;
	uint8_t data = 0x00;

	// fresh_out_of_reset: Fresh out of reset bit, default of 1, user can set this to 0 after initial boot and
	// can therefore use this to check for a reset condition
	result = vl6180x_write_registers(VL6180X_REGISTER_SYSTEM_FRESH_OUT_OF_RESET, &data, 1);

	return result;
}


/******************************************************************************
 * @brief Load SR03 settings
 * 		  These are hardcoded values copied from AN4545 - section 9, page 24-25	
 * 
 * @param[out] true if writing to registers succeeds
*/
static bool vl6180x_load_SR03_settings()
{
	bool result = true;
	
	// Mandatory : private registers
	result &= vl6180x_write_registers(0x0207, &(uint8_t){0x01}, 1);
	result &= vl6180x_write_registers(0x0208, &(uint8_t){0x01}, 1);
	result &= vl6180x_write_registers(0x0096, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x0097, &(uint8_t){0xfd}, 1);
	result &= vl6180x_write_registers(0x00e3, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00e4, &(uint8_t){0x04}, 1);
	result &= vl6180x_write_registers(0x00e5, &(uint8_t){0x02}, 1);
	result &= vl6180x_write_registers(0x00e6, &(uint8_t){0x01}, 1);
	result &= vl6180x_write_registers(0x00e7, &(uint8_t){0x03}, 1);
	result &= vl6180x_write_registers(0x00f5, &(uint8_t){0x02}, 1);
	result &= vl6180x_write_registers(0x00d9, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x00db, &(uint8_t){0xce}, 1);
	result &= vl6180x_write_registers(0x00dc, &(uint8_t){0x03}, 1);
	result &= vl6180x_write_registers(0x00dd, &(uint8_t){0xf8}, 1);
	result &= vl6180x_write_registers(0x009f, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00a3, &(uint8_t){0x3c}, 1);
	result &= vl6180x_write_registers(0x00b7, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00bb, &(uint8_t){0x3c}, 1);
	result &= vl6180x_write_registers(0x00b2, &(uint8_t){0x09}, 1);
	result &= vl6180x_write_registers(0x00ca, &(uint8_t){0x09}, 1);
	result &= vl6180x_write_registers(0x0198, &(uint8_t){0x01}, 1);
	result &= vl6180x_write_registers(0x01b0, &(uint8_t){0x17}, 1);
	result &= vl6180x_write_registers(0x01ad, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00ff, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x0100, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x0199, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x01a6, &(uint8_t){0x1b}, 1);
	result &= vl6180x_write_registers(0x01ac, &(uint8_t){0x3e}, 1);
	result &= vl6180x_write_registers(0x01a7, &(uint8_t){0x1f}, 1);
	result &= vl6180x_write_registers(0x0030, &(uint8_t){0x00}, 1);	

	return result;
}


/******************************************************************************
 * @brief Load recommended settings
 * 		  copied from AN4545 - section 9, page 24-25	
 * 
 * @param[out] true if writing registers returns OK
*/
static bool vl6180x_load_recommended_configuration()
{
	bool result = true;

	// Recommended : Public registers - See data sheet for more detail
	result &= vl6180x_write_registers(0x0011, &(uint8_t){0x10}, 1);	// Enables polling for ‘New Sample ready’
																	// when measurement completes

	result &= vl6180x_write_registers(0x010a, &(uint8_t){0x30}, 1); // Set the averaging sample period
																	// (compromise between lower noise and
																	// increased execution time)

	result &= vl6180x_write_registers(0x003f, &(uint8_t){0x46}, 1); // Sets the light and dark gain (upper
																	// nibble). Dark gain should not be
																	// changed.

	result &= vl6180x_write_registers(0x0031, &(uint8_t){0xFF}, 1); // sets the # of range measurements after
																	// which auto calibration of system is
																	// performed

	result &= vl6180x_write_registers(0x0040, &(uint8_t){0x63}, 1); // Set ALS integration time to 100ms

	result &= vl6180x_write_registers(0x002e, &(uint8_t){0x01}, 1); // perform a single temperature calibration
																	// of the ranging sensor

	//Optional: Public registers - See data sheet for more detail
	result &= vl6180x_write_registers(0x001b, &(uint8_t){0x09}, 1); // Set default ranging inter-measurement
																	// period to 100ms

	result &= vl6180x_write_registers(0x003e, &(uint8_t){0x31}, 1); // Set default ALS inter-measurement period
																	// to 500ms

	result &= vl6180x_write_registers(0x0014, &(uint8_t){0x24}, 1); // Configures interrupt on ‘New Sample
																	// Ready threshold event’

	return result;
}