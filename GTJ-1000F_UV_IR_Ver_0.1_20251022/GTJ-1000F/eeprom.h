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

#ifndef __EEPROM_H__
#define __EEPROM_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mmi.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EEPROM_MARKING		0xDC91
#define EEPROM_SIZE			2048

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum {
	EEP_RESERVED = 0,
	EEP_CHECKSUM,				// 1
	EEP_PREFIX,					// 2
	EEP_FUNCTION_VERFI,			// 3
	EEP_Detector_Address,		// 4
	EEP_Device_Address,			// 5
	EEP_4mA_Data,				// 6
	EEP_20mA_Data,				// 7
	EEP_16V_Out_Data,			// 8
	EEP_24V_Out_Data,			// 9
	EEP_32V_Out_Data,			// 10
	EEP_Device_Mode_Select,		// 11
	EEP_Detector_BPS_Select,	// 12
	EEP_T_Calibraton_FS_Mode,	// 13
	EEP_LogMode,				// 14
	EEP_IRSizeSet,				// 15
	EEP_T_Calibration_FERQ_1,	// 16
	EEP_T_Calibration_FERQ_2,	// 17
	EEP_T_Calibration_FERQ_3,	// 18
	EEP_T_Calibration_FERQ_4,	// 19
	EEP_T_Calibration_FERQ_5,	// 20
	EEP_T_Calibration_FERQ_6,	// 21
	EEP_T_Calibration_FERQ_7,	// 22
	EEP_DAC_Fullscale_Time,
	EEP_MAX
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern const S16 	EEP_SYS_DEFAULT[];
extern S16			epr_sm[EEP_MAX];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initailize_Eeprom(void);
extern void EEP_Wr( U16 addr, S16 data );
extern void EEP_Sys_Wr( U16 addr, S16 data );
extern void EEP_Load( void );
extern U8 EEP_CRC_Check( void );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
