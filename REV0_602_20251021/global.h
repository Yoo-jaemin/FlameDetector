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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define _DEBUG_BUILD

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define ATMEGA_XTAL					14745600L   		//ũ����Ż�� ��
#define ATMEGA_XTAL					16000000L   		//ũ����Ż�� ��

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef signed char				S8;
typedef unsigned char			U8;

typedef signed int				S16;
typedef unsigned int			U16;
typedef unsigned int			UNS16;

typedef unsigned int 			INT16;

typedef unsigned long  			U32;
typedef long					S32;
typedef unsigned char   		BYTE;
typedef unsigned char   		byte;

typedef unsigned char			BIT;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BIT0					0x01
#define BIT1					0x02
#define BIT2					0x04
#define BIT3					0x08
#define BIT4					0x10
#define BIT5					0x20
#define BIT6					0x40
#define BIT7					0x80

#endif
