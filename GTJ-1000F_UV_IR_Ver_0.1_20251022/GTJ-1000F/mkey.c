/* *********************************************************************************
 *  GTJ-1000F.c
 *
 *	Created: 2025-09-30 
 *	Author : jmyoo
  
 ********************************************************************************* */ 

#include <stdio.h>
#include <avr/io.h>

#include "global.h"
#include "dbg_printf.h"

#include "mkey.h"
#include "gpio.h"
#include "mmi.h"
#include "timer.h"
#include "uart2560.h"
#include "modbus.h"
#include "eeprom.h"
#include "adc.h"
#include "lcd.h"
#include "potentiometer.h"
#include "sw_lib.h"
#include "dac7571.h"


U16	key_cnt;
U8	key_push_check;
U8	KeyData, KeyBuf, keyScanBuf;
U8	Key_long_flag;
U8	setting_mode_cursor;
U8	character_cousor;

U8	key_func_select_flag;
U8	alm_relay_chk, trb_relay_chk;
U8	alm_relay_operation, trb_relay_operation;
U8	relay_chk_cnt;
U16	det_ir_reponsivity_466;
U16	mmi_tmp[line_max];


void Initialize_Mkey( void )
{
	key_cnt = 0;
	key_push_check = 0;
	KeyData = 0xff;
	Key_long_flag = 0;
	setting_mode_cursor = 0;
	key_func_select_flag = 0;
	
	alm_relay_chk = 0;
	trb_relay_chk = 0;
	relay_chk_cnt = 0;
}

void Key_Check(void)
{
	U8 	dat, cop;

	dat = (PINE >> 2) & 0x3F;
	cop = dat ^ 0x3f;
	
	if (cop != 0)	// key on
	{ 
		if (key_push_check == 0) {
			key_cnt++;	
 			if (key_cnt > KEY_LONG_CNT) {
				key_cnt = KEY_LONG_CNT;
				KeyBuf = keyScanBuf;
				Key_long_flag = 2; // Key_Pushed_Flag
				key_push_check = 1;
			}
			keyScanBuf = cop;
		}
	} 
	else 
	{
		if (key_cnt == 0) {
			Key_long_flag = 0;
			key_push_check = 0;
		}
		else if ((key_cnt < KEY_LONG_CNT) && (key_cnt > KEY_SHORT_CNT)) {
			KeyBuf = keyScanBuf;
			Key_long_flag = 1;
			key_push_check = 1;
		}
		key_cnt = 0;
	}
	Key_scan();
}

void Relay_Check(void)
{
	U8 	dat, cop;
	
	dat = (PINJ >> 3) & 0x0F;
	cop = dat ^ 0x0F;
	
	// PJ4, PJ3
	if ( (cop & 0x03) == 0x03 )			alm_relay_chk = 0;	// PJ4 = 0, PJ3 = 0 -> 
	else								alm_relay_chk = 1;
	
	// PJ6, PJ5
	if ( ((cop >> 2) & 0x03) == 0x03 )	trb_relay_chk = 0;  
	else								trb_relay_chk = 1;
}

void Key_scan(void)
{	
	if (mmi_mode == FMODE_WARM_UP) {
		KeyBuf = 0xff;
		return;
	}
	
	if (Key_long_flag == 1)
	{
		switch (KeyBuf)		// KeyBuf-> key check
		{  
			case _KEY_FU:	KeyData = KEY_FU_S;		break;
			case _KEY_UP:	KeyData = KEY_UP_S;		break;
			case _KEY_DW:	KeyData = KEY_DW_S;		break;
			case _KEY_RST:	KeyData = KEY_RE_S;		break;
			case _KEY_TEST:	KeyData = KEY_TE_S;		break;
			case _KEY_PWR:  KeyData = KEY_PW_S;
				
				if (!(det_power_control & Det_Power_Control_Bit)) // Power on
				{
					det_power_control = Det_Power_Control_Bit;
					mmi_mode = FMODE_DET_SYSTEM_CHECK;
					P24V_OUT_ON();
					PWR_LED_ON();
				}
				else	// Power off
				{
					Mode_Select();
					det_power_control = 0;
					det_power_out = 0;
					Det_Power_Task();
					Calibration_Mode_Flow = 0;
					DAC_Mode_Flow = 0;
					sTimerFlag[TMR_DET_SYSTEM_CHECK] = 0;
					Det_System_Fault_Flag = 0;
					
					dac_data = 0;
					DAC_Voltage = 0;
					dac8571_dac_wr(0x0000);
					if (FullScaleMode)	
						FullScaleMode = 0;
				} break;
		} 
	}
	else if (Key_long_flag > 1)
	{
		switch (KeyBuf)
		{
			case _KEY_FU:	KeyData = KEY_FU_L;		break;
			case _KEY_UP:	KeyData = KEY_UP_L;		break;
			case _KEY_DW:	KeyData = KEY_DW_L;		break;
			case _KEY_RST:	KeyData = KEY_RE_L;		break;
			case _KEY_TEST:	KeyData = KEY_TE_L;		break;
			case _KEY_PWR:	KeyData = KEY_PW_L;		break;	
		}
	}
	KeyBuf = 0;
}

void Key_Task(void)
{
	switch (mmi_mode) 
	{
		case FMODE_WARM_UP:							Func_Warm_Up_Key();							break;
		case FMODE_NORMAL:							Func_Normal_Key();							break;
		case FMODE_U_CALIBRATION_MODE:				Func_U_Calibration_Mode_Key();				break;
		case FMODE_T_CALIBRATION_MODE:				Func_T_Calibration_Mode_Key();				break;
		case FMODE_CONFIGIURATION:					Func_Configiuration_Key();					break;
		case FMODE_DET_CONFIGIURATION:				Func_Det_Configiuration_Key();				break;
		case FMODE_DEV_CONFIGIURATION:				Func_Dev_Configiuration_Key();				break;
		case FMODE_MODBUS_CONFIGIURATION:			Func_Modbus_Configiuration_Key();			break;
		case FMODE_DET_CALIB_DEFAULT_SET:			Func_Det_Calib_Default_Set_Key();			break;
		case FMODE_DET_TRIPLE_REL_RATIO_LIMIT_SET:	Func_Det_Relative_Ratio_Limit_Set_Key();	break;	
		case FMODE_MODBUS_ADDRESS_SET:				Func_Address_Set_Key();						break;
		case FMODE_MODBUS_BPS_SET:					Func_BPS_Set_Key();							break;
		case FMODE_DEV_MA_CAL_SET:					Func_Dev_Ma_Cal_Set_Key();					break;
		case FMODE_DEV_MODE_SET:					Func_Dev_Mode_Set_Key();					break;
		case FMODE_DET_IR_RESPONSIVITY_SET:			Func_Det_Ir_Responsivity_Set_Key();			break;
		case FMODE_DET_RELAY_SET:					Func_Det_Relay_Set_Key();					break;
		case FMODE_DET_FUNCTION_SET:				Func_Det_Function_Set_Key();				break;
		case FMODE_DET_DETECTION_DELAY_SET:			Func_Det_Detection_Delay_Set_Key();			break;
		case FMODE_DET_TEST_MODE:					Func_Det_Test_Mode_Key();					break;
		case FMODE_DET_EEP_CLEAR_MODE:				Func_Det_Eep_Clear_Mode_Key();				break;
		case FMODE_DAC_CONTROL_MODE:				Func_DAC_Control_Mode_Key();				break;
		case FMODE_DEV_DAC_CONFIGURATION:			Func_Dev_DAC_Set_Key();						break;
	}
	Normal_Return_Check();
	KeyData = 0xff;
}

void Func_Warm_Up_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:	break;
		case KEY_UP_S:	break;
		case KEY_DW_S:	break;
		case KEY_RE_S:
			Mode_Select();
			sTimerTick[TMR_SELFTEST] = 0;
			break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Normal_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:	break;
		case KEY_UP_S:	break;
		case KEY_DW_S:
			if (det_power_control & Det_Power_Control_Bit) {
				det_power_out++;
				if (det_power_out == Det_Power_MAX)		
					det_power_out = Det_Power_24V;
				Det_Power_Task();
			} break;
		case KEY_RE_S:	break;
		case KEY_TE_S:	// BIT 체크
			mmi_mode = FMODE_DET_TEST_MODE;
			ModeSetFlow[FMODE_DET_TEST_MODE] = 1;
			break;
		case KEY_FU_L:	mmi_mode = FMODE_CONFIGIURATION;		break;
		case KEY_UP_L:	
			mmi_mode = FMODE_DET_TRIPLE_REL_RATIO_LIMIT_SET;
			//mmi_mode = FMODE_DET_MA_CAL_SET;
			//ModeSetFlow[FMODE_DET_MA_CAL_SET] = 1;
			break;
		case KEY_DW_L:	mmi_mode = FMODE_DET_CALIB_DEFAULT_SET;	break;
		case KEY_RE_L:	
			mmi_mode = FMODE_DET_EEP_CLEAR_MODE;
			ModeSetFlow[FMODE_DET_EEP_CLEAR_MODE] = 1;
			break;
		case KEY_TE_L:
			if (!LogMode_flag)	LogMode_flag = 1;
			else				LogMode_flag = 0;
			EEP_Sys_Wr(EEP_LogMode, LogMode_flag);
			break;
	}
}

