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
#include <avr/io.h>

#include "global.h"
#include "gpio.h"
#include "main.h"
#include "sw_lib.h"

/*
*******************************************************************************
[KR]
* Description: 	mS 단위 딜레이 함수(Polling 방식)
* Arguments: 	지연될 mS 지정(unit : 1mS)
* Returns: 	없음
* Called from:  태스크
* Note(s):	없음
*******************************************************************************
*/
void Delay_ms(unsigned int time_ms)		/* time delay for ms */
{
	register unsigned int i;
	for(i = 0; i < time_ms; i++)
	{
		Delay_us(250);
		Delay_us(250);
		Delay_us(250);
		Delay_us(250);
	}
}

/*
*******************************************************************************
[KR]
* Description: 	uS 단위 딜레이 함수(Polling 방식)
* Arguments: 	지연될 uS 지정(unit : 1uS)
* Returns: 	없음
* Called from:  태스크
* Note(s):	없음
*******************************************************************************
*/
void Delay_us(unsigned char time_us)            /* time delay for us */
{
	register unsigned char i;
	for(i = 0; i < time_us; i++)		// 4 cycle +
	{
		asm volatile(" PUSH  R0 ");	// 2 cycle +
		asm volatile(" POP   R0 ");	// 2 cycle +
		asm volatile(" PUSH  R0 ");	// 2 cycle +
		asm volatile(" POP   R0 ");	// 2 cycle +
		asm volatile(" PUSH  R0 ");	// 2 cycle +
		asm volatile(" POP   R0 ");	// 2 cycle = 16 cycle = 1 us for 16MHz
	}
}

void sys_reboot(void)
{

	//while(1);
#ifdef UART_PRINT_EN
	//printf("Sys Rst\n");
#endif

	//set_reset_flag(SYSTEM_AUTO_RESET);
	asm volatile("jmp 0x0000");

}
