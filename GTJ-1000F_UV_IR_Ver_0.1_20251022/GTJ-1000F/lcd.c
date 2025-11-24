#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "global.h"
#include "dbg_printf.h"

#include "lcd.h"
#include "version.h"
#include "sw_lib.h"
#include "uart2560.h"
#include "mkey.h"
#include "mmi.h"
#include "gpio.h"

LCD_Address_Number  LCD_Address;			//LCD display address

//U8  lcd_dis_buf[32];
//U8  lcd_baddr;
//U8 	lcdb_line[18];

//16*4;
char lcd1_buf[16];
char lcd2_buf[16];
char lcd3_buf[16];
char lcd4_buf[16];

U8 dis_blink_evt;
U8 dis_blink_sta;

void Initialize_LCD(void)
{
	dis_blink_evt=0;
	dis_blink_sta=0;
	
	memset( lcd1_buf,0,sizeof( lcd1_buf));
	memset( lcd2_buf,0,sizeof( lcd2_buf));
	memset( lcd3_buf,0,sizeof( lcd3_buf));
	memset( lcd4_buf,0,sizeof( lcd4_buf));
	
	LCD_cmd_nibble_set((U8)0x30);		//Function set
	_delay_ms(10);
	LCD_cmd_nibble_set((U8)0x30);		//Function set
	_delay_ms(10);
	LCD_cmd_nibble_set((U8)0x30);		//Function set
	_delay_ms(3);

	LCD_cmd_nibble_set((U8)0x20);		//Function set
	LCD_busy_check();					//LCD busy check svc
	LCD_cmd_byte_set((U8)0x28);			//Function set
	LCD_busy_check();					//LCD busy check svc
	LCD_cmd_byte_set((U8)0x0c);			//Display on, Cursor off
	LCD_busy_check();					//LCD busy check svc
	LCD_cmd_byte_set((U8)0x1c);			//Shift display, Shift right
	LCD_busy_check();					//LCD busy check svc
	LCD_cmd_byte_set((U8)0x06);			//Increment, Freeze display
	LCD_busy_check();					//LCD busy check svc
	LCD_cmd_byte_set((U8)0x01);			//Display clear
	LCD_busy_check();					//LCD busy check svc
	_delay_ms(5);
	
	LCD_BL_T();
}

void LCD_cmd_nibble_set(U8 LCDcommand)
{
	LCD_DataPort_Output( (U8)(LCDcommand >> 4) );		//Data High Nibble output
	_PLCD_RS_L;											//LCD RS low + Delay 5usec
	LCD_wr_latch();
}

void LCD_cmd_byte_set(U8 LCDcommand)
{
	LCD_DataPort_Output((U8)(LCDcommand >> 4));			//Data High Nibble output
	_PLCD_RS_L;
	LCD_wr_latch();

	LCD_DataPort_Output(LCDcommand);					//Data Low Nibble output
	LCD_wr_latch();
}

void LCD_dis_data(U8 Ddata)
{
	LCD_DataPort_Output((U8)(Ddata >> 4));				//Data High Nibble output
	_PLCD_RS_H;
	LCD_wr_latch();
	
	LCD_DataPort_Output(Ddata);							//Data Low Nibble output
	LCD_wr_latch();
	
	LCD_busy_check();									//LCD busy check
}

void LCD_dis_add(U8 Dadd)
{
	LCD_DataPort_Output((U8)((Dadd | 0x80) >> 4));		//Data High Nibble output
	_PLCD_RS_L;											//LCD RS low + Delay 5usec
	LCD_wr_latch();
	
	LCD_DataPort_Output(Dadd);							//Data Low Nibble output
	LCD_wr_latch();
	
	LCD_busy_check();									//LCD busy check
	LCD_busy_check();									//LCD busy check
}

void LCD_Display(U8 Dadd, char *Dstr)
{
	U8 Count=0;

	LCD_dis_add(Dadd); 									//address output
	while( (Dstr[Count] != _NULL) && (Count != 16) )
	{
		LCD_dis_data(Dstr[Count]); 						//string data output
		Count++;
	}
}

