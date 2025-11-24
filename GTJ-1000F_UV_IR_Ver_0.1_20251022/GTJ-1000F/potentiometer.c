#include <stdio.h>
#include <string.h>

#include <avr/io.h>

#include "global.h"
#include "dbg_printf.h"
#include "gpio.h"
#include "sw_lib.h"

void Potentiometer_Start(void);
void Potentiometer_Stop(void);
U8 Potentiometer_AckRead( void );
U8 Potentiometer_Byte_Wr( U8 bData );
U8 Potentiometer_Wr( U8 addr, U8 data);

void Initialize_Potentiometer( void )
{
	POTENTIOMETER_SDA_H();
	POTENTIOMETER_SCL_H();
	
	//Potentiometer_Wr(0x10, 0x40);
}
void Potentiometer_Start(void)
{
	POTENTIOMETER_SDA_H();
	Delay_us(10);
	POTENTIOMETER_SCL_H();
	Delay_us(10);
	POTENTIOMETER_SDA_L();
	Delay_us(10);
	POTENTIOMETER_SCL_L();
	Delay_us(10);
}

void Potentiometer_Stop(void)
{
	POTENTIOMETER_SDA_L();
	Delay_us(10);
	POTENTIOMETER_SCL_H();
	Delay_us(10);
	POTENTIOMETER_SDA_H();
	Delay_us(10);
}

U8 Potentiometer_AckRead( void )
{
	U8		bAck;
	
	POTENTIOMETER_SCL_H();
	Delay_us( 10 );
	bAck = POTENTIOMETER_SDA_R();
	POTENTIOMETER_SCL_L();
	Delay_us( 10 );
	
	return bAck;
}

U8 Potentiometer_Byte_Wr( U8 bData )
{
	U16		i;
	
	for( i=0; i<8; i++ ){
		if( bData & 0x80 ){
			POTENTIOMETER_SDA_H();	
		}
		else {
			POTENTIOMETER_SDA_L();	
		}
		bData <<= 1;
		POTENTIOMETER_SCL_H();
		Delay_us( 10 );
		POTENTIOMETER_SCL_L();
		Delay_us( 10 );
	}
	
	return Potentiometer_AckRead();
}

U8 Potentiometer_Wr( U8 addr, U8 data)
{
	U8		bAck = 0;
	
	//DPRINT("potentionmetor\n");
	Potentiometer_Start();
	if( Potentiometer_Byte_Wr(0xA0) ){	// Slave Address
		bAck |= 1;
	}
	if( Potentiometer_Byte_Wr(addr) ){	// Register Address
		bAck |= 2;
	}
	if( Potentiometer_Byte_Wr(data) ){	// Data Byte to Register
		bAck |= 4;
	}
	Potentiometer_Stop();
	
	//DPRINT1("bAck = %d\n", bAck);
	
	return bAck;
}