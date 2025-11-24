/*
*******************************************************************************
* COPYRIGHT (c) 1992-2012 GASTRON Co.,Ltd. All rights reserved.
*******************************************************************************
* Model Name	: GTF-1100 Series
* Module name	: 
* Description	: 
* Version		: REV 0.51
* Start Date	: 2017.01.17
* Modify Date	: 2018.06.14
* C-Compiler	: avr-gcc
* MPU			: ATmega1281
* Written by	: Min-Sung, Kang (mskang@gastron.com)
*******************************************************************************
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <avr/eeprom.h>
#include <avr/pgmspace.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "dbg_printf.h"

#include "eeprom.h"
#include "modbus.h"
#include "timer.h"
#include "sw_lib.h"
#include "adc.h"
#include "uart2560.h"
#include "mmi.h"


U16	EepErrorFlag;
S16	epr_sm[EEP_MAX];
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const S16 	EEP_SYS_DEFAULT[]PROGMEM =
{
	0x1234,
	0,						// EEP_CHECKSUM
	EEPROM_MARKING,			// EEP_PREFIX
	0,						// EEP_FUNCTION_VERFI
	1,						// EEP_Device Address
	1,						// EEP_Detector Address
	157,					// EEP_4mA_Data
	802,					// EEP_20mA_Data
	1600,					// EEP_16V_Out_Data
	2400,					// EEP_24V_Out_Data
	3200,					// EEP_32V_Out_Data
	0,						// EEP_Device_Mode_Select
	0,						// EEP_Detector_BPS_Select
	0,						// EEP_T_Calibraton_FS_Mode
	0,						// EEP_LogMode
	1,						// EEP_IRSizeSet
	5500,					// EEP_T_Calibration_FERQ_1
	14710,					// EEP_T_Calibration_FERQ_2
	18950,					// EEP_T_Calibration_FERQ_3
	20730,					// EEP_T_Calibration_FERQ_4
	24570,					// EEP_T_Calibration_FERQ_5
	28350,					// EEP_T_Calibration_FERQ_6
	33455,					// EEP_T_Calibration_FERQ_7
	1						// EEP_DAC_Fullscale_Time
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EEP_Wr( U16 addr, S16 data );
U16 EEP_Rd( U16 addr );
void EEP_Load( void );
void EEP_SysFormat( void );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Initailize_Eeprom(void)
{
	EEP_Load();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void EEP_Wr( U16 addr, S16 data )
{
	U16		wadd;
	
	wadd = addr*2;
	
	eeprom_update_word( (U16*)wadd, data );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EEP_Sys_Wr( U16 addr, S16 data )
{
	epr_sm[addr] = data;
	EEP_Wr( addr, data );

	switch( addr ){
		case EEP_Detector_Address:
			Det_Address = epr_sm[addr];
			break;
		case EEP_Device_Address:
			Dvc_Address = epr_sm[addr];
			break;
		case EEP_4mA_Data:
			mA_x1 = epr_sm[addr];
			ADC_Factor();
			break;
		case EEP_20mA_Data:
			mA_x2 = epr_sm[addr];
			ADC_Factor();
			break;
		case EEP_Device_Mode_Select:
			Device_Mode_Select = epr_sm[addr];
			break;
		case EEP_Detector_BPS_Select:
			Det_BPS = epr_sm[addr];
			break;
		case EEP_T_Calibraton_FS_Mode:
			FullScaleMode = epr_sm[addr];
			break;
		case EEP_LogMode:
			LogMode_flag = epr_sm[addr];
			break;
		case EEP_IRSizeSet:
			IR_Size_SetMode = epr_sm[addr];
			break;
		case EEP_T_Calibration_FERQ_1:
			REF_DAC_DATA[1] = epr_sm[addr];
			break;
		case EEP_T_Calibration_FERQ_2:
			break;
			REF_DAC_DATA[2] = epr_sm[addr];
		case EEP_T_Calibration_FERQ_3:
			REF_DAC_DATA[3] = epr_sm[addr];
			break;
		case EEP_T_Calibration_FERQ_4:
			REF_DAC_DATA[4] = epr_sm[addr];
			break;
		case EEP_T_Calibration_FERQ_5:
			REF_DAC_DATA[5] = epr_sm[addr];
			break;
		case EEP_T_Calibration_FERQ_6:
			REF_DAC_DATA[6] = epr_sm[addr];
			break;
		case EEP_T_Calibration_FERQ_7:
			REF_DAC_DATA[7] = epr_sm[addr];
			break;
		case EEP_DAC_Fullscale_Time:
			FullScaleTime = epr_sm[addr];
			break;	
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
U16 EEP_Rd( U16 addr )
{
	U16		wadd, data;
	
	wadd = addr*2;
	
	data = eeprom_read_word( (U16*)wadd );
	
	return data;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EEP_Load( void )
{
	U16		i;
	
	if( EEP_Rd( EEP_PREFIX ) != EEPROM_MARKING ){
		EEP_SysFormat();
	}
	else {
		for(i=0; i<EEP_MAX; i++){
			epr_sm[i] = EEP_Rd(i);
			Delay_ms( 1 );
		}
	}
	Device_Mode_Select = epr_sm[EEP_Device_Mode_Select];
	Det_Address = epr_sm[EEP_Detector_Address];
	Dvc_Address = epr_sm[EEP_Device_Address];
	mA_x1 = epr_sm[EEP_4mA_Data];
	mA_x2 = epr_sm[EEP_20mA_Data];
	Det_BPS = epr_sm[EEP_Detector_BPS_Select];
	LogMode_flag = epr_sm[EEP_LogMode];
	
	FullScaleMode = epr_sm[EEP_T_Calibraton_FS_Mode];
	FullScaleTime = epr_sm[EEP_DAC_Fullscale_Time];
	IR_Size_SetMode	= epr_sm[EEP_IRSizeSet];
	
	REF_DAC_DATA[1] = epr_sm[EEP_T_Calibration_FERQ_1];
	REF_DAC_DATA[2] = epr_sm[EEP_T_Calibration_FERQ_2];
	REF_DAC_DATA[3] = epr_sm[EEP_T_Calibration_FERQ_3];
	REF_DAC_DATA[4] = epr_sm[EEP_T_Calibration_FERQ_4];
	REF_DAC_DATA[5] = epr_sm[EEP_T_Calibration_FERQ_5];
	REF_DAC_DATA[6] = epr_sm[EEP_T_Calibration_FERQ_6];
	REF_DAC_DATA[7] = epr_sm[EEP_T_Calibration_FERQ_7];
	Delay_ms( 500 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void EEP_SysFormat( void )
{
	U16		i;
	
	for( i=0; i<EEP_MAX; i++ ){
		epr_sm[i] = pgm_read_word( &EEP_SYS_DEFAULT[i] );
		EEP_Wr( i, epr_sm[i] );
	}
}

U8 EEP_CRC_Check( void )
{
	U16			wCRC;
	U16			i;
	U16			epr[DET_EEP_MAX];
	U8			*pdat;
	U8			ret = 0;
	
	for( i=DET_EEP_CHECKSUM; i<DET_EEP_EVT_BIT_IR430_1S_PTOP_MIN; i++ ){
		epr[i] = EEP_Rd(i);
	}
	
	pdat = (U8*)(&epr[DET_EEP_PREFIX_1]);
	wCRC = CRC16( pdat, (DET_EEP_EVT_BIT_IR430_1S_PTOP_MIN-DET_EEP_PREFIX_1)*2 );
	
	if( epr[DET_EEP_CHECKSUM] != wCRC ){
		ret = 1;
	}
	
	return ret;
}