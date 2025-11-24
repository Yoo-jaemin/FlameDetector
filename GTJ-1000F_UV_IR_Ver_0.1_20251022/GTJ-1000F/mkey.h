#ifndef __MKEY_H__
#define __MKEY_H__

#define _KEY_FU				0x01		//(PINE &= BIT2)	//S1 Func
#define _KEY_RST			0x10		//(PINE &= BIT3)	//S2 Reset
#define _KEY_UP				0x04		//(PINE &= BIT4)	//S3 Up
#define _KEY_DW				0x08		//(PINE &= BIT5)	//S4 Down
#define	_KEY_TEST			0x02		//(PINE &= BIT6)	//S5 Test
#define	_KEY_PWR			0x20		//(PINE &= BIT7)	//S6 Power
#define _M_KEY_PORT			(_KEY_FU | _KEY_UP | _KEY_DW | _KEY_RST | _KEY_TEST | _KEY_PWR)

#define KEY_LONG_CNT		1000
#define KEY_SHORT_CNT		50

#define DAC_CHARACTOR		4

enum {
	KEY_FU_S = 0,
	KEY_FU_L,
	KEY_UP_S,
	KEY_UP_L,
	KEY_DW_S,
	KEY_DW_L,
	KEY_RE_S,
	KEY_RE_L,
	KEY_TE_S,
	KEY_TE_L,
	KEY_PW_S,
	KEY_PW_L,
};

enum {
	line_1 = 0,
	line_2,
	line_3,
	line_4,
	line_max
};


extern U16	key_cnt;
extern U8	key_push_check;
extern U8	KeyData, KeyBuf;
extern U8	Key_long_flag;
extern U8	setting_mode_cursor;

extern U8	character_cousor;


extern U8	key_func_select_flag;
extern U8	alm_relay_chk, trb_relay_chk;
extern U8	alm_relay_operation, trb_relay_operation;
extern U16	det_ir_reponsivity_466;
extern U16	mmi_tmp[line_max];;


extern void Initialize_Mkey( void );
extern void Key_Check(void);
extern void Relay_Check(void);
extern void Key_scan(void);
extern void Key_Task(void);
extern void Func_Warm_Up_Key(void);
extern void Func_Normal_Key(void);
extern void Func_U_Calibration_Mode_Key(void);
extern void Func_T_Calibration_Mode_Key(void);

extern void Func_DAC_Control_Mode_Key(void);

extern void Func_Configiuration_Key(void);
extern void Func_Det_Configiuration_Key(void);
extern void Func_Dev_Configiuration_Key(void);
extern void Func_Modbus_Configiuration_Key(void);
extern void Func_Det_Relative_Ratio_Limit_Set_Key(void);
extern void Func_Det_Calib_Default_Set_Key(void);
extern void Func_Address_Set_Key(void);
extern void Func_BPS_Set_Key(void);
extern void Func_Dev_Ma_Cal_Set_Key(void);
extern void Func_Dev_Mode_Set_Key(void);
extern void Func_Det_Relay_Set_Key(void);
extern void Func_Det_Ir_Responsivity_Set_Key(void);
extern void Func_Det_Function_Set_Key(void);
extern void Func_Det_Test_Mode_Key(void);
extern void Func_Det_Eep_Clear_Mode_Key(void);
extern U8 Cousor_Location(U8 cnt, U8 end);
extern U16 Long_Key(U16 cnt);
extern void Reset_Key_Check(void);
extern void Normal_Return_Check(void);

extern U8 LCD_Character_Cousor_Location(U8 cnt, U8 data, U8 mode);
extern void Func_Det_Detection_Delay_Set_Key(void);
extern void Func_Dev_DAC_Set_Key(void);

#endif