void Func_U_Calibration_Mode_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:
			if (Calibration_Mode_Flow == 1)
				Calibration_Mode_Flow = 2;
			break;
		case KEY_RE_S:	Calibration_Mode_Flow = 0;			break;
		case KEY_UP_S:
		case KEY_DW_S:
		case KEY_TE_S:	break;
		case KEY_FU_L:	mmi_mode = FMODE_CONFIGIURATION;	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_T_Calibration_Mode_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:
			if (Det_Input_Reg_Data[DET_InputReg_DeviceType] == STATUS_UVIR) {
				if (blackbody_freq <= FREQ_CTRL_MIN_U) {
					Calibration_Mode_Flow = CALIB_START;
					dac_flag = 7;
					EXT_BIT_OFF();
					Calibration_Mode_Count = Det_BlackBody_Delay_Count;
				}
			}
			else {
				if (blackbody_freq <= FREQ_CTRL_MIN_T) {
					Calibration_Mode_Flow = CALIB_START;
					dac_flag = 1;
					EXT_BIT_OFF();
					Calibration_Mode_Count = Det_BlackBody_Delay_Count;
				}
			} break;
		case KEY_RE_S:
		{
			Calibration_Mode_Flow = 0;
			blackbody_freq = 0;
			dac_flag = 0;
			dac8571_dac_wr(dac_data);
		} break;
		
		case KEY_UP_S:
		case KEY_DW_S:	
			ERROR_cousor = LCD_Character_Cousor_Location(41, ERROR_cousor, Det_T_CALIBRATION_MODE);
			break;
			
		case KEY_TE_S:	break;
		case KEY_FU_L:	mmi_mode = FMODE_CONFIGIURATION;	break;
		case KEY_UP_L:
		case KEY_DW_L:
			if (FullScaleMode) {
				FullScaleMode = 0;
				EEP_Sys_Wr(EEP_T_Calibraton_FS_Mode, FullScaleMode);
			}
			else {
				FullScaleMode = 1;
				EEP_Sys_Wr(EEP_T_Calibraton_FS_Mode, FullScaleMode);
			} break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_DAC_Control_Mode_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:	
		case KEY_RE_S:	LCD_Character_Cousor_Location(DAC_CHARACTOR, 0, Det_DAC_CONTROL_MODE);	break;
		
		case KEY_UP_S:
		case KEY_DW_S:
			if (DAC_Mode_Flow == 1) {
				switch (character_cousor)
				{
					case 0:		Dev_DAC_Value[0] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Value[0],Det_DAC_CONTROL_MODE);		break;
					case 1:		Dev_DAC_Value[1] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Value[1],Det_DAC_CONTROL_MODE);		break;
					case 2:		Dev_DAC_Value[2] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Value[2],Det_DAC_CONTROL_MODE);		break;
					case 3:		Dev_DAC_Value[3] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Value[3],Det_DAC_CONTROL_MODE);		break;
					case 4:		Dev_DAC_Value[4] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Value[4],Det_DAC_CONTROL_MODE);		break;
				}
			}
			else if (DAC_Mode_Flow == 2) {
				switch (character_cousor)
				{
					case 0:		Dev_DAC_Voltage[0] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Voltage[0],Det_DAC_CONTROL_MODE);	break;
					case 1:		Dev_DAC_Voltage[1] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Voltage[1],Det_DAC_CONTROL_MODE);	break;
					case 2:		Dev_DAC_Voltage[2] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Voltage[2],Det_DAC_CONTROL_MODE);	break;
					case 3:		Dev_DAC_Voltage[3] = LCD_Character_Cousor_Location(DAC_CHARACTOR, Dev_DAC_Voltage[3],Det_DAC_CONTROL_MODE);	break;
				}
			}
			else if (DAC_Mode_Flow == 3)
				DAC_Mode_cousor = LCD_Character_Cousor_Location(1, DAC_Mode_cousor,Det_DAC_CONTROL_MODE);

			else if (DAC_Mode_Flow == 0)	// DAC 메뉴 선택
				DAC_Mode_cousor = LCD_Character_Cousor_Location(2, DAC_Mode_cousor,Det_DAC_CONTROL_MODE);
				break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	mmi_mode = FMODE_CONFIGIURATION;		break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	mmi_mode = FMODE_DEV_DAC_CONFIGURATION;	break;
	}
}

