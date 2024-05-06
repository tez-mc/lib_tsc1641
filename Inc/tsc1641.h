#pragma once

#include <stdint.h>
#include "main.h"
#include "i2c.h"

/*------------------Shunt value------------------------------------*/

#define TSC1641_RShunt_Val	0x01f4	// In this example, Rshunt LSB is 5�Ohm so Rshunt of 5mOhm = 0d1000 0x01f4

/*------------------I2C adress ------------------------------------*/
#define I2C_TSC1641_ADD_W	0x80
#define I2C_TSC1641_ADD_R	0x81

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
#define TSC1641_rst_On							0x01  /*RST off*/
#define TSC1641_rst_Off							0x00  /*RST on */

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

#define TSC1641_Temp_Off						0x00  /*Temperatrue sensor off*/
#define TSC1641_Temp_On							0x01  /*Temperatrue sensor on */

/*mode*/
#define TSC1641_Mode_ShutDown				0x00	/*mode shutdown                */
#define TSC1641_Mode_Vsh_Trig				0x01	/*mode trigger Vshunt only     */
#define TSC1641_Mode_Vload_Trig			0x02	/*mode trigger Vload only      */
#define TSC1641_Mode_Vshload_Trig		0x03	/*mode trigger Vshunt and Vload*/
#define TSC1641_Mode_Idle						0x04	/*mode Idle                    */
#define TSC1641_Mode_VshCont				0x05	/*mode continuous Vshunt only  */
#define TSC1641_Mode_VloadCont			0x06	/*mode continuous Vload only   */
#define TSC1641_Mode_VshloadCont		0x07	/*mode continuous Vshunt and Vload*/


/* ------------------Alert-----------------------------------------*/
#define TSC1641_Alert_On							0x01
#define TSC1641_Alert_Off							0x00

typedef struct{
	uint8_t TSC1641_RESET ;
	uint8_t TSC1641_CT ;
	uint8_t TSC1641_TEMP ;
	uint8_t TSC1641_MODE ;
} Configuration;

// definition of the ALERT register
typedef struct{
	uint8_t TSC1641_SOL ;		// Shunt Voltage Over voltage
	uint8_t TSC1641_SUL ;		// Shunt Voltage Under voltage
	uint8_t TSC1641_LOL ;		// Load Voltage Over voltage
	uint8_t TSC1641_LUL ;		// Load Voltage Under voltage
	uint8_t TSC1641_POL ;		// Power Over Limit
	uint8_t TSC1641_TOL ;		// Temperature Over Limit
	uint8_t TSC1641_CVNR ;	// Conversion ready alert enable
	uint8_t TSC1641_APOL ;	// Alert polarity
	uint8_t TSC1641_ALEN ;	// Alert Latch Enable
} Alert;

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

void TSC1641_SetConf(I2C_HandleTypeDef *hi2c1, Configuration * CONF1);
void TSC1641_SetRShunt(I2C_HandleTypeDef *hi2c1);
void TSC1641_SetAlerts(I2C_HandleTypeDef *hi2c1, Alert* ALERT1);
void TSC1641_SetLimits(I2C_HandleTypeDef *hi2c1, Limit* LIMIT);
void TSC1641_GetAlert(I2C_HandleTypeDef *hi2c1, Flag* FLAG1);
void TSC1641_GetShuntVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]);
void TSC1641_GetVloadVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]);
void TSC1641_GetCurrentVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]);






