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

typedef struct{
	TSC1641_NUM_T 	i2c_id;
	uint16_t devReadAddress;
	uint16_t devWriteAddress;
}TSC1641Handle;

//===========================================  forward declarations ===========================================
static HAL_StatusTypeDef TSC1641_SetConf2_p( TSC1641Handle *tscHandle, RegConfiguration * conf );
static HAL_StatusTypeDef TSC1641_SetRShunt_p( TSC1641Handle* tscHandle );
static HAL_StatusTypeDef TSC1641_SetMask_p( TSC1641Handle* tscHandle, RegMask* reg );
static HAL_StatusTypeDef TSC1641_SetLimits_p( TSC1641Handle* tscHandle, Limit* LIMIT);

static void TSC1641_GetCurrentVal_p( TSC1641Handle* tscHandle, uint8_t Data[] );


//===========================================  MEMBERS ===========================================

static I2C_HandleTypeDef* I2C_Instances[] = {
#if I2C1_FD_ENABLED
	&hi2c1
#endif
#if I2C2_FD_ENQBLED
	, &hi2c2
#endif
#if I2C3_FD_ENQBLED
	, &hi2c3
#endif
};

#define NUM_OF_INSTANCES ( sizeof( I2C_Instances ) / sizeof ( I2C_Instances[ 0 ] ) )

static TSC1641Handle tsc_handles[ NUM_OF_INSTANCES ] = {
	{
		.i2c_id = TSC1641_Invailid,
		.devReadAddress = I2C_ADDR_INVALID,
		.devWriteAddress = I2C_ADDR_INVALID
	}
};

static uint8_t DataCurrent[ 2 ];

//===========================================  INTERFACE ===========================================

HAL_StatusTypeDef TSC1641Initialize( TSC1641_NUM_T instance ){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES || tsc_handles[ instance ].i2c_id != TSC1641_Invailid ){
		retval = HAL_ERROR;
	}
	else{
		tsc_handles[ instance ].i2c_id	= instance;
		tsc_handles[ instance ].devReadAddress = I2C_TSC1641_ADD_R;
		tsc_handles[ instance ].devWriteAddress = I2C_TSC1641_ADD_W;
	}

	return retval;
}

// 5.3.1 Configuration register (00h)
HAL_StatusTypeDef TSC1641SetConf( TSC1641_NUM_T instance, RegConfiguration * rcnf ){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id  ){
		retval = HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		retval = TSC1641_SetConf2_p( tscHandle, rcnf );
	}

	return retval;
}

// 5.3.9 Rshunt register (08h)
HAL_StatusTypeDef TSC1641SetRShunt( TSC1641_NUM_T instance ){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		retval = HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		retval = TSC1641_SetRShunt_p( tscHandle );
	}

	return retval;
}

// 5.3.10 - 5.3.15
HAL_StatusTypeDef TSC1641SetLimits( TSC1641_NUM_T instance , Limit* LIMIT){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		retval = HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		retval = TSC1641_SetLimits_p( tscHandle, LIMIT );
	}

	return retval;
}

// 5.3.7 Mask register (06h)
HAL_StatusTypeDef TSC1641SetMask( TSC1641_NUM_T instance, RegMask* reg){
	HAL_StatusTypeDef retval = HAL_OK;

	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		retval = HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		retval = TSC1641_SetMask_p( tscHandle, reg );
	}

	return retval;
}

double TSC1641GetCurrentAmp( TSC1641_NUM_T instance ){
	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		return 0.0;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		TSC1641_GetCurrentVal_p( tscHandle, DataCurrent );
		return I_LSB * DataCurrent[ 1 ];
	}
}

//=========================================== impl ===========================================
volatile static HAL_StatusTypeDef last_status;

HAL_StatusTypeDef TSC1641_SetConf2_p( TSC1641Handle *tscHandle, RegConfiguration * pCnf){
	 I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
	uint8_t data[ 2 ] = { 0x00, 0x00 };
	data[ 0 ] = ( pCnf->bitbuffer >> 8 );
	data[ 1 ] = ( pCnf->bitbuffer & 0xFF );
	uint8_t datasend[3] = { TSC1641_RegAdd_Conf, data[0], data[1] };
	last_status = HAL_I2C_Master_Transmit( hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000 );
	return last_status;
}

/*write the shunt resisor value (TSC1641_RegAdd_RShunt) in the RShunt register*/
static HAL_StatusTypeDef TSC1641_SetRShunt_p( TSC1641Handle* tscHandle ){
	I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
	uint8_t Data[2] = {(TSC1641_RShunt_Val>>8),(uint8_t)TSC1641_RShunt_Val};
	uint8_t Datasend[3] = {TSC1641_RegAdd_RShunt, Data[0], Data[1]};
//	return HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &Datasend[0], 3, 1000);
	return HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &Datasend[0], 3, 1000);
}

static HAL_StatusTypeDef TSC1641_SetMask_p( TSC1641Handle* tscHandle, RegMask* reg){
	I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
	uint8_t data[ 2 ] = { 0x00, 0x00 };
	data[ 0 ] = ( reg->bitbuffer >> 8 );
	data[ 1 ] = ( reg->bitbuffer & 0xFF );
	uint8_t datasend[3] = {TSC1641_RegAdd_MaskAl, data[0], data[1]};
//	return HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	return HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);
}

