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

#ifndef __adc_H__
#define __adc_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define AD_BUF_SIZE				4

#define ADC_4_5V				(4.5/ADC_LSB)
#define ADC_4_0V				(4.0/ADC_LSB)
#define ADC_3_5V				(3.5/ADC_LSB)
#define ADC_3_0V				(3.0/ADC_LSB)
#define ADC_2_5V				(2.5/ADC_LSB)
#define ADC_2_0V				(2.0/ADC_LSB)
#define ADC_1_5V				(1.5/ADC_LSB)
#define ADC_1_0V				(1.0/ADC_LSB)
#define ADC_0_5V				(0.5/ADC_LSB)

//#define ADC_REF					(5.f)
#define ADC_REF					(2.56)
#define ADC_RES					(1024)
#define ADC_LSB					(ADC_REF/ADC_RES)

#define ADC_PUV_SEN_FACTOR		((2.2+0.030)/0.030*ADC_LSB)
//#define ADC_P24_SEN_FACTOR		((30.f+3.4)/3.4*ADC_LSB)
//#define ADC_POUT_FACTOR			((30.f+3.4)/3.4*ADC_LSB)
#define ADC_P24_SEN_FACTOR		((30.f+2.2)/2.2*ADC_LSB)
#define ADC_POUT_FACTOR			((30.f+2.2)/2.2*ADC_LSB)
#define ADC_DAC_OUT_FACTOR		((1+1)/1*ADC_LSB)
#define ADC_ANALOG_A_FACTOR		(2000-400) / (mA_x2 - mA_x1)
#define ADC_ANALOG_B_FACTOR		((2000*mA_x1) - (400*mA_x2))/(mA_x1-mA_x2)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ADC_Disable				ADCSRA &= 0x7F
#define ADC_Enable				ADCSRA |= 0x80
#define ADC_StartConversion		ADCSRA |= 0x40

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern double	fADBuf[4], PUV, P24, Tempareture, DAC_Out;
extern U16		ADbuf[AD_BUF_SIZE];
extern float	mA_x1,mA_x2;
extern U16		Det_Analog_Data;
extern double	P24, POUT;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_ADC( void );
extern void ADC_Task( void );
extern void ADC_Factor(void);

#endif // __adc_H__
