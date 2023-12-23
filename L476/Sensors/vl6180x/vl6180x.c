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

static bool vl6180x_retrieve_measurement(float *distance_mm);
static bool vl6180x_clear_data_ready_interrupt();

/******************************************************************************
 * @brief Initializes global variables and checks sensor ID
 * 
 * @param[in] vl6180x_read        function pointer for register read operations
 * @param[in] vl6180x_write       function pointer for register write operations
 * @param[in] vl6180x_sleep_fn_ms function pointer for sleep/delay operations 
 * 
 * @param[out] true               if all succeeds
 */
bool  vl6180x_initialize(vl6180x_register_operation *vl6180x_read, vl6180x_register_operation *vl6180x_write, vl6180x_sleep_function *vl6180x_sleep_fn_ms)
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


/******************************************************************************
 * @brief Start single measurement
 * 
 * @pre device Initialized
 * 
 * @param[out] true if single measurement started
*/
bool vl680x_request_single_measurement()
{
	bool result = true;
}


/******************************************************************************
 * @brief Start continuous measurement
 * 
 * @pre device initialized
 * 
 * @param[out] true if continuous mode started
*/
bool vl6180x_start_continuous_measurements()
{
	bool result = true;
}


/******************************************************************************
 * @brief Stop continuous measurements
 * 
 * @param[out] true if continuous mode stopped
*/
bool vl6180x_stop_continous_measurements()
{
	bool result = true;
}


/******************************************************************************
 * @brief Check if new data is available
 * 
 * @pre Single or Continuos measurement started
 * 
 * @param[out] true if check was OK and data available
*/
bool vl6180x_is_measurement_ready()
{
	bool result = true;
}


/******************************************************************************
 * @brief Get results of single measurement
 * 
 * @pre measurement ready
 * 
 * @param[out] distance in mm
 * @param[out] true if succeeded
*/
bool vl6180x_get_measurement_result(float *distance_mm)
{
	bool result = true
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
	result &= vl6180x_write_registers(0x0097, &(uint8_t){0xFD}, 1);
	result &= vl6180x_write_registers(0x00E3, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00E4, &(uint8_t){0x04}, 1);
	result &= vl6180x_write_registers(0x00E5, &(uint8_t){0x02}, 1);
	result &= vl6180x_write_registers(0x00E6, &(uint8_t){0x01}, 1);
	result &= vl6180x_write_registers(0x00E7, &(uint8_t){0x03}, 1);
	result &= vl6180x_write_registers(0x00F5, &(uint8_t){0x02}, 1);
	result &= vl6180x_write_registers(0x00D9, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x00DB, &(uint8_t){0xCE}, 1);
	result &= vl6180x_write_registers(0x00DC, &(uint8_t){0x03}, 1);
	result &= vl6180x_write_registers(0x00DD, &(uint8_t){0xF8}, 1);
	result &= vl6180x_write_registers(0x009F, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00A3, &(uint8_t){0x3C}, 1);
	result &= vl6180x_write_registers(0x00B7, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00BB, &(uint8_t){0x3C}, 1);
	result &= vl6180x_write_registers(0x00B2, &(uint8_t){0x09}, 1);
	result &= vl6180x_write_registers(0x00CA, &(uint8_t){0x09}, 1);
	result &= vl6180x_write_registers(0x0198, &(uint8_t){0x01}, 1);
	result &= vl6180x_write_registers(0x01B0, &(uint8_t){0x17}, 1);
	result &= vl6180x_write_registers(0x01AD, &(uint8_t){0x00}, 1);
	result &= vl6180x_write_registers(0x00FF, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x0100, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x0199, &(uint8_t){0x05}, 1);
	result &= vl6180x_write_registers(0x01A6, &(uint8_t){0x1B}, 1);
	result &= vl6180x_write_registers(0x01AC, &(uint8_t){0x3E}, 1);
	result &= vl6180x_write_registers(0x01A7, &(uint8_t){0x1F}, 1);
	result &= vl6180x_write_registers(0X0030, &(uint8_t){0x00}, 1);	

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

	// Enables polling for ‘New Sample ready’ when measurement completes
	result &= vl6180x_write_registers(0x0011, &(uint8_t){0x10}, 1);

	// Set the averaging sample period (compromise between lower noise and
	// increased execution time)
	result &= vl6180x_write_registers(0x010A, &(uint8_t){0x30}, 1);

	// Sets the light and dark gain (upper nibble). Dark gain should not be changed.
	result &= vl6180x_write_registers(0x003F, &(uint8_t){0x46}, 1);

	// sets the # of range measurements after which auto calibration of system is performed
	result &= vl6180x_write_registers(0x0031, &(uint8_t){0xFF}, 1); 

	// Set ALS integration time to 100ms
	result &= vl6180x_write_registers(0x0040, &(uint8_t){0x63}, 1);

	// perform a single temperature calibration of the ranging sensor
	result &= vl6180x_write_registers(0x002E, &(uint8_t){0x01}, 1); 

	//Optional: Public registers - See daaa sheet for more detail
	
	// Set default ranging inter-measurement period to 100ms
	result &= vl6180x_write_registers(0x001B, &(uint8_t){0x09}, 1);

	// Set default ALS inter-measurement period to 500ms
	result &= vl6180x_write_registers(0x003E, &(uint8_t){0x31}, 1); 

	// Configures interrupt on ‘New Sample ready threshold event’
	result &= vl6180x_write_registers(0x0014, &(uint8_t){0x24}, 1);

	return result;
}


/******************************************************************************
 * @brief reads measurement register
 * 
 * @param[out] distance in mm
 * @param[out] true if succeeded
*/
static bool vl6180x_retrieve_measurement(float *distance_mm)
{
	bool result = true;
}


/******************************************************************************
 * @brief clears data ready interrupt
 * 
 * @param[out] true if succeeded
*/
static bool vl6180x_clear_data_ready_interrupt()
{
	bool result = true;
}