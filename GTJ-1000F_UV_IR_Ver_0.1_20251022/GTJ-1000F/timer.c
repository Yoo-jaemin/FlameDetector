/* *********************************************************************************
 *  GTJ-1000F.c
 *
 *	Created: 2025-09-30 
 *	Author : jmyoo
  
 ********************************************************************************* */ 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "dbg_printf.h"

#include "timer.h"
#include "gpio.h"
#include "uart2560.h"
#include "modbus.h"
#include "mkey.h"
#include "adc.h"
#include "lcd.h"
#include "dac7571.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16	sTimerTick[ TMR_BUFF_MAX ];
U16	sTimerFlag[ TMR_BUFF_MAX ];
U16	EncoderFreqTick[2];
U8	timer_test = 0;

void Initialize_Timer( void );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_Timer( void )
{
	TCCR1A = 0x00;
	TCCR1B = 0x01; 				//No prescaling
	TCNT1 = TMR3_REG_VALUE; 	//Enable TOIE3
	
	TIMSK1 = 0x01;

	TCCR3A = 0x00;
	TCCR3B = 0x01; 				//No prescaling
	TCNT3 = TMR3_REG_VALUE; 	//Enable TOIE3
	
	TIMSK3 = 0x01;
	
	PCICR = 0x01;
	PCMSK0 = 0x80;
	
	sTimerTick[TMR_10MS] = 10;
	sTimerFlag[TMR_10MS] = 0;
	
	sTimerTick[TMR_100MS] = 100;
	sTimerFlag[TMR_100MS] = 0;
	
	sTimerTick[TMR_250MS] = 250;
	sTimerFlag[TMR_250MS] = 0;

	sTimerTick[TMR_1000MS] = 1000;
	sTimerFlag[TMR_1000MS] = 0;

	sTimerTick[TMR_SELFTEST] = 3;
	
	sTimerFlag[TMR_DET_SYSTEM_CHECK] = 0;

	sTimerTick[TMR_NORMAL_RETURN] = NORMAL_RETURN_CNT;
	sTimerFlag[TMR_NORMAL_RETURN] = 0;

	sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
	sTimerFlag[TMR_MODBUS_TIMEOUT] = 0;
	
	sTimerTick[TMR_ADC_TASK] = 0x8018;
	sTimerFlag[TMR_ADC_TASK] = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Chopper Blade DAC = MCU가 DAC을 통해 회전 모터 전압을 제어
ISR(PCINT0_vect)	// PINB 인터럽트
{
	if (PINB & 0x80) {	//주기(1펄스)
		EncoderFreqTick[0] = TCNT1;						// 잔여 카운트
		TCNT1 = 0;
		EncoderFreqTick[1] = sTimerTick[TMR_ENC_FREQ];	// 1주기 동안 발생한 16bit 타이머 카운터 오버플로우 횟수
		sTimerTick[TMR_ENC_FREQ] = 0;
		sTimerFlag[TMR_ENC_FREQ] = 1;
		
		if (sTimerFlag[TMR_ENC_FREQ]) {
			sTimerFlag[TMR_ENC_FREQ] = 0;
			blackbody_freq = 1 / (( 65536.f * EncoderFreqTick[1] + EncoderFreqTick[0] ) * 0.000000000625 );
			// 16bit Timer Counter * I/O 인터럽트 발생 까지의 Overflow 횟수 + 나머지 잔여 카운트 = 1주기 총 카운트 * 16Mhz 크리스탈 1클럭 소요 시간 / 100(1:100 Scale 표시 소수점 2자리까지 제어 가능)
		}
	}
}


ISR(TIMER1_OVF_vect)
{
	sTimerTick[TMR_ENC_FREQ]++;
}

// Timer3 overflow interrupt service routine

ISR(TIMER3_OVF_vect)
{
	TCNT3 = TMR3_REG_VALUE;		//Enable TOIE3
	
	Key_Check();
	Relay_Check();
	USART1_Rx_Packet_Check();
	USART2_Rx_Packet_Check();
	
	modbus_rtu_master.delay_ms++;
	modbus_rtu_master.connect_chk_cnt++;
	
	if (sTimerTick[TMR_ADC_TASK] & 0x8000) {
		if (sTimerTick[TMR_ADC_TASK] == 0x8000) {
			//sTimerTick[TMR_ADC_TASK] = 0x8018; // 25ms 
			sTimerTick[TMR_ADC_TASK] = 0x8003; // 25ms 
			sTimerFlag[TMR_ADC_TASK] = 1;
			ADC_StartConversion;
		}	else	sTimerTick[TMR_ADC_TASK]--;
	}

	sTimerTick[TMR_10MS]--;
	if (sTimerTick[TMR_10MS] == 0)
	{
		sTimerTick[TMR_10MS] = 10;
		FullScale_Mode(FullScaleMode, 20);
	}

	sTimerTick[TMR_100MS]--;
	if (sTimerTick[TMR_100MS] == 0)
	{
		sTimerTick[TMR_100MS] = 100;
		sTimerFlag[TMR_100MS] = 1;
	}

	sTimerTick[TMR_250MS]--;
	if (sTimerTick[TMR_250MS] == 0)
	{
		sTimerTick[TMR_250MS] = 250; 
		
		if (key_func_select_flag) {
			if (!key_push_check)
				sTimerFlag[TMR_250MS] = !sTimerFlag[TMR_250MS];
			else	sTimerFlag[TMR_250MS] = 1;
			
		} else {
			if (mmi_mode == FMODE_U_CALIBRATION_MODE)
				sTimerFlag[TMR_250MS] = !sTimerFlag[TMR_250MS];
			else if(mmi_mode == FMODE_DAC_CONTROL_MODE) {
				sTimerFlag[TMR_250MS] = !sTimerFlag[TMR_250MS];
				sTimerTick[TMR_DAC_DELAY]--;
			}
			else	sTimerFlag[TMR_250MS] = 1;		
		}
	}
	
	if (sTimerTick[TMR_MODBUS_TIMEOUT] & 0x8000)
	{
		if (sTimerTick[TMR_MODBUS_TIMEOUT] == 0x8000) {
			sTimerTick[TMR_MODBUS_TIMEOUT] = 0;
			sTimerFlag[TMR_MODBUS_TIMEOUT] = 1;
		}
		else	sTimerTick[TMR_MODBUS_TIMEOUT]--;
	}

// ========================================================== 1sec ==========================================================
	sTimerTick[TMR_1000MS]--;
	if (sTimerTick[TMR_1000MS] == 0)
	{
		sTimerTick[TMR_1000MS] = 1000;
		sTimerFlag[TMR_1000MS] = 1;
		sTimerFlag[TMR_1000MS_String] = 1;
		
		if (Device_Mode_Select == Det_U_CALIBRATION_MODE)
		{
			if ((Calibration_Mode_Flow > 3) && (Calibration_Mode_Flow < 14))	Calibration_Mode_Flow++;
			else if (Calibration_Mode_Flow == 3)								Calibration_Mode_Count--;
			else																Calibration_Mode_Count = 0;
		}
		else if (Device_Mode_Select == Det_T_CALIBRATION_MODE) 
		{
			blackbody_ctr_flag = 1;
			if (Calibration_Mode_Flow) {
				if (Calibration_Mode_Count != 0)	
					Calibration_Mode_Count--;
			}
		} 
		
		if (det_power_control & Det_Power_Control_Bit) {	// power on
			sTimerFlag[TMR_DET_SYSTEM_CHECK]++;
			if (sTimerFlag[TMR_DET_SYSTEM_CHECK] > 10)	
				sTimerFlag[TMR_DET_SYSTEM_CHECK] = 0;
		}
		
		if (mmi_mode == FMODE_WARM_UP) {
			sTimerTick[TMR_SELFTEST]--;
			if (sTimerTick[TMR_SELFTEST] == 0) 
				Mode_Select();
		} else if (mmi_mode > FMODE_NORMAL) {
			
			if (sTimerFlag[TMR_NORMAL_RETURN] == 1)		sTimerTick[TMR_NORMAL_RETURN]--;
			else										sTimerTick[TMR_NORMAL_RETURN] = NORMAL_RETURN_CNT;
			
			if (sTimerTick[TMR_NORMAL_RETURN] == 0) {
				sTimerFlag[TMR_NORMAL_RETURN] = 0;
				Mode_Select();
				alarm_state_old = 0xff;
				key_func_select_flag = 0;
				setting_mode_cursor = 0;
				multiple_wr.to_send = 0;
				multiple_wr.totalSend = 0;
			}
		}
		
		if (ModeSetFlow[FMODE_DET_EEP_CLEAR_MODE] > 2 && ModeSetFlow[FMODE_DET_EEP_CLEAR_MODE] < 5)
			ModeSetFlow[FMODE_DET_EEP_CLEAR_MODE]++;	
		
		STATUS_LED_T();
	} 
// =================================================================================================================================	
}

void FullScale_Mode(U8 mode, U8 cnt)
{
	static U8 Cal_phase = 0;
	static U8 phase_cnt = 0;

	if (mode == 1)
	{
		timer_test++;
		if ((timer_test % FullScaleTime) == 0)
			dac_data++;
		
		if (timer_test == cnt)
		{
			dac8571_dac_wr(dac_data);
			if (dac_data >= DAC_FULLSCALE_MAX) {
				DAC_Mode_Flow = 5;
				sTimerTick[TMR_DAC_DELAY] = 20;
				dac_data = 0;
				FullScaleMode = 0;
			}
			timer_test = 0;
		}
	}
	else if (mode == 2)
	{
		timer_test++;
		if (timer_test == cnt)
		{
			timer_test = 0;
			phase_cnt++;
			if (phase_cnt >= 150) {
				phase_cnt = 0;
				Cal_phase++;
				dac_data = REF_DAC_DATA[Cal_phase];
				
				if (Cal_phase > DAC_CAL_POINT) {
					Cal_phase = 0;
					DAC_Mode_Flow = 5;
					sTimerTick[TMR_DAC_DELAY] = 20;
					dac_data = 0;
					FullScaleMode = 0;
				}
			}
			dac8571_dac_wr(dac_data);
		}
	}
}