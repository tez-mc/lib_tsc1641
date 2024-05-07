#pragma once

// Rsh = Rdig * Rlsb
// Rdig = Rsh / Rlsb = 5e-3 / 10e-6 == 500 == 0x01F4

#define V_SHUNT_LSB 	2.5e-6//[V]
#define V_LOAD_LSB		2e-3//[V]
#define RSHUNT			5e-3//[Ohm]
#define RSHUNT_LSB		10e-6//[Ohm]
#define I_LSB 			( V_SHUNT_LSB / RSHUNT )



#define TSC1641_RShunt_Val	0x01f4	// Rshunt LSB is 10uOhm so Rshunt of 5mOhm = 0d1000 0x01f4
//#define TSC1641_RShunt_Val	( uint16_t )( RSHUNT / RSHUNT_LSB )

/*------------------I2C adress ------------------------------------*/
#define I2C_TSC1641_DEFAULT_ADDR	0x40										// 5.2.1 Serial bus address
#define I2C_TSC1641_ADD_W			0x80									// Value if A1 and A0 are connected to GND. Otherwise refer to the datasheet.
#define I2C_TSC1641_ADD_R			0x81									// Value if A1 and A0 are connected to GND. Otherwise refer to the datasheet.
#define I2C_ADDR_INVALID			0x0

#define I2C1_FD_ENABLED	1





