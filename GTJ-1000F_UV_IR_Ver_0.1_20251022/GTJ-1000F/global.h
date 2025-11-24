#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "types.h"

#define _FREQ_CTRL
#define ATMEGA_XTAL				16000000L   		//크리스탈의 값

/*
*******************************************************************************
* SYSTEM
*******************************************************************************
*/
typedef unsigned char		BYTE;
typedef unsigned char		BIT;
typedef unsigned char		U8;
typedef char				S8;
typedef unsigned int		U16;
typedef int					S16;
typedef unsigned long		U32;
typedef long				S32;

#ifndef _NULL
#define	_NULL				0
#endif

#define _TRUE				1
#define _FALSE				0
#define _OK					1
#define _FAIL				0

#define _OFF				0
#define _ON					1

#define _LOW				0  
#define _HIGH				1  

#define ACMD_MAX			4
#define ACMD_D_MAX 			8


typedef struct a_cmd {
	U8 reg;
	U8 cmd;
	U16 dsize;
	U8 hbuf[ACMD_D_MAX];
} ACMD;


/*
*******************************************************************************
* MISCELLANEOUS
*******************************************************************************
*/
#define	BIT0				0x0001
#define	BIT1				0x0002
#define	BIT2				0x0004
#define	BIT3				0x0008
#define	BIT4				0x0010
#define	BIT5				0x0020
#define	BIT6				0x0040
#define	BIT7				0x0080

#define	IBIT0				0xFFFE	//1111 1110
#define	IBIT1				0xFFFD	//1111 1101
#define	IBIT2				0xFFFB	//1111 1011
#define	IBIT3				0xFFF7	//1111 0111
#define	IBIT4				0xFFEF	//1110 1111
#define	IBIT5				0xFFDF	//1101 1111
#define	IBIT6				0xFFBF	//1011 1111
#define	IBIT7				0xFF7F	//0111 1111

#define	BIT8				0x0100
#define	BIT9				0x0200
#define	BIT10				0x0400
#define	BIT11				0x0800
#define	BIT12				0x1000
#define	BIT13				0x2000
#define	BIT14				0x4000
#define	BIT15				0x8000

#endif	//__GLOBAL_H__