void Func_Configiuration_Key(void) // Func long key on ~~
{
	Cousor_Location(line_1, line_3);
		
	switch (KeyData)
	{
		case KEY_FU_S:
			switch (setting_mode_cursor)
			{
				case line_1:	mmi_mode = FMODE_DET_CONFIGIURATION;			 break;
				case line_2:	mmi_mode = FMODE_DEV_CONFIGIURATION;			 break;
				case line_3:	mmi_mode = FMODE_MODBUS_CONFIGIURATION;			 break;
			}
			setting_mode_cursor = 0;
			break;
		case KEY_UP_S:
		case KEY_DW_S:	break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Det_Configiuration_Key(void)
{
	Cousor_Location(line_1, line_4);
		
	switch (KeyData)
	{
		case KEY_FU_S:
			switch (setting_mode_cursor)
			{
				case line_1:
					mmi_mode = FMODE_DET_IR_RESPONSIVITY_SET;
					mmi_tmp[line_1] = det_ir_reponsivity_466;
					break;
				case line_2:
					mmi_mode = FMODE_DET_RELAY_SET;
					mmi_tmp[line_1] = alm_relay_operation;
					mmi_tmp[line_2] = trb_relay_operation;
					break;
				case line_3:
					mmi_mode = FMODE_DET_FUNCTION_SET;
					mmi_tmp[line_1] = bit_operation;
					mmi_tmp[line_2] = det_pwr_ctr_value;
					mmi_tmp[line_3] = det_log_mode_value;
					mmi_tmp[line_4] = FlameType;
					break;
				case line_4:
					mmi_mode = FMODE_DET_DETECTION_DELAY_SET; 
					mmi_tmp[line_1] = FlameDetDelay_Sec;
					break;
			}
			setting_mode_cursor = 0;
			break;
		case KEY_UP_S:
		case KEY_DW_S:
			break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Det_Detection_Delay_Set_Key(void)
{
	Cousor_Location(line_1, line_1);
	if (Det_Input_Reg_Data[DET_InputReg_FW_SUB_VERSION] != FW_SUB_VER)
	{
		switch (KeyData)
		{
			case KEY_FU_S:
			if (key_func_select_flag == 1) {
				key_func_select_flag = 0;
				Det_Write_Single_Func(DET_EEP_FLAME_DETECTION_DELAY_TIME, mmi_tmp[line_1]);
				DPRINT1(" FlameDetDelay Wr: %d\r\n", mmi_tmp[line_1]);
				} else {
				key_func_select_flag = 1;
				mmi_tmp[line_1] = FlameDetDelay_Sec;
				DPRINT1(" FlameDetDelay: %d\r\n", mmi_tmp[line_1]);
			} break;
			
			case KEY_UP_S:
			case KEY_DW_S:
			if (key_func_select_flag) {
				if (KeyData == KEY_UP_S) {
					if (mmi_tmp[line_1] == 20)	mmi_tmp[line_1] = 0;
					else						mmi_tmp[line_1]++;
					} else {
					if (mmi_tmp[line_1] == 0)	mmi_tmp[line_1] = 20;
					else						mmi_tmp[line_1]--;
				}
			} break;
			case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();
			break;
			case KEY_UP_L:
			case KEY_DW_L:
			if (key_func_select_flag) {
				mmi_tmp[line_1] = Long_Key(mmi_tmp[line_1]);
				
				if (mmi_tmp[line_1] >= 20)		mmi_tmp[line_1] = 0;
				else if (mmi_tmp[line_1] <= 0)	mmi_tmp[line_1] = 20;
			} break;
		}
	} 
	else 
	{
		switch (KeyData)
		{
			case KEY_RE_S: Reset_Key_Check(); break;
		}
	} 	
}

void Func_Dev_Configiuration_Key(void)
{
	Cousor_Location(line_1, line_2); 
		
	switch (KeyData)
	{
		case KEY_FU_S:
			switch (setting_mode_cursor)
			{
				case line_1:
					mmi_mode = FMODE_DEV_MA_CAL_SET;
					break;
				case line_2:
					mmi_mode = FMODE_DEV_MODE_SET;
					mmi_tmp[line_1] = Device_Mode_Select;
					break;	
			}
			setting_mode_cursor = 0;
			break;
		case KEY_UP_S:
		case KEY_DW_S:	
			break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Modbus_Configiuration_Key(void)
{
	Cousor_Location(line_1, line_2);
		
	switch (KeyData)
	{
		case KEY_FU_S:
			switch (setting_mode_cursor) 
			{
				case line_1:
					mmi_mode = FMODE_MODBUS_ADDRESS_SET;
					mmi_tmp[line_1] = Det_Address;
					mmi_tmp[line_2] = Dvc_Address;
					break;
				case line_2:
					mmi_mode = FMODE_MODBUS_BPS_SET;
					mmi_tmp[line_1] = Det_BPS;
					break;
			}
			setting_mode_cursor = 0;
			break;
		case KEY_UP_S:  break;
		case KEY_DW_S:	break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Det_Relative_Ratio_Limit_Set_Key(void)
{
	Cousor_Location(line_2, line_2);
	
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag == 1) {
				key_func_select_flag = 0;
				
				Det_Write_Multi_Func(TRIPLE_IR_REL_RATIO_395_SIZE_1, TRIPLE_IR_REL_RATIO_530_SIZE_999);
				DPRINT1("\r\n ****** Det_Relative_Ratio_Limit_Set : Det%d ******\r\n ", mmi_tmp[line_2]);
			} else {
				key_func_select_flag = 1;
				//mmi_tmp[line_2] = 1;
			} break;
		case KEY_UP_S:
		case KEY_DW_S:
			if (key_func_select_flag) {
				if (KeyData == KEY_UP_S) {
					if (mmi_tmp[line_2] == DET_SIZE)	mmi_tmp[line_2] = 1;
					else								mmi_tmp[line_2]++;
				} else {
					if (mmi_tmp[line_2] <= 1)	mmi_tmp[line_2] = DET_SIZE;
					else						mmi_tmp[line_2]--;
				}
			} break;
		case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();
			break;	
		case KEY_UP_L:
		case KEY_DW_L:	
			if (key_func_select_flag) {
				mmi_tmp[line_2] = Long_Key(mmi_tmp[line_2]);
				
				if (mmi_tmp[line_2] >= DET_SIZE+1)		mmi_tmp[line_2] = 1;
				else if (mmi_tmp[line_2] <= 0)			mmi_tmp[line_2] = DET_SIZE;
			} break;		
	}// end switch
}

void Func_Det_Calib_Default_Set_Key(void)
{
	Cousor_Location(line_2, line_2);
	
	switch (KeyData)
	{
		case KEY_FU_S:
			Det_Modbus_Buf[Det_Comm_Flow] = Commnication_Default_Set;
			break;
		case KEY_UP_S:  break;
		case KEY_DW_S:	break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L: 
		case KEY_UP_L:
		case KEY_DW_L:
		case KEY_RE_L:
		case KEY_TE_L:	break;
	}
}

void Func_Address_Set_Key(void)
{
	Cousor_Location(line_1, line_2);
		
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag)
			{
				key_func_select_flag = 0;
				if (setting_mode_cursor == line_1)		EEP_Sys_Wr(EEP_Detector_Address, mmi_tmp[line_1]);
				else if(setting_mode_cursor == line_2)	EEP_Sys_Wr(EEP_Device_Address, mmi_tmp[line_2]);
			}
			else	// select
			{
				key_func_select_flag = 1;
				mmi_tmp[line_1] = Det_Address;
				mmi_tmp[line_2] = Dvc_Address;
			} break;
		case KEY_UP_S:
		case KEY_DW_S:
			if (!key_func_select_flag) {
				//Cousor_Location(line_1, line_2);
			}
			else {
				if (setting_mode_cursor == line_1)
				{
					mmi_tmp[line_1] = Long_Key(mmi_tmp[line_1]);
				
					if (mmi_tmp[line_1] > 247)		mmi_tmp[line_1] = 247;
					else if (mmi_tmp[line_1] < 1)	mmi_tmp[line_1] = 1;
				}
				else if (setting_mode_cursor == line_2)
				{
					mmi_tmp[line_2] = Long_Key(mmi_tmp[line_2]);
				
					if (mmi_tmp[line_2] > 247)		mmi_tmp[line_2] = 247;
					else if (mmi_tmp[line_2] < 1)	mmi_tmp[line_2] = 1;
				}
			} break;
		case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();
			break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:
		case KEY_DW_L:
			if (!key_func_select_flag) {
				//Cousor_Location(line_1, line_2);
			}
			else {
				if (setting_mode_cursor == line_1) 
				{
					mmi_tmp[line_1] = Long_Key(mmi_tmp[line_1]);
				
					if (mmi_tmp[line_1] > 245)		mmi_tmp[line_1] = 245;
					else if (mmi_tmp[line_1] < 1)	mmi_tmp[line_1] = 1;
				}
				else if (setting_mode_cursor == line_2)
				{
					mmi_tmp[line_2] = Long_Key(mmi_tmp[line_2]);
				
					if (mmi_tmp[line_2] > 245)		mmi_tmp[line_2] = 245;
					else if (mmi_tmp[line_2] < 1)	mmi_tmp[line_2] = 1;
				}
			} break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_BPS_Set_Key(void)
{
	Cousor_Location(line_1, line_1);
		
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag == 1) {
				key_func_select_flag = 0;
				EEP_Sys_Wr(EEP_Detector_BPS_Select, mmi_tmp[line_1]);
				Initialize_UARTBps();
			} else {
				key_func_select_flag = 1;
				mmi_tmp[line_1] = Det_BPS;
			} break;
			
		case KEY_UP_S:
			if (key_func_select_flag == 1) {
				mmi_tmp[line_1]++;
				if (mmi_tmp[line_1] > 2)
					mmi_tmp[line_1] = 0;
			} break;
			
		case KEY_DW_S:
			if (key_func_select_flag == 1) {
				mmi_tmp[line_1]--;
				if (mmi_tmp[line_1] == 0xffff)
					mmi_tmp[line_1] = 2;
			} break;
		case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();
			break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Dev_Ma_Cal_Set_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag == 0)			EEP_Sys_Wr(EEP_4mA_Data, fADBuf[0]);
			else if (key_func_select_flag == 1)		EEP_Sys_Wr(EEP_20mA_Data, fADBuf[0]);
			//else if(key_func_select_flag == 2)	
			key_func_select_flag++;
			break;
		case KEY_UP_S:	break;
		case KEY_DW_S:	break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Dev_Mode_Set_Key()
{
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag) {
				key_func_select_flag = 0;
				EEP_Sys_Wr(EEP_Device_Mode_Select, mmi_tmp[line_1]);
			} else {
				key_func_select_flag = 1;
				mmi_tmp[line_1] = Device_Mode_Select;
			} break;
			
		case KEY_UP_S:
			if (key_func_select_flag == 1) {
				mmi_tmp[line_1]++;
				if (mmi_tmp[line_1] > Det_DAC_CONTROL_MODE)
					mmi_tmp[line_1] = Det_NORMAL_MODE;
			} break;
		case KEY_DW_S:
			if (key_func_select_flag == 1) {
				mmi_tmp[line_1]--;
				if (mmi_tmp[line_1] == 0xffff)
					mmi_tmp[line_1] = Det_DAC_CONTROL_MODE;
			} break;
		case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();	
			break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Det_Ir_Responsivity_Set_Key(void)
{
	Cousor_Location(line_1, line_3);
		
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag) {
				key_func_select_flag = 0;	
				if (setting_mode_cursor == line_1)
					Det_Write_Single_Func(DET_EEP_CAL_BUF_IR430_RESPONSIVITY, mmi_tmp[line_1]);
			} else {
				key_func_select_flag = 1;
				mmi_tmp[line_1] = det_ir_reponsivity_466;
			} break;
		case KEY_UP_S:
		case KEY_DW_S:
			if (!key_func_select_flag) {
				//Cousor_Location(line_1, line_3);
			} else {
				if (setting_mode_cursor == line_1) {
					mmi_tmp[line_1] = Long_Key(mmi_tmp[line_1]);
				
					if (mmi_tmp[line_1] > 2000)		mmi_tmp[line_1] = 2000;
					else if (mmi_tmp[line_1] < 1)	mmi_tmp[line_1] = 1;	
				}
			} break;
		case KEY_RE_S:
			if (det_power_control & Det_Default_Data_Check_Bit) {
				det_power_control &= ~Det_Default_Data_Check_Bit;
				mmi_mode = FMODE_NORMAL;
			} else {
				if (key_func_select_flag)	key_func_select_flag = 0;
				else						Reset_Key_Check();
			} break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:
		case KEY_DW_L:
			if (!key_func_select_flag) {
				//Cousor_Location(line_1, line_3);
			} else {
				if (setting_mode_cursor == line_1) {
					mmi_tmp[line_1] = Long_Key(mmi_tmp[line_1]);
				
					if (mmi_tmp[line_1] > 2000)		mmi_tmp[line_1] = 2000;
					else if (mmi_tmp[line_1] < 1)	mmi_tmp[line_1] = 1;
				}
			} break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}	
}

void Func_Det_Relay_Set_Key(void)
{
	Cousor_Location(line_1, line_2);
	
	switch (KeyData)
	{
		case KEY_FU_S:
		if (key_func_select_flag == 1)
		{
			key_func_select_flag = 0;
			
			if (setting_mode_cursor == line_1)			Det_Write_Single_Func(DET_EEP_ALM_RLY_OPERATION, mmi_tmp[line_1]);
			else if (setting_mode_cursor == line_2)		Det_Write_Single_Func(DET_EEP_TRB_RLY_OPERATION, mmi_tmp[line_2]);
		}
		else
		{
			key_func_select_flag = 1;
			mmi_tmp[line_1] = alm_relay_operation;
			mmi_tmp[line_2] = trb_relay_operation;
		} break;
		case KEY_UP_S:
		case KEY_DW_S:
		if (key_func_select_flag == 0) {
			//Cousor_Location(line_1, line_2);
		} else if (key_func_select_flag == 1) {
			if (setting_mode_cursor == line_1)
			{
				if (mmi_tmp[line_1])	mmi_tmp[line_1] = 0;
				else					mmi_tmp[line_1] = 1;
			}
			else if (setting_mode_cursor == line_2) {
				if (mmi_tmp[line_2])	mmi_tmp[line_2] = 0;
				else					mmi_tmp[line_2] = 1;
			}
		} break;
		case KEY_RE_S:
		if (key_func_select_flag)	key_func_select_flag = 0;
		else						Reset_Key_Check();
		break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Det_Function_Set_Key(void)
{
	Cousor_Location(line_1, line_4);
	
	switch(KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag) 
			{
				key_func_select_flag = 0;
				
				switch (setting_mode_cursor)
				{
					case line_1:	Det_Write_Single_Func(DET_EEP_BIT_MODE, mmi_tmp[line_1]);		break;
					case line_2:	Det_Write_Single_Func(DET_EEP_UV_PWR_CTRL, mmi_tmp[line_2]);	break;
					case line_3:	Det_Write_Single_Func(DET_EEP_LOG_MODE, mmi_tmp[line_3]);		break;
					case line_4:	Det_Write_Single_Func(DET_EEP_FLAME_TYPE, mmi_tmp[line_4]);		break;
				}
			}
			else 
			{
				key_func_select_flag = 1;
				mmi_tmp[line_1] = bit_operation;
				mmi_tmp[line_2] = det_pwr_ctr_value;
				mmi_tmp[line_3] = det_log_mode_value;
				mmi_tmp[line_4] = FlameType;
				DPRINT1("FlameType %d\r\n", FlameType);
			} break;
		case KEY_UP_S:
		case KEY_DW_S:
			if (!key_func_select_flag) {
				//Cousor_Location(line_1, line_3);
			} else {
				
				if (setting_mode_cursor == line_1)
				{
					if (KeyData == KEY_UP_S) {
						mmi_tmp[line_1]++;
						if (mmi_tmp[line_1] > 2)
							mmi_tmp[line_1] = 0;
					} else {
						mmi_tmp[line_1]--;
						if(mmi_tmp[line_1] == 0xffff)
							mmi_tmp[line_1] = 2;
					}
				}
				else if (setting_mode_cursor == line_2)
				{
					if (mmi_tmp[line_2])	mmi_tmp[line_2] = 0;
					else					mmi_tmp[line_2] = 1;
				}
				else if (setting_mode_cursor == line_3)
				{
					if (KeyData == KEY_UP_S) {
						mmi_tmp[line_3]++;
						if (mmi_tmp[line_3] > 3)
							mmi_tmp[line_3] = 0;
					} else {
						mmi_tmp[line_3]--;
						if (mmi_tmp[line_3] == 0xffff)
							mmi_tmp[line_3] = 3;
					}	
				}
				else if (setting_mode_cursor == line_4)
				{
					if (mmi_tmp[line_4])		mmi_tmp[line_4] = 0;
					else						mmi_tmp[line_4] = 1;
				}
			} break;
		case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();
			break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}	
}

void Func_Dev_DAC_Set_Key(void)
{
	switch (KeyData)
	{
		case KEY_FU_S:
			if (key_func_select_flag) {
				key_func_select_flag = 0;
				EEP_Sys_Wr(EEP_DAC_Fullscale_Time, mmi_tmp[line_1]);
			}
			else {
				key_func_select_flag = 1;
				mmi_tmp[line_1] = FullScaleTime;
			} break;
			
		case KEY_UP_S:
			if (key_func_select_flag == 1) {
				mmi_tmp[line_1]++;
				if (mmi_tmp[line_1] > 5)
					mmi_tmp[line_1] = 0;	
			} break;
		case KEY_DW_S:
			if (key_func_select_flag == 1) {
				mmi_tmp[line_1]--;
				if (mmi_tmp[line_1] == 0xffff)
					mmi_tmp[line_1] = 5;
			} break;
		case KEY_RE_S:
			if (key_func_select_flag)	key_func_select_flag = 0;
			else						Reset_Key_Check();
			break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

void Func_Det_Test_Mode_Key(void)
{
	switch (KeyData)
	{
		case KEY_RE_S:	Reset_Key_Check();	break;
	}
}
 
void Func_Det_Eep_Clear_Mode_Key(void)
{
	Cousor_Location(line_2, line_3);

	switch (KeyData)
	{
		case KEY_FU_S:
		{	
			switch (setting_mode_cursor)
			{
				case line_2:
					Det_Write_Multi_Func(DET_EEP_EVT_BIT_IR430_1S_PTOP_MIN, DET_EEP_EVT_IR530_DRIFT_T_MAX_H);
					ModeSetFlag[FMODE_DET_EEP_CLEAR_MODE] = 1;
					break;
				case line_3:
					Det_Write_Multi_Func(DET_EEP_CAL_BUF_IR430_RESPONSIVITY,DET_EEP_CAL_VAL_IR530_07);
					ModeSetFlag[FMODE_DET_EEP_CLEAR_MODE] = 2;
					break;;	
			}
			ModeSetFlow[FMODE_DET_EEP_CLEAR_MODE] = 3;
			setting_mode_cursor = 0;
			LCD_Clear_Line_All();
		} break;
		case KEY_UP_S:
		case KEY_DW_S:	break;
		case KEY_RE_S:	Reset_Key_Check();	break;
		case KEY_TE_S:	break;
		case KEY_FU_L:	break;
		case KEY_UP_L:	break;
		case KEY_DW_L:	break;
		case KEY_RE_L:	break;
		case KEY_TE_L:	break;
	}
}

U8 Cousor_Location(U8 start, U8 end)
{
	if (KeyData == KEY_UP_S)
	{
		if (key_func_select_flag == 0)	
		{
			if (setting_mode_cursor != start)	setting_mode_cursor--;
			else								setting_mode_cursor = end;
		}
	}
	else if(KeyData == KEY_DW_S)
	{
		if (key_func_select_flag == 0)
		{
			if (setting_mode_cursor != end)		setting_mode_cursor++;
			else								setting_mode_cursor = start;
		}
	}
	
	if (setting_mode_cursor < start)
		setting_mode_cursor = start;
	
	return setting_mode_cursor;
}

U8 LCD_Character_Cousor_Location(U8 h_cousor_size, U8 h_cousor, U8 mode)
{
	switch (KeyData)
	{
		case KEY_FU_S:
			if (mode == Det_T_CALIBRATION_MODE) 
			{
				if (Calibration_Mode_Flow == 0)		Calibration_Mode_Flow = 1;
				else								character_cousor++;
				
				if (character_cousor == h_cousor_size) {
					character_cousor = 0;
					if (Calibration_Mode_Flow < 4)
						Calibration_Mode_Flow++;
				}
			}
			else if (mode == Det_DAC_CONTROL_MODE)
			{
				if (DAC_Mode_Flow == 0) {	// Mode Select
					DAC_Mode_Flow = DAC_Mode_cousor + 1;
					if (DAC_Mode_Flow == 3)
						DAC_Mode_cousor = 0;
				}
				else if (DAC_Mode_Flow == 3) 
				{
					if (FullScaleMode == 0)
						FullScaleMode = DAC_Mode_cousor + 1;
					DAC_Mode_cousor = 0;
				}
				else if (DAC_Mode_Flow >= 5)	DAC_Mode_Flow = 6;
				else							character_cousor++;

				if (DAC_Mode_Flow == 1) {	// DAC Value Control
					if (character_cousor == h_cousor_size + 1) {
						character_cousor = 0;
						DAC_Mode_Flow = 4;	// DAC Output
					}
				}
				else {						// DAC_Mode_Flow == 2					
					if (character_cousor == h_cousor_size) {
						character_cousor = 0;
						DAC_Mode_Flow = 4;
					}
				}
			}
			else
			{
				character_cousor++;
				if (character_cousor == h_cousor_size) {
					character_cousor = 0;
					if (Calibration_Mode_Flow == 1)
						Calibration_Mode_Flow = 2;
				}
			} break;
			
		case KEY_UP_S:
			if (mode == Det_DAC_CONTROL_MODE && (DAC_Mode_Flow == 0 || DAC_Mode_Flow == 3) ) {
				if (h_cousor)	h_cousor--;
				else			h_cousor = h_cousor_size;
			}
			else if (mode == Det_T_CALIBRATION_MODE && Calibration_Mode_Flow == CALIB_ERROR) {
				h_cousor++;
				if (h_cousor > h_cousor_size)
					h_cousor = 0;
			}
			else {
				h_cousor++;
				if (h_cousor > 9)
					h_cousor = 0;
			} break;
			
		case KEY_DW_S:
			if (mode == Det_DAC_CONTROL_MODE && (DAC_Mode_Flow == 0 || DAC_Mode_Flow == 3) ) {
				h_cousor++;
				if (h_cousor > h_cousor_size)
					h_cousor = 0;
			}
			else if (mode == Det_T_CALIBRATION_MODE && Calibration_Mode_Flow == CALIB_ERROR) {
				if (h_cousor)	h_cousor--;
				else			h_cousor = h_cousor_size;
			}
			else {
				h_cousor--;
				if (h_cousor > 10)
					h_cousor = 9;
			} break;
			
		case KEY_RE_S:
			if (mode == Det_T_CALIBRATION_MODE) {
				character_cousor--;
				if (character_cousor == 0xff) {
					if (Calibration_Mode_Flow >= 2 && Calibration_Mode_Flow < 5) {
						Calibration_Mode_Flow--;
						character_cousor = h_cousor_size-1;
					}
					else	character_cousor = 0;
				}
			}
			else if (mode == Det_DAC_CONTROL_MODE) {
				character_cousor--;
				if (character_cousor == 0xff) {
					DAC_Mode_Flow = 0;
					character_cousor = 0;
				}
			}
			else {
				character_cousor--;
				if (character_cousor == 0xff)
					character_cousor = 0;
			} break;
	}
	return h_cousor;
}

U16 Long_Key(U16 cnt)
{
	if ( (KeyData == KEY_UP_S) | (KeyData == KEY_UP_L) ) {
		cnt++;
		key_push_check = 0;
	} else if ( (KeyData == KEY_DW_S) | (KeyData == KEY_DW_L) ) {
		cnt--;
		key_push_check = 0;
	}
	return cnt;
}

void Normal_Return_Check(void)
{
	if (KeyData != 0xff)
	{
		sTimerFlag[TMR_NORMAL_RETURN] = 1;
		if (mmi_mode >= FMODE_DEV_MA_CAL_SET)	sTimerTick[TMR_NORMAL_RETURN] = NORMAL_RETURN_CNT ;
		else									sTimerTick[TMR_NORMAL_RETURN] = LONG_RETURN_CNT;
	}
}

void Reset_Key_Check(void)
{
	if (mmi_mode <= FMODE_CONFIGIURATION)				Mode_Select();
	else if (mmi_mode <= FMODE_MODBUS_CONFIGIURATION)	mmi_mode = FMODE_CONFIGIURATION;
	else if (mmi_mode <= FMODE_MODBUS_BPS_SET)			mmi_mode = FMODE_MODBUS_CONFIGIURATION;
	else if (mmi_mode <= FMODE_DET_FUNCTION_SET)		mmi_mode = FMODE_DET_CONFIGIURATION;
	else if (mmi_mode <= FMODE_DEV_MODE_SET)			mmi_mode = FMODE_DEV_CONFIGIURATION;
	else if (mmi_mode == FMODE_DEV_DAC_CONFIGURATION)	mmi_mode = FMODE_DAC_CONTROL_MODE;

	alarm_state_old = 0xff;
	setting_mode_cursor = 0;
	key_func_select_flag = 0;
	
	multiple_wr.startAddress = 0;
	multiple_wr.endAddress = 0;
	multiple_wr.to_send = 0;
	multiple_wr.totalSend = 0;
	modbus_rtu_master.write_flag = 0;
	
	Initialize_ModeFlag();
}
