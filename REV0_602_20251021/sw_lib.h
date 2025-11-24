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

/* Required to safely shift negative values */
/*
#define shift_right(x, s) ({    \
        __typeof__(x) __x = (x);        \
        __typeof__(s) __s = (s);        \
        __x < 0 ? -(-__x >> __s) : __x >> __s;  \
})
*/

#ifndef __SW_LIB_H__
#define __SW_LIB_H__

void Delay_us(unsigned char time_us);            /* time delay for us */
void Delay_ms(unsigned int time_ms);       	/* time delay for ms */
void sys_reboot(void);

#endif	//__SW_LIB_H__

