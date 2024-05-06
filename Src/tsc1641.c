/*
 * tsc1641.c
 *
 *  Created on: May 2, 2024
 *      Author: tasifacij
 */


#include "tsc1641.h"
#include "tsc1641_cfg.h"
#include "stm32l4xx_hal_i2c.h"
#include "i2c.h"

//===========================================  forward declarations ===========================================
//static HAL_StatusTypeDef TSC1641_SetConf2_p(I2C_HandleTypeDef *hi2c, RegConfiguration * conf);
static HAL_StatusTypeDef TSC1641_SetRShunt_p(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef TSC1641_SetMask_p(I2C_HandleTypeDef *hi2c, RegMask* reg);
static HAL_StatusTypeDef TSC1641_SetLimits_p(I2C_HandleTypeDef *hi2c, Limit* LIMIT);

//static void TSC1641_GetAlert(I2C_HandleTypeDef *hi2c1, Flag* FLAG1);
//static void TSC1641_GetShuntVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]);
//static void TSC1641_GetVloadVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]);
static void TSC1641_GetCurrentVal_p(I2C_HandleTypeDef *hi2c, uint8_t Data[]);

//static void set_handle( TSC1641_NUM_T instance );

//===========================================  TYPES ===========================================

typedef struct{
	I2C_HandleTypeDef* 	i2c_handle;
}TSC1641Handle;

//===========================================  MEMBERS ===========================================

static I2C_HandleTypeDef* I2C_Instances[] = {
	&hi2c1
};

#define NUM_OF_INSTANCES ( sizeof( I2C_Instances ) / sizeof ( I2C_Instances[ 0 ] ) )

//#define NUM_OF_INSTANCES 1


static TSC1641Handle tsc_handles[ NUM_OF_INSTANCES ] = {
	{
		.i2c_handle = NULL
	}
};

static uint8_t DataCurrent[ 2 ];

//===========================================  INTERFACE ===========================================

HAL_StatusTypeDef TSC1641Initialize( TSC1641_NUM_T instance ){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES || tsc_handles[ instance ].i2c_handle != NULL ){
		retval = HAL_ERROR;
	}
	else{
		tsc_handles[ instance ].i2c_handle	= I2C_Instances[ instance ];
	}

	return retval;
}

HAL_StatusTypeDef TSC1641SetConf( TSC1641_NUM_T instance, RegConfiguration * rcnf ){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES  ){
		retval = HAL_ERROR;
	}else{
		retval = TSC1641_SetConf2_p( &hi2c1, rcnf );
	}

	return retval;
}

HAL_StatusTypeDef TSC1641SetRShunt( TSC1641_NUM_T instance ){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES ){
		retval = HAL_ERROR;
	}else{
		retval = TSC1641_SetRShunt_p( &hi2c1 );
	}

	return retval;
}

HAL_StatusTypeDef TSC1641SetLimits( TSC1641_NUM_T instance , Limit* LIMIT){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES  ){
		retval = HAL_ERROR;
	}else{
		retval = TSC1641_SetLimits_p( &hi2c1, LIMIT );
	}

	return retval;
}

HAL_StatusTypeDef TSC1641SetMask( TSC1641_NUM_T instance, RegMask* reg){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES  ){
		retval = HAL_ERROR;
	}else{
		retval = TSC1641_SetMask_p( &hi2c1, reg );
	}

	return retval;
}

double TSC1641GetCurrentAmp( TSC1641_NUM_T instance ){
	if( instance >= NUM_OF_INSTANCES  ){
		return 0.0;
	}else{
		TSC1641_GetCurrentVal_p( &hi2c1, DataCurrent );
		return I_LSB * DataCurrent[ 1 ];
	}
}

//=========================================== impl ===========================================
volatile static HAL_StatusTypeDef last_status;

HAL_StatusTypeDef TSC1641_SetConf2_p(I2C_HandleTypeDef *hi2c, RegConfiguration * pCnf){
	uint8_t data[ 2 ] = { 0x00, 0x00 };
	data[ 0 ] = ( pCnf->bitbuffer >> 8 );
	data[ 1 ] = ( pCnf->bitbuffer & 0xFF );
	uint8_t datasend[3] = { TSC1641_RegAdd_Conf, data[0], data[1] };
	last_status = HAL_I2C_Master_Transmit( hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000 );
	return last_status;
}

/*write the shunt resisor value (TSC1641_RegAdd_RShunt) in the RShunt register*/
static HAL_StatusTypeDef TSC1641_SetRShunt_p(I2C_HandleTypeDef *hi2c){
	uint8_t Data[2] = {(TSC1641_RShunt_Val>>8),(uint8_t)TSC1641_RShunt_Val};
	uint8_t Datasend[3] = {TSC1641_RegAdd_RShunt, Data[0], Data[1]};
	return HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &Datasend[0], 3, 1000);
}

