#pragma once

#include <stdint.h>
#include <math.h>
#include "main.h"
#include "i2c.h"



/*------------------register map-----------------------------------*/
#define TSC1641_RegAdd_Conf					0x00								//configuration register
#define TSC1641_RegAdd_ShuntV				0x01								//read only. LSB=2.5uV
#define TSC1641_RegAdd_LoadV				0x02								//read only. LSB=2mV
#define TSC1641_RegAdd_Power				0x03								//read only. LSB=2.5mW
#define TSC1641_RegAdd_Current				0x04								//read only. LSB=2.5uV/Rshunt
#define TSC1641_RegAdd_Temp					0x05								//read only. LSB=0.5�C
#define TSC1641_RegAdd_MaskAl				0x06								//Mask register
#define TSC1641_RegAdd_Alert				0x07								//read only
#define TSC1641_RegAdd_RShunt				0x08								//shunt register. LSB=10uOhm
#define TSC1641_RegAdd_VshuntOV				0x09								//Alert threshold. LSB=2.5uV
#define TSC1641_RegAdd_VshuntUV				0x0A								//Alert threshold. LSB=2.5uV
#define TSC1641_RegAdd_VloadOV				0x0B								//Alert threshold. LSB=2mV
#define TSC1641_RegAdd_VloadUV				0x0C								//Alert threshold. LSB=2mV
#define TSC1641_RegAdd_PowerOL				0x0D								//Alert threshold. LSB=25mW. Works only if VLOAD>0
#define TSC1641_RegAdd_TempOL				0x0E								//Alert threshold. LSB=0.5�C
#define TSC1641_RegAdd_ManufID				0xFE
#define TSC1641_RegAdd_DieID				0xFF


/*-----------------configuration register---------------------------*/
#define TSC1641_rst_On						0x01  /*RST off*/
#define TSC1641_rst_Off						0x00  /*RST on */

/*conversion time*/
#define TSC1641_Conf_CT_128					0x00  /*conversion time of 128us*/
#define TSC1641_Conf_CT_256					0x01
#define TSC1641_Conf_CT_512					0x02
#define TSC1641_Conf_CT_1024				0x03
#define TSC1641_Conf_CT_2048				0x04
#define TSC1641_Conf_CT_4096				0x05
#define TSC1641_Conf_CT_8192				0x06
#define TSC1641_Conf_CT_16384				0x07
#define TSC1641_Conf_CT_32768				0x08	/*conversion time of 32ms*/

#define TSC1641_Temp_Off					0x00  /*Temperatrue sensor off*/
#define TSC1641_Temp_On						0x01  /*Temperatrue sensor on */

/*mode*/
#define TSC1641_Mode_ShutDown				0x00	/*mode shutdown                */
#define TSC1641_Mode_Vsh_Trig				0x01	/*mode trigger Vshunt only     */
#define TSC1641_Mode_Vload_Trig				0x02	/*mode trigger Vload only      */
#define TSC1641_Mode_Vshload_Trig			0x03	/*mode trigger Vshunt and Vload*/
#define TSC1641_Mode_Idle					0x04	/*mode Idle                    */
#define TSC1641_Mode_VshCont				0x05	/*mode continuous Vshunt only  */
#define TSC1641_Mode_VloadCont				0x06	/*mode continuous Vload only   */
#define TSC1641_Mode_VshloadCont			0x07	/*mode continuous Vshunt and Vload*/


/* ------------------Alert-----------------------------------------*/
#define TSC1641_Alert_On							0x01
#define TSC1641_Alert_Off							0x00




// 5.3.1 Configuration register (00h)
typedef struct{
	union{
		uint16_t bitbuffer;
		struct{
			uint8_t M02		: 3;
			uint8_t TEMP 	: 1;
			uint8_t CT03 	: 4;
			uint8_t EMPTY 	: 7;
			uint8_t RST 	: 1;
		}bits;
	};
} RegConfiguration;

// definition of the ALERT register
// 5.3.7 Mask register (06h)
typedef
	union{
		struct{
			uint8_t ALEN 	: 1;	// Alert Latch Enable
			uint8_t APOL 	: 1;	// Alert polarity
			uint8_t EMPTY_1	: 6;
			uint8_t EMPTY_2	: 1;
			uint8_t CVNR  	: 1;	// Conversion ready alert enable
			uint8_t TOL 	: 1;		// Temperature Over Limit
			uint8_t POL 	: 1;		// Power Over Limit
			uint8_t LUL 	: 1;		// Load Voltage Under voltage
			uint8_t LOL 	: 1;		// Load Voltage Over voltage
			uint8_t SUL 	: 1;		// Shunt Voltage Under voltage
			uint8_t SOL 	: 1;		// Shunt Voltage Over voltage

		}bits;
		uint16_t bitbuffer;
} RegMask;


// Definition of the parameters in alert register :
typedef struct{
	uint8_t TSC1641_OVF ;		// Math Overflow Flag
	uint8_t TSC1641_SATF ;	// Measurement saturation Flag
	uint8_t TSC1641_SOF ;		// Shunt Voltage Over voltage
	uint8_t TSC1641_SUF ;		// Shunt Voltage Under voltage
	uint8_t TSC1641_LOF ;		// Load Voltage Over voltage
	uint8_t TSC1641_LUF ;		// Load Voltage Under voltage
	uint8_t TSC1641_POF ;		// Power Over Limit
	uint8_t TSC1641_TOF ;		// Temperature Over Limit
	uint8_t TSC1641_CVRF ;	// Conversion ready alert enable
}Flag;

// Definition of alert thresholds :
typedef struct{
	uint16_t VSHUNT_OV_LIM ;	// Vsunt Over voltage limit value
	uint16_t VSHUNT_UV_LIM ;		// Vshunt Under voltage limit value
	uint16_t VLOAD_OV_LIM ;	// Vload Over voltage limit value
	uint16_t VLOAD_UV_LIM ;		// Vload Under voltage limit value
	uint16_t POWER_OV_LIM ;		// Power over limit value
	uint16_t TEMP_OV_LIM ;		// Temperature over limit value
}Limit;




typedef enum
{
	TSC1641_FD_1 = 0x0,              /*hi2c1*/
	TSC1641_FD_2 = 0x1,              /*hi2c1*/
	TSC1641_FD_3 = 0x2,              /*hi2c2*/
	TSC1641_Invailid
}TSC1641_NUM_T;

HAL_StatusTypeDef TSC1641Initialize( TSC1641_NUM_T instance );
HAL_StatusTypeDef TSC1641SetConf( TSC1641_NUM_T instance, RegConfiguration * conf );

HAL_StatusTypeDef TSC1641_SetConf2_p(I2C_HandleTypeDef *hi2c, RegConfiguration * pCnf);

HAL_StatusTypeDef TSC1641SetRShunt( TSC1641_NUM_T instance );
HAL_StatusTypeDef TSC1641SetLimits( TSC1641_NUM_T instance , Limit* LIMIT);
HAL_StatusTypeDef TSC1641SetMask( TSC1641_NUM_T instance, RegMask* reg);
double TSC1641GetCurrentAmp( TSC1641_NUM_T instance );


