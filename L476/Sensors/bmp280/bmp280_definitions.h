/*
 * bmp280_definitions.h
 *
 *  Created on: Nov 20, 2023
 *      Author: Aniel
 */

#ifndef BMP280_BMP280_DEFINITIONS_H_
#define BMP280_BMP280_DEFINITIONS_H_


//=============================================================================
//	Device addresses
//=============================================================================

#define BMP280_I2C_DEVICE_ADDRESS		(0x76 << 1)
#define BMP280_I2C_DEVICE_ADDRESS_ALT	(0x77 << 1)

//=============================================================================
//	Memory addresses
//=============================================================================

#define BMP280_ADDRESS_MEASUREMENT_DATA_START	0xF7
#define BMP280_ADDRESS_MEASUREMENT_CONTROL 		0xF4
#define BMP280_ADDRESS_CONFIG					0xF5
#define BMP280_ADDRESS_RESET					0xE0
#define BMP280_ADDRESS_ID						0xD0
#define BMP280_ADDRESS_CALIBRATION_START		0x88

#define BMP280_LENGTH_MEASUREMENT_DATA			6
#define BMP280_LENGTH_CALIBRATION				26

//=============================================================================
//	predefined values
//=============================================================================

#define BMP280_VALUE_MEASUREMENT_RESET		0x80
#define BMP280_VALUE_ID						0x58

//=============================================================================
// Measurement control
//=============================================================================

typedef enum
{
	BMP280_PRESSURE_OVERSAMPLING_MEASUREMENT_OFF = (0b000 << 2),
	BMP280_PRESSURE_OVERSAMPLING_1X_ULTRA_LOW_POWER = (0b001 << 2),
	BMP280_PRESSURE_OVERSAMPLING_2X_LOW_POWER = (0b010 << 2),
	BMP280_PRESSURE_OVERSAMPLING_4X_STANDARD_RESOLUTION = (0b011 << 2),
	BMP280_PRESSURE_OVERSAMPLING_8X_HIGH_RESOLUTION = (0b100 << 2),
	BMP280_PRESSURE_OVERSAMPLING_16X_ULTRA_HIGH_RESOLUTION = (0b101 << 2),
}bmp280_pressure_oversampling_enum;

typedef enum
{
	BMP280_TEMPERATURE_OVERSAMPLING_MEASUREMENT_OFF = (0b000 << 5),
	BMP280_TEMPERATURE_OVERSAMPLING_1X_ULTRA_LOW_POWER = (0b001 << 5),
	BMP280_TEMPERATURE_OVERSAMPLING_2X_LOW_POWER = (0b010 << 5),
	BMP280_TEMPERATURE_OVERSAMPLING_4X_STANDARD_RESOLUTION = (0b011 << 5),
	BMP280_TEMPERATURE_OVERSAMPLING_8X_HIGH_RESOLUTION = (0b100 << 5),
	BMP280_TEMPERATURE_OVERSAMPLING_16X_ULTRA_HIGH_RESOLUTION = (0b101 << 5),
}bmp280_temperature_oversampling_enum;


typedef enum
{
	BMP280_POWER_MODE_SLEEP = 0b00,
	BMP280_POWER_MODE_FORCED = 0b01,
	BMP280_POWER_MODE_NORMAL = 0b11,
}bmp280_power_mode_enum;


//=============================================================================
//	config
//=============================================================================

typedef enum
{
	BMP280_STANDBY_TIME_0_5_MS = (0b000 << 5),
	BMP280_STANDBY_TIME_62_5_MS = (0b001 << 5),
	BMP280_STANDBY_TIME_125_MS = (0b010 << 5),
	BMP280_STANDBY_TIME_250_MS = (0b011 << 5),
	BMP280_STANDBY_TIME_500_MS = (0b100 << 5),
	BMP280_STANDBY_TIME_1000_MS = (0b101 << 5),
	BMP280_STANDBY_TIME_2000_MS = (0b110 << 5),
	BMP280_STANDBY_TIME_4000_MS = (0b111 << 5),
}bmp280_standby_time_enum;

typedef enum
{
	BMP280_FILTER_OFF = (0b000 << 2),
	BMP280_FILTER_COEFFIENT_2X = (0b001 << 2),
	BMP280_FILTER_COEFFIENT_4X = (0b010 << 2),
	BMP280_FILTER_COEFFIENT_8X = (0b011 << 2),
	BMP280_FILTER_COEFFIENT_16X = (0b100 << 2),
}bmp280_filter_coefficient;

typedef enum
{
	BMP280_SPI3W_DISABLED = 0b0,
	BMP280_SPI3W_ENABLED = 0b1,
}bmp280_spi3w_enabled_enum;




#endif /* BMP280_BMP280_DEFINITIONS_H_ */