static HAL_StatusTypeDef TSC1641_SetMask_p(I2C_HandleTypeDef *hi2c, RegMask* reg){
	uint8_t data[ 2 ] = { 0x00, 0x00 };
	data[ 0 ] = ( reg->bitbuffer >> 8 );
	data[ 1 ] = ( reg->bitbuffer & 0xFF );
	uint8_t datasend[3] = {TSC1641_RegAdd_MaskAl, data[0], data[1]};
	return HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
}

static HAL_StatusTypeDef TSC1641_SetLimits_p(I2C_HandleTypeDef *hi2c, Limit* LIMIT){
	uint8_t datasend[3];

	/* Write SHUNT OV limit */
	datasend[0] = TSC1641_RegAdd_VshuntOV;
	datasend[1] = (LIMIT->VSHUNT_OV_LIM)>>8;
	datasend[ 2 ] = LIMIT->VSHUNT_OV_LIM;
	HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write SHUNT UV limit */
	datasend[0] = TSC1641_RegAdd_VshuntUV;
	datasend[1] = (LIMIT->VSHUNT_UV_LIM)>>8;
	datasend[2] = LIMIT->VSHUNT_UV_LIM;
	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write LOAD OV limit */
	datasend[0] = TSC1641_RegAdd_VloadOV;
	datasend[1] = (LIMIT->VLOAD_OV_LIM)>>8;
	datasend[2] = LIMIT->VLOAD_OV_LIM;
	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write LOAD UV limit */
	datasend[0] = TSC1641_RegAdd_VloadUV;
	datasend[1] = (LIMIT->VLOAD_UV_LIM)>>8;
	datasend[2] = LIMIT->VLOAD_UV_LIM;
	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write POWER OverLoad limit */
	datasend[0] = TSC1641_RegAdd_PowerOL;
	datasend[1] = (LIMIT->POWER_OV_LIM)>>8;
	datasend[2] = LIMIT->POWER_OV_LIM;
	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write TEMP OverLoad limit */
	datasend[0] = TSC1641_RegAdd_TempOL;
	datasend[1] = (LIMIT->TEMP_OV_LIM)>>8;
	datasend[2] = LIMIT->TEMP_OV_LIM;
	return HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
}

/*Get the flag regiuster by reading the FLAG register and store the data in the FLAG1 variable*/
//static void TSC1641_GetAlert(I2C_HandleTypeDef *hi2c1, Flag* FLAG1){
//	uint8_t Data[2] = {0x00, 0x00};
//	uint8_t datasend[3] = {TSC1641_RegAdd_Alert, Data[0], Data[1]};
//	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
//	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[ 0 ], 2, 1000);
//
//	FLAG1->TSC1641_OVF = (Data[0]&0x40)>>6 ;
//	FLAG1->TSC1641_SATF = (Data[0]&0x20)>>5 ;
//	FLAG1->TSC1641_SOF = (Data[1]&0x40)>>6 ;
//	FLAG1->TSC1641_SUF = (Data[1]&0x20)>>5 ;
//	FLAG1->TSC1641_LOF = (Data[1]&0x10)>>4 ;
//	FLAG1->TSC1641_LUF = (Data[1]&0x08)>>3 ;
//	FLAG1->TSC1641_POF = (Data[1]&0x04)>>2 ;
//	FLAG1->TSC1641_TOF = (Data[1]&0x02)>>1;
//	FLAG1->TSC1641_CVRF = (Data[1]&0x01);
//}

//static void TSC1641_GetShuntVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]){
//	uint8_t datasend[1] = {TSC1641_RegAdd_ShuntV};
//	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
//	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[ 0 ], 2, 1000);
//}

//static void TSC1641_GetVloadVal(I2C_HandleTypeDef *hi2c1, uint8_t Data[]){
//	uint8_t datasend[1] = {TSC1641_RegAdd_LoadV};
//	HAL_I2C_Master_Transmit(hi2c1, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
//	HAL_I2C_Master_Receive(hi2c1, I2C_TSC1641_ADD_R, &Data[0], 2, 1000);
//}

static void TSC1641_GetCurrentVal_p(I2C_HandleTypeDef *hi2c, uint8_t Data[]){
	uint8_t datasend[1] = {TSC1641_RegAdd_Current};
	HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 1, 1000);
	HAL_I2C_Master_Receive(hi2c, I2C_TSC1641_ADD_R, &Data[0], 2, 1000);
}

