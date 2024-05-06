/*
 * tsc1641.c
 *
 *  Created on: May 2, 2024
 *      Author: tasifacij
 */


#include "tsc1641.h"

void TSC1641_SetConf(I2C_HandleTypeDef *hi2c1, Configuration * conf){
	uint8_t data[ 2 ] = { 0x00, 0x00 };
	data[ 0 ] = ( conf->TSC1641_RESET << 7 );
	data[ 1 ] = ((conf->TSC1641_CT)<<4) + ((conf->TSC1641_TEMP)<<3) + conf->TSC1641_MODE;
	uint8_t datasend[3] = { TSC1641_RegAdd_Conf, data[0], data[1] };
	HAL_I2C_Master_Transmit( hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000 );
}

void TSC1641_SetConf2(I2C_HandleTypeDef *hi2c1, Configuration_2 * pCnf){
	uint8_t data[ 2 ] = { 0x00, 0x00 };
	data[ 0 ] = ( pCnf->bitbuffer >> 8 );
	data[ 1 ] = ( pCnf->bitbuffer & 0xFF );
	uint8_t datasend[3] = { TSC1641_RegAdd_Conf, data[0], data[1] };
	HAL_I2C_Master_Transmit( hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000 );
}

/*write the shunt resisor value (TSC1641_RegAdd_RShunt) in the RShunt register*/
void TSC1641_SetRShunt(I2C_HandleTypeDef *hi2c1){
	uint8_t Data[2] = {(TSC1641_RShunt_Val>>8),(uint8_t)TSC1641_RShunt_Val};
	uint8_t Datasend[3] = {TSC1641_RegAdd_RShunt, Data[0], Data[1]};
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &Datasend[0], 3, 1000);
}

void TSC1641_SetAlerts(I2C_HandleTypeDef *hi2c1, Alert* ALERT1){
	uint8_t data[2] = {0x00, 0x00};
	data[0] = ((ALERT1->TSC1641_SOL<<7) + (ALERT1->TSC1641_SUL<<6) + (ALERT1->TSC1641_LOL<<5) + (ALERT1->TSC1641_LUL<<4) + (ALERT1->TSC1641_POL<<3) + (ALERT1->TSC1641_TOL<<2) + (ALERT1->TSC1641_CVNR<<1));
	data[1] = ((ALERT1->TSC1641_APOL<<1)) + ALERT1->TSC1641_ALEN;
	uint8_t datasend[3] = {TSC1641_RegAdd_MaskAl, data[0], data[1]};
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
}

void TSC1641_SetLimits(I2C_HandleTypeDef *hi2c1, Limit* LIMIT){
	uint8_t datasend[3];

	/* Write SHUNT OV limit */
	datasend[0] = TSC1641_RegAdd_VshuntOV;
	datasend[1] = (LIMIT->VSHUNT_OV_LIM)>>8;
	datasend[ 2 ] = LIMIT->VSHUNT_OV_LIM;
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	/* Write SHUNT UV limit */
	datasend[0] = TSC1641_RegAdd_VshuntUV;
	datasend[1] = (LIMIT->VSHUNT_UV_LIM)>>8;
	datasend[2] = LIMIT->VSHUNT_UV_LIM;
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	/* Write LOAD OV limit */
	datasend[0] = TSC1641_RegAdd_VloadOV;
	datasend[1] = (LIMIT->VLOAD_OV_LIM)>>8;
	datasend[2] = LIMIT->VLOAD_OV_LIM;
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	/* Write LOAD UV limit */
	datasend[0] = TSC1641_RegAdd_VloadUV;
	datasend[1] = (LIMIT->VLOAD_UV_LIM)>>8;
	datasend[2] = LIMIT->VLOAD_UV_LIM;
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	/* Write POWER OverLoad limit */
	datasend[0] = TSC1641_RegAdd_PowerOL;
	datasend[1] = (LIMIT->POWER_OV_LIM)>>8;
	datasend[2] = LIMIT->POWER_OV_LIM;
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	/* Write TEMP OverLoad limit */
	datasend[0] = TSC1641_RegAdd_TempOL;
	datasend[1] = (LIMIT->TEMP_OV_LIM)>>8;
	datasend[2] = LIMIT->TEMP_OV_LIM;
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
}

/*Get the flag regiuster by reading the FLAG register and store the data in the FLAG1 variable*/
void TSC1641_GetAlert(I2C_HandleTypeDef *hi2c1, Flag* FLAG1){
	uint8_t Data[2] = {0x00, 0x00};
	uint8_t datasend[3] = {TSC1641_RegAdd_Alert, Data[0], Data[1]};
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[ 0 ], 2, 1000);

	FLAG1->TSC1641_OVF = (Data[0]&0x40)>>6 ;
	FLAG1->TSC1641_SATF = (Data[0]&0x20)>>5 ;
	FLAG1->TSC1641_SOF = (Data[1]&0x40)>>6 ;
	FLAG1->TSC1641_SUF = (Data[1]&0x20)>>5 ;
	FLAG1->TSC1641_LOF = (Data[1]&0x10)>>4 ;
	FLAG1->TSC1641_LUF = (Data[1]&0x08)>>3 ;
	FLAG1->TSC1641_POF = (Data[1]&0x04)>>2 ;
	FLAG1->TSC1641_TOF = (Data[1]&0x02)>>1;
	FLAG1->TSC1641_CVRF = (Data[1]&0x01);
}

void TSC1641_GetShuntVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]){
	uint8_t datasend[1] = {TSC1641_RegAdd_ShuntV};
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[ 0 ], 2, 1000);
}

void TSC1641_GetVloadVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]){
	uint8_t datasend[1] = {TSC1641_RegAdd_LoadV};
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[0], 2, 1000);
}

void TSC1641_GetCurrentVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]){
	uint8_t datasend[1] = {TSC1641_RegAdd_Current};
	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[0], 2, 1000);
}


