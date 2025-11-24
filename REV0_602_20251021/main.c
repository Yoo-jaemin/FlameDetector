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

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>

#include <avr/pgmspace.h>

#include "global.h"
#include "dbg_printf.h"

#include "main.h"
#include "gpio.h"
#include "mmi.h"
#include "modbus.h"
#include "timer.h"
#include "adc.h"
#include "ir.h"
#include "uv.h"
#include "uart1281.h"
#include "mmi.h"
#include "dac7571.h"
#include "eep.h"
#include "version.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DRPINT RS232 출력 관련.... 정의
FILE Put_Device =  FDEV_SETUP_STREAM(Putchar, NULL, _FDEV_SETUP_WRITE);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t mcusr_mirror __attribute__ ((section (".noinit")));

void get_mcusr(void) \
	__attribute__((naked)) \
	__attribute__((section(".init3")));

void get_mcusr(void)
{
	mcusr_mirror = MCUSR;
	MCUSR = 0;
	wdt_disable();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main( void )
{
#if 0
	U16		wCRC, i;
	U8		d[21];
#endif

	stdout =& Put_Device;	// DRPINT 재정의
	
	wdt_disable();
	
	Initialize_GPIO();
	
	EEP_Load();
	
	Initialize_System();
    Initialize_Timer();
	Initialize_UART();

	
#if 0
	d[ 0] = 1;
	d[ 1] = 6;
	d[ 2] = 0;
	d[ 3] = 3;
	d[ 4] = 0;
	d[ 5] = 1;
	wCRC = CRC16( d, 6 );
	d[ 6] = wCRC&255;
	d[ 7] = wCRC>>8&255;
	
	printf( "\n\n SAMPLE PACKET: " );
	for( i=0; i<8; i++ ){
		printf( "%02X", d[i] );
	}
#endif
	
	Initialize_ADC();
	Initialize_DAC7571();
	
	DPRINT2( "\n\n FLAME DETECTOR (GTF-1100 SERIES) F/W: REV%u.%02u\n", _FW_VERSION_MA, _FW_VERSION_MI );
	DPRINT( "\n SENSOR TYPE: " );
	if( DeviceType == DTYPE_TRIPLE ){
		DPRINT( "GTF-1100T (TRIPLE-IR)" );
	}
	else {
		DPRINT( "GTF-1100U (UV/IR)" );
	}
	
	DPRINT( "\n FLAME TYPE: " );
	if( FlameType == NON_HYDROCARBON ){
		DPRINT( "NON-HYDROCARBON" );
	}
	else {
		DPRINT( "HYDROCARBON" );
	}
	
	DPRINT( "\n ALARM LATCH: " );
	if( AlarmLatch ){
		DPRINT( "ENABLE" );
	}
	else {
		DPRINT( "DISABLE" );
	}
	
	DPRINT( "\n\n DETECTION DISTANCE: " );
	switch( DetDistance ){
		case DET_DISTANCE_18M: DPRINT( "18" ); break;
		case DET_DISTANCE_36M: DPRINT( "36" ); break;
		case DET_DISTANCE_48M: DPRINT( "48" ); break;
		case DET_DISTANCE_60M: DPRINT( "60" ); break;
	}
	DPRINT( " METER" );
	
	DPRINT( "\n\n LOGGER MODE: " );
	switch( LogMode ){
		case LMODE_SCOPE: DPRINT( "SERIAL SCOPE" ); break;
		case LMODE_NO: DPRINT( "NO" ); break;
		case LMODE_RAW: DPRINT( "RAW" ); break;
		case LMODE_CALCULATED: DPRINT( "CALCULATED" ); break;
		case LMODE_UV_ONLY: DPRINT( "UV ONLY" ); break;
	}
	
	DPRINT( "\n BIT MODE: " );
	switch( BIT_Mode ){
		case BIT_MODE_OFF: DPRINT( "OFF" ); break;
		case BIT_MODE_AUTO: DPRINT( "AUTO" ); break;
		case BIT_MODE_MANUAL: DPRINT( "MANUAL" ); break;
	}
	DPRINT( "\n\n" );
	
	Initialize_IR();
	Initialize_UV();
	
	if( DeviceType == DTYPE_UVIR ){
		DPRINT( "\n UVTRON POWER CONTROL: " );
		if( UvPwrCtrlFlag ){
			DPRINT( "ENABLE" );
		}
		else {
			DPRINT( "DISABLE" );
		}
	}
	
	DPRINT2( "\n\n MODBUS BAUD: %ld, ADDR: %u\n\n", Modbus_Baud, Modbus_Addr );
	printf( "\n" );
	
	wdt_enable( WDTO_1S );
    
	sei();
	
	while( 1 )
	{
		ADC_Task();
		Temperature_Calculation();
		Flame_Detection_Handler();
		USART1_Rx_Handler();
		ScopeMode_Log_Handler();
		wdt_reset();
	}
	
	return 0;
}
