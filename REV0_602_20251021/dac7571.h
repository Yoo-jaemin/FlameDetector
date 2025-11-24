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

#ifndef __dac7571_H__
#define __dac7571_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DAC7571_SCL_H()			(PORTE |=  BIT2)
#define DAC7571_SCL_L()			(PORTE &= ~BIT2)
#define DAC7571_SDA_H()			(PORTE |=  BIT3)
#define DAC7571_SDA_L()			(PORTE &= ~BIT3)
#define DAC7571_SDA_R()			(PINE >> 3 & 0x01)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DAC7571_REF				(5.0f)
#define DAC7571_RES				(4096)
#define DAC7571_LSB				(DAC7571_REF/DAC7571_RES)

#define XTR115_R_IN				(20000.f) // 20Kohm
#define XTR115_GAIN				(100.f)

#define DAC7571_uA_Value(uA)	(( U16 )(uA * 0.000001 / XTR115_GAIN * XTR115_R_IN / DAC7571_LSB) & (DAC7571_RES - 1))

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_DAC7571( void );
extern U8 DAC7571_uA_Wr( U16 uA );

#endif // __dac7571_H__
