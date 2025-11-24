#include <stdio.h>

#include "global.h"
#include "dac7571.h"

#include "gpio.h"
#include "sw_lib.h"


#if 0

void dac7571_dac_start(void)
{
	DAC_SDA_L();	//PDAC_DATA_LOW;
	dac7571_dac_dly();
	dac7571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;
	dac7571_dac_dly();
	dac7571_dac_dly();
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac7571_dac_stop(void)
{
	dac7571_dac_dly();
	dac7571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;
	dac7571_dac_dly();
	dac7571_dac_dly();
	DAC_SDA_H();	//PDAC_DATA_HIGH;
	dac7571_dac_dly();
	dac7571_dac_dly();
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac7571_dac_dly(void)
{
	//Delay_us(5);
	Delay_us(20);
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac7571_dac_wr(U16 dat)
{
	dac7571_dac_start();

	dac7571_dac_byte_wr(0x98);	//DAC Write device address
	
	dac7571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac7571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac7571_dac_dly();

	dac7571_dac_byte_wr( (BYTE)((dat>>8)&0xf) );

	dac7571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac7571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac7571_dac_dly();

	dac7571_dac_byte_wr((BYTE)((dat)&0xff));

	dac7571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac7571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac7571_dac_dly();

	dac7571_dac_stop();	
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac7571_dac_byte_wr(BYTE dat)
{
BYTE cnt;

	for(cnt=0;cnt<8;cnt++)
	{
		if ( (dat<<cnt) & 0x80)		//MSB data bit check
		{
			DAC_SDA_H();	//PDAC_DATA_HIGH;		//DACIIC Data high + Delay 6usec
		}else
		{
			DAC_SDA_L();	//PDAC_DATA_LOW;		//DACIIC Data low + Delay 6usec
		}
		
		dac7571_dac_dly();
		DAC_SCL_H();		//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
		dac7571_dac_dly();
		DAC_SCL_L();		//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec		
		dac7571_dac_dly();
	}
	DAC_SDA_L();
}

#endif





void dac8571_dac_start(void)
{
	DAC_SCL_H();
	DAC_SDA_H();
	dac8571_dac_dly();
	dac8571_dac_dly();
	dac8571_dac_dly();
	dac8571_dac_dly();
	dac8571_dac_dly();
	dac8571_dac_dly();
	DAC_SDA_L();	//PDAC_DATA_LOW;
	dac8571_dac_dly();
	dac8571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;
	dac8571_dac_dly();
	dac8571_dac_dly();
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac8571_dac_stop(void)
{
	dac8571_dac_dly();
	dac8571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;
	dac8571_dac_dly();
	dac8571_dac_dly();
	DAC_SDA_H();	//PDAC_DATA_HIGH;
	dac8571_dac_dly();
	dac8571_dac_dly();
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac8571_dac_dly(void)
{
	//Delay_us(5);
	Delay_us(20);
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac8571_dac_wr(U16 dat)
{
	dac8571_dac_start();

	dac8571_dac_byte_wr(0x98);	//DAC Write device address
	
	dac8571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac8571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac8571_dac_dly();

	dac8571_dac_byte_wr( 0x10 );
	dac8571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac8571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac8571_dac_dly();

	dac8571_dac_byte_wr( (BYTE)((dat>>8)&0xff) );
	dac8571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac8571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac8571_dac_dly();

	dac8571_dac_byte_wr((BYTE)((dat)&0xff));
	dac8571_dac_dly();
	DAC_SCL_H();	//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
	dac8571_dac_dly();
	DAC_SCL_L();	//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
	dac8571_dac_dly();

	dac8571_dac_stop();
}
/*
*******************************************************************************
*
*******************************************************************************
*/
void dac8571_dac_byte_wr(BYTE dat)
{
	BYTE cnt;

	for(cnt=0;cnt<8;cnt++)
	{
		if ( (dat<<cnt) & 0x80)		//MSB data bit check
		{
			DAC_SDA_H();	//PDAC_DATA_HIGH;		//DACIIC Data high + Delay 6usec
		}else
		{
			DAC_SDA_L();	//PDAC_DATA_LOW;		//DACIIC Data low + Delay 6usec
		}
		
		dac8571_dac_dly();
		DAC_SCL_H();		//PDAC_CLOCK_HIGH;		//DACIIC Clock high + Delay 6usec
		dac8571_dac_dly();
		DAC_SCL_L();		//PDAC_CLOCK_LOW;		//DACIIC Clock low + Delay 6usec
		dac8571_dac_dly();
	}
	DAC_SDA_L();
}