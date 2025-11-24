/* *********************************************************************************
 *  GTJ-1000F.c
 *
 *	Created: 2025-09-30 
 *	Author : jmyoo
 *	Description : IR, UV integrated jig, 
 *	Newly programmed by reflecting and updating the existing firmware contents 
 
 ********************************************************************************* */ 

#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "global.h"
#include "dbg_printf.h"

#include "version.h"
#include "gpio.h"
#include "uart2560.h"
#include "timer.h"
#include "lcd.h"
#include "sw_lib.h"
#include "mmi.h"
#include "modbus.h"
#include "mkey.h"
#include "eeprom.h"
#include "adc.h"
#include "potentiometer.h"
#include "dac7571.h"

FILE Put_Device =  FDEV_SETUP_STREAM(Putchar_U0, NULL, _FDEV_SETUP_WRITE);	// Putchar_U0 = DBG		//put, get, write(OK to write) //AVR printf 용


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

int main(void)
{
	/* Replace with your application code */
	stdout =& Put_Device;		// printf 재정의 DBG 표준출력
	
	wdt_disable();
	
	Initialize_GPIO();
	Initailize_Eeprom();
	Initialize_System();
	Initialize_Timer();
	Initialize_UARTBps();
	
	//Ratio_lmt_read(1);
	Initialize_ADC();
	Initialize_LCD();
	Initialize_MODBUS();
	Initialize_Mkey();
	Initialize_Potentiometer();
	
	MODBUS_RTU_Master_Init();

	wdt_enable( WDTO_1S );
	sei();						// global interrupt enable
	
	DPRINT2("GTJ-1000F Ver%d.%02d\n", _FW_VERSION_MA, _FW_VERSION_MI);
	

	while (1) 
	{
		ADC_Task();
		Key_Task();
		
		USART1_Rx_Handler();
		
		MODBUS_RTU_Master_Task();
		MODBUS_RTU_Master_Rx_Handler();
		MODBUS_RTU_Master_Write_Handler();
		MODBUS_RTU_Master_Holding_Register_Mapping();
		
		LCD_Operating();
		LED_Indicator();
		Log_Handler();
		wdt_reset();
	}
}