void LCD_Display_Select_Cursor(void)
{
	switch (setting_mode_cursor)
	{
		case line_1:
			sprintf(lcd1_buf,">");
			sprintf(lcd2_buf," ");
			sprintf(lcd3_buf," ");
			sprintf(lcd4_buf," ");
			break;
		case line_2:
			if (mmi_mode != FMODE_DET_EEP_CLEAR_MODE) sprintf(lcd1_buf," ");
			sprintf(lcd2_buf,">");
			sprintf(lcd3_buf," ");
			sprintf(lcd4_buf," ");
			break;
		case line_3:
			if (mmi_mode != FMODE_DET_EEP_CLEAR_MODE) sprintf(lcd1_buf," ");
			sprintf(lcd2_buf," ");
			sprintf(lcd3_buf,">");
			sprintf(lcd4_buf," ");
			break;
		case line_4:
			if (mmi_mode != FMODE_DET_EEP_CLEAR_MODE) sprintf(lcd1_buf," ");
			sprintf(lcd2_buf," ");
			sprintf(lcd3_buf," ");
			sprintf(lcd4_buf,">");
			break;
	}
	LCD_Display(LCDADD_LINE1_0, lcd1_buf);
	LCD_Display(LCDADD_LINE2_0, lcd2_buf);
	LCD_Display(LCDADD_LINE3_0, lcd3_buf);
	LCD_Display(LCDADD_LINE4_0, lcd4_buf);
}

void LCD_PartDis(U8 Dadd, U8 *Dstr, U8 size)
{
	U8 i;

	LCD_dis_add(Dadd); 									//address output
	for(i=0;i<size;i++)
	{
		LCD_dis_data(Dstr[i]); 							//string data output
	}
	
	//while( (Dstr[Count] != _NULL) && (size = 16) )
	//{
	//	LCD_dis_data(Dstr[Count]); 	//string data output
	//	Count++;
	//}
}

void LCD_busy_check(void)
{
	U8 Count;
	for (Count=0; Count < 25; Count++)
	{
		LCD_Port_Delay();
	}
}

void LCD_DataPort_Output(U8 Data)
{
	U8 sta=0;
	
	sta = PORTK & 0xf0;     						  // PORTK high bit & 0xf0
	sta |= ( Data & (0x0f) ); 						  // low bit high
	PORTK = sta;
}

void LCD_Port_Delay(void)
{
	_delay_us(5);									 // 5uS Delay
}

void LCD_wr_latch(void)
{
	LCD_Port_Delay();
	_PLCD_WE_L;
	LCD_Port_Delay();
	_PLCD_ENABLE_H;									//LCD Enable high + Delay 5usec
	LCD_Port_Delay();
	LCD_Port_Delay();
	_PLCD_ENABLE_L;									//LCD Enable low + Delay 5usec
	LCD_Port_Delay();
	_PLCD_WE_H;
}

void LCD_Clear_Line(U8 Line)
{
	U8 i = 0;

	LCD_dis_add(Line); 								//address output
	for(i = 0 ; i < 16 ; i++)
	{
		LCD_dis_data(' '); 							//string data output
	}
}

void LCD_Clear_Point(U8 Line, U8 Point)
{
	U8 i;

	LCD_dis_add(Line); 								//address output
	for(i = 0; i < Point ; i++)
	{
		LCD_dis_data(' '); 							//string data output
	}
}

void LCD_Clear_Line_All(void)
{
	LCD_cmd_byte_set((U8)0x01);
	//LCD_Clear_Line(LCD_LINE1);
	//LCD_Clear_Line(LCD_LINE2);
	//LCD_Clear_Line(LCD_LINE3);
	//LCD_Clear_Line(LCD_LINE4);
	LCD_busy_check();			//LCD busy check svc
	_delay_ms(5);
}
