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

#include <avr/interrupt.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "dbg_printf.h"
#include "timer.h"
#include "uart1281.h"
#include "gpio.h"
#include "adc.h"
#include "ir.h"
#include "uv.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16		sTimerTick[ TMR_BUFF_MAX ];
U16		sTimerFlag[ TMR_BUFF_MAX ];

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_Timer( void );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_Timer( void )
{
#if 0
	TCCR0A = 0x21;
	TCCR0B = 0x01;
	OCR0B = 30;
#endif
	TCCR3A = 0x00;
	TCCR3B = 0x01; //No prescaling
	TCNT3 = TMR3_REG_VALUE; //Enable TOIE3
	
	TIMSK3 = 0x01;
	
	sTimerTick[TMR_10MS] = 10;
	sTimerTick[TMR_40MS] = 0;
	sTimerTick[TMR_ADC_TASK] = 0x8003; //32768 + 3 
	sTimerFlag[TMR_ADC_TASK] = 0;
	if( BIT_Mode == BIT_MODE_AUTO ){
		sTimerTick[TMR_BIT_PERIOD] = 0x8000+WARM_UP_TIME-1;
		sTimerFlag[TMR_BIT_PERIOD] = BIT_Period-1;
	}
	else {
		sTimerTick[TMR_BIT_PERIOD] = 0;
		sTimerFlag[TMR_BIT_PERIOD] = 0;
	}
	sTimerTick[TMR_TEMP_CALCULATION] = 0x8000+24;
	sTimerFlag[TMR_TEMP_CALCULATION] = 0;
	
	sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
	sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
	
	sTimerTick[TMR_IR_DRIFT_MAX_CHK] = 0x8000+24;
	sTimerFlag[TMR_IR_DRIFT_MAX_CHK] = 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Timer3 overflow interrupt service routine
ISR(TIMER3_OVF_vect)
{
	U16		bTn;
	
	//TMR_TP_H();
	
	TCNT3 = TMR3_REG_VALUE;
	
	USART1_Rx_Packet_Check();
	
	BIT_Drive();
	External_BIT_Scan();
	
	if( sTimerTick[TMR_ADC_TASK] & 0x8000 ){
		if( sTimerTick[TMR_ADC_TASK] == 0x8000 ){
			sTimerTick[TMR_ADC_TASK] = 0x8003; //4ms 
			sTimerFlag[TMR_ADC_TASK] = 1;
			
			if( IrRawIndex == 0 ){
				BIT_Start();
			}
			
			ADC_StartConversion;
		}
		else {
			sTimerTick[TMR_ADC_TASK]--;
		}
	}
	
	if( sTimerTick[TMR_MODBUS_TIMEOUT] & 0x8000 ){
		if( sTimerTick[TMR_MODBUS_TIMEOUT] == 0x8000 ){
			sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
			
			sTimerFlag[TMR_MODBUS_TIMEOUT] = 1;
		}
		else {
			sTimerTick[TMR_MODBUS_TIMEOUT]--;
		}
	}
	
	sTimerTick[TMR_10MS]--;
	if( sTimerTick[TMR_10MS] == 0 ){
		sTimerTick[TMR_10MS] = 10;
		
		bTn = sTimerTick[TMR_40MS] & 3;
		switch( bTn ){
			case 0:
				LED_Indicator();
				break;
				
	 		case 1:
				if( sTimerTick[TMR_STA_LED_BLINK] & 0x8000 ){
					if( sTimerTick[TMR_STA_LED_BLINK] == 0x8000 ){
						sTimerTick[TMR_STA_LED_BLINK] = 0x8000+74;
						
						sTimerFlag[TMR_STA_LED_BLINK] = 1;
					}
					else {
						if( sTimerTick[TMR_STA_LED_BLINK] == 0x8000+62 ){
							sTimerFlag[TMR_STA_LED_BLINK] = 0;
						}
						sTimerTick[TMR_STA_LED_BLINK]--;
					}
				}
				if( sTimerTick[TMR_WAR_LED_BLINK] & 0x8000 ){
					if( sTimerTick[TMR_WAR_LED_BLINK] == 0x8000 ){
						sTimerTick[TMR_WAR_LED_BLINK] = 0x8000+4;
						
						sTimerFlag[TMR_WAR_LED_BLINK] ^= 1;
					}
					else {
						sTimerTick[TMR_WAR_LED_BLINK]--;
					}
				}
	 			break;
				
			case 2:
				if( sTimerTick[TMR_FLT_LED_BLINK] & 0x8000 ){
					if( sTimerTick[TMR_FLT_LED_BLINK] == 0x8000 ){
						sTimerTick[TMR_FLT_LED_BLINK] = 0x8000+74;
					}
					else {
						sTimerTick[TMR_FLT_LED_BLINK]--;
					}
				}
				switch( AlarmState ){
					case ALARM_STATE_PWR_FAULT:
						switch( sTimerTick[TMR_FLT_LED_BLINK] ){
							case 0x8000:
								sTimerFlag[TMR_FLT_LED_BLINK] = 1;
								break;
						}
						break;
						
					case ALARM_STATE_BIT_FAULT:
						switch( sTimerTick[TMR_FLT_LED_BLINK] ){
							case 0x8000:
							case 0x800A:
								sTimerFlag[TMR_FLT_LED_BLINK] = 1;
								break;
						}
						break;
						
					case ALARM_STATE_EEP_FAULT:
						switch( sTimerTick[TMR_FLT_LED_BLINK] ){
							case 0x8000:
							case 0x800A:
							case 0x8014:
								sTimerFlag[TMR_FLT_LED_BLINK] = 1;
								break;
						}
						break;
						
					case ALARM_STATE_IR_OFFSET_FAULT:
						switch( sTimerTick[TMR_FLT_LED_BLINK] ){
							case 0x8000:
							case 0x800A:
							case 0x8014:
							case 0x801E:
								sTimerFlag[TMR_FLT_LED_BLINK] = 1;
								break;
						}
						break;
				}
				break;
	 			
	 		case 3:
				if( sTimerTick[TMR_BIT_PERIOD] & 0x8000 ){
					if( sTimerTick[TMR_BIT_PERIOD] == 0x8000 ){
						sTimerTick[TMR_BIT_PERIOD] = 0x8000+1499;
						sTimerFlag[TMR_BIT_PERIOD]++;
					}
					else {
						sTimerTick[TMR_BIT_PERIOD]--;
					}
				}
				if( sTimerTick[TMR_TEMP_CALCULATION] & 0x8000 ){
					if( sTimerTick[TMR_TEMP_CALCULATION] == 0x8000 ){
						sTimerTick[TMR_TEMP_CALCULATION] = 0x8000+24;
						sTimerFlag[TMR_TEMP_CALCULATION] = 1;
					}
					else {
						sTimerTick[TMR_TEMP_CALCULATION]--;
					}
				}
				if( sTimerTick[TMR_IR_DRIFT_MAX_CHK] & 0x8000 ){
					if( sTimerTick[TMR_IR_DRIFT_MAX_CHK] == 0x8000 ){
						sTimerTick[TMR_IR_DRIFT_MAX_CHK] = 0x8000+124;
						sTimerFlag[TMR_IR_DRIFT_MAX_CHK] = 1;
					}
					else {
						sTimerTick[TMR_IR_DRIFT_MAX_CHK]--;
					}
				}
	 			break;
		}
		sTimerTick[TMR_40MS]++;
	}
	
	//TMR_TP_L();
}