HAL_StatusTypeDef TSC1641SetLoadOVLimit( TSC1641_NUM_T instance, double v_loadOV ){
	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		return HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
		uint16_t overVoltageDigit = ( uint16_t )( v_loadOV / V_LOAD_LSB );
		uint8_t datasend[3];
		/* Write SHUNT OV limit */
		datasend[ 0 ] = TSC1641_RegAdd_VshuntOV;
		datasend[ 1 ] = overVoltageDigit >> 8;
		datasend[ 2 ] = overVoltageDigit;
		return HAL_I2C_Master_Transmit( hi2c, tscHandle->devWriteAddress, &datasend[ 0 ], 3, 1000 );
	}
}

HAL_StatusTypeDef TSC1641SetLoadUVLimit( TSC1641_NUM_T instance, double v_loadUV ){
	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		return HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
		uint16_t underVoltageDigit = ( uint16_t )( v_loadUV / V_LOAD_LSB );
		uint8_t datasend[3];
		/* Write SHUNT OV limit */
		datasend[ 0 ] = TSC1641_RegAdd_VshuntUV;
		datasend[ 1 ] = underVoltageDigit >> 8;
		datasend[ 2 ] = underVoltageDigit;
		return HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);
	}
}

HAL_StatusTypeDef TSC1641SetTempOLimit( TSC1641_NUM_T instance, double t_over_limit ){
	if( instance >= NUM_OF_INSTANCES || TSC1641_Invailid == tsc_handles[ instance ].i2c_id ){
		return HAL_ERROR;
	}else{
		TSC1641Handle* tscHandle = &( tsc_handles[ instance ] );
		I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
		uint16_t tempOverLimitDigit = ( uint16_t )( t_over_limit / TEMP_LSB );
		uint8_t datasend[ 3 ];
		datasend[ 0 ] = TSC1641_RegAdd_TempOL;
		datasend[ 1 ] = tempOverLimitDigit >> 8;
		datasend[ 2 ] = tempOverLimitDigit;
		return HAL_I2C_Master_Transmit( hi2c, tscHandle->devWriteAddress, &datasend[ 0 ], 3, 1000 );
	}
}

static HAL_StatusTypeDef TSC1641_SetLimits_p( TSC1641Handle* tscHandle, Limit* LIMIT ){
	I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
	uint8_t datasend[ 3 ];

	/* Write SHUNT OV limit */
	datasend[0] = TSC1641_RegAdd_VshuntOV;
	datasend[1] = (LIMIT->VSHUNT_OV_LIM)>>8;
	datasend[ 2 ] = LIMIT->VSHUNT_OV_LIM;
//	HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	HAL_StatusTypeDef retval = HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write SHUNT UV limit */
	datasend[0] = TSC1641_RegAdd_VshuntUV;
	datasend[1] = (LIMIT->VSHUNT_UV_LIM)>>8;
	datasend[2] = LIMIT->VSHUNT_UV_LIM;
//	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	retval = HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write LOAD OV limit */
	datasend[0] = TSC1641_RegAdd_VloadOV;
	datasend[1] = (LIMIT->VLOAD_OV_LIM)>>8;
	datasend[2] = LIMIT->VLOAD_OV_LIM;
//	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	retval = HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write LOAD UV limit */
	datasend[0] = TSC1641_RegAdd_VloadUV;
	datasend[1] = (LIMIT->VLOAD_UV_LIM)>>8;
	datasend[2] = LIMIT->VLOAD_UV_LIM;
//	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	retval = HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write POWER OverLoad limit */
	datasend[0] = TSC1641_RegAdd_PowerOL;
	datasend[1] = (LIMIT->POWER_OV_LIM)>>8;
	datasend[2] = LIMIT->POWER_OV_LIM;
//	retval = HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	retval = HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);

	if( retval != HAL_OK ){
		return retval;
	}

	/* Write TEMP OverLoad limit */
	datasend[0] = TSC1641_RegAdd_TempOL;
	datasend[1] = (LIMIT->TEMP_OV_LIM)>>8;
	datasend[2] = LIMIT->TEMP_OV_LIM;
//	return HAL_I2C_Master_Transmit(hi2c, I2C_TSC1641_ADD_W, &datasend[0], 3, 1000);
	return HAL_I2C_Master_Transmit(hi2c, tscHandle->devWriteAddress, &datasend[0], 3, 1000);
}

static void TSC1641_GetCurrentVal_p( TSC1641Handle* tscHandle, uint8_t Data[] ){
	I2C_HandleTypeDef *hi2c = I2C_Instances[ tscHandle->i2c_id ];
	uint8_t datasend[ 1 ] = { TSC1641_RegAdd_Current };
	HAL_I2C_Master_Transmit( hi2c, tscHandle->devWriteAddress, &datasend[ 0 ], 1, 1000 );
	HAL_I2C_Master_Receive( hi2c, tscHandle->devReadAddress, &Data[ 0 ], 2, 1000 );
}

// 5.2.1 Serial bus address
bool TSC1641SetDeviceAddress( TSC1641_NUM_T inst, uint16_t addr ){
	if( inst < NUM_OF_INSTANCES ){
		uint16_t r_addr = ( ( addr << 1 ) | 0x1 );
		uint16_t w_addr = ( addr << 1 );
		TSC1641Handle* tscHandle = &( tsc_handles[ inst ] );
		tscHandle->devReadAddress = r_addr;
		tscHandle->devWriteAddress = w_addr;
		return true;
	}else{
		return false;
	}
}
