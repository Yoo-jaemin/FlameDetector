#ifndef __LCD_H__
#define __LCD_H__

#define	_PLCD_ENABLE_H	(PORTK |= BIT5);
#define	_PLCD_ENABLE_L	(PORTK &= IBIT5);

#define	_PLCD_RS_H		(PORTK |= BIT6);
#define	_PLCD_RS_L		(PORTK &= IBIT6);

#define	_PLCD_WE_H		(PORTK |= BIT4)	;
#define	_PLCD_WE_L		(PORTK &= IBIT4);

//Back Light is always High(Pull Up)
//#define _PLCD_BK_H		(PORTK |= BIT7);
//#define _PLCD_BK_L		(PORTK &= IBIT7);

#define LCD_LINE1			LCDADD_LINE1_0
#define LCD_LINE2			LCDADD_LINE2_0
#define LCD_LINE3			LCDADD_LINE3_0
#define LCD_LINE4			LCDADD_LINE4_0


typedef enum {
	LCDADD_LINE1_0 = 0x00,	//LCD Address Line1 - 0
	LCDADD_LINE1_1,			//LCD Address Line1 - 1
	LCDADD_LINE1_2,			//LCD Address Line1 - 2
	LCDADD_LINE1_3,			//LCD Address Line1 - 3
	LCDADD_LINE1_4,			//LCD Address Line1 - 4
	LCDADD_LINE1_5,			//LCD Address Line1 - 5
	LCDADD_LINE1_6,			//LCD Address Line1 - 6
	LCDADD_LINE1_7,			//LCD Address Line1 - 7
	LCDADD_LINE1_8,			//LCD Address Line1 - 8
	LCDADD_LINE1_9,			//LCD Address Line1 - 9
	LCDADD_LINE1_10,		//LCD Address Line1 - 10
	LCDADD_LINE1_11,		//LCD Address Line1 - 11
	LCDADD_LINE1_12,		//LCD Address Line1 - 12
	LCDADD_LINE1_13,		//LCD Address Line1 - 13
	LCDADD_LINE1_14,		//LCD Address Line1 - 14
	LCDADD_LINE1_15,		//LCD Address Line1 - 15

	LCDADD_LINE3_0 = 0x10,	//LCD Address Line3 - 0
	LCDADD_LINE3_1,			//LCD Address Line3 - 1
	LCDADD_LINE3_2,			//LCD Address Line3 - 2
	LCDADD_LINE3_3,			//LCD Address Line3 - 3
	LCDADD_LINE3_4,			//LCD Address Line3 - 4
	LCDADD_LINE3_5,			//LCD Address Line3 - 5
	LCDADD_LINE3_6,			//LCD Address Line3 - 6
	LCDADD_LINE3_7,			//LCD Address Line3 - 7
	LCDADD_LINE3_8,			//LCD Address Line3 - 8
	LCDADD_LINE3_9,			//LCD Address Line3 - 9
	LCDADD_LINE3_10,		//LCD Address Line3 - 10
	LCDADD_LINE3_11,		//LCD Address Line3 - 11
	LCDADD_LINE3_12,		//LCD Address Line3 - 12
	LCDADD_LINE3_13,		//LCD Address Line3 - 13
	LCDADD_LINE3_14,		//LCD Address Line3 - 14
	LCDADD_LINE3_15,		//LCD Address Line3 - 15
	
	LCDADD_LINE2_0 = 0x40,	//LCD Address Line2 - 0
	LCDADD_LINE2_1,			//LCD Address Line2 - 1
	LCDADD_LINE2_2,			//LCD Address Line2 - 2
	LCDADD_LINE2_3,			//LCD Address Line2 - 3
	LCDADD_LINE2_4,			//LCD Address Line2 - 4
	LCDADD_LINE2_5,			//LCD Address Line2 - 5
	LCDADD_LINE2_6,			//LCD Address Line2 - 6
	LCDADD_LINE2_7,			//LCD Address Line2 - 7
	LCDADD_LINE2_8,			//LCD Address Line2 - 8
	LCDADD_LINE2_9,			//LCD Address Line2 - 9
	LCDADD_LINE2_10,		//LCD Address Line2 - 10
	LCDADD_LINE2_11,		//LCD Address Line2 - 11
	LCDADD_LINE2_12,		//LCD Address Line2 - 12
	LCDADD_LINE2_13,		//LCD Address Line2 - 13
	LCDADD_LINE2_14,		//LCD Address Line2 - 14
	LCDADD_LINE2_15,		//LCD Address Line2 - 15

	LCDADD_LINE4_0 = 0x50,	//LCD Address Line4 - 0
	LCDADD_LINE4_1,			//LCD Address Line4 - 1
	LCDADD_LINE4_2,			//LCD Address Line4 - 2
	LCDADD_LINE4_3,			//LCD Address Line4 - 3
	LCDADD_LINE4_4,			//LCD Address Line4 - 4
	LCDADD_LINE4_5,			//LCD Address Line4 - 5
	LCDADD_LINE4_6,			//LCD Address Line4 - 6
	LCDADD_LINE4_7,			//LCD Address Line4 - 7
	LCDADD_LINE4_8,			//LCD Address Line4 - 8
	LCDADD_LINE4_9,			//LCD Address Line4 - 9
	LCDADD_LINE4_10,		//LCD Address Line4 - 10
	LCDADD_LINE4_11,		//LCD Address Line4 - 11
	LCDADD_LINE4_12,		//LCD Address Line4 - 12
	LCDADD_LINE4_13,		//LCD Address Line4 - 13
	LCDADD_LINE4_14,		//LCD Address Line4 - 14
	LCDADD_LINE4_15			//LCD Address Line4 - 15

}LCD_Address_Number;


extern char lcd1_buf[16];
extern char lcd2_buf[16];
extern char lcd3_buf[16];
extern char lcd4_buf[16];


extern U8 dis_blink_evt;
extern U8 dis_blink_sta;

void Initialize_LCD(void);
void LCD_cmd_nibble_set(U8 LCDcommand);
void LCD_cmd_byte_set(U8 LCDcommand);
void LCD_dis_data(U8 Ddata);
void LCD_dis_add(U8 Dadd);
void LCD_Display(U8 Dadd, char *Dstr);
void LCD_Display_Select_Cursor(void);
void LCD_busy_check(void);
void LCD_DataPort_Output(U8 Data);
void LCD_Port_Delay(void);
void LCD_wr_latch(void);

void LCD_Clear_Line(U8 Line);
void LCD_Clear_Point(U8 Line, U8 Point);
void LCD_Clear_Line_All(void);
void LCD_PartDis(U8 Dadd, U8 *Dstr, U8 size);

void DBG_LCD(char cmd);


#endif