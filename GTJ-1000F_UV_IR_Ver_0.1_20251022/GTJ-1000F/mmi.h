#ifndef __mmi_h__
#define __mmi_h__


#define	Det_Power_Control_Bit			0x01
#define	Det_Power_System_Check_Bit		0x04
#define	Det_Default_Data_Check_Bit		0x10
#define	Det_System_Check_Complete		0x20

#define	Det_UV_Calibration_Count_MAX	70
#define	Det_BlackBody_Delay_Count		5
#define	Det_BlackBody_Stabilize_Count	5
#define	Det_BlackBody_Average_Count		10
#define	Det_BlackBody_DataCheck_Count	10
#define	DAC_OUT_CAL						(5*((float)dac_data/(float)4096))
#define	DAC_CNT_MAX						7

#define HYDROCARBON						0
#define NON_HYDROCARBON					1

#define CLEAR_EEP_UV_BIT_FINISH_TIME	0


#define	DAC_FULLSCALE_ZERO		5421	//2hz
#define	DAC_FULLSCALE_MAX		49104	//100hz
#define	DAC_VALUE_UP_LMT		65535
#define	DAC_VALUE_UP_WAR		16400
#define	DAC_OUT_VOL(x,y)		y=(((float)x/1000*(float)65535)/5)
#define	DAC_OUT_VAL(x,y)		y=(((float)x/(float)65535)*5*1000)
#define	DAC_CAL_POINT			7

#define	FREQ_CTRL_MIN_T				180
#define	FREQ_CTRL_MIN_U				5500

#define Det_UVIR_CHECK_POINT		7
#define Det_TRIPLE_IR_CHECK_POINT	21

#define DIFF_FREQ					8	

#define DET_SIZE					20
#define RATIO_LIMIT_SIZE			1000			


enum {
	FMODE_WARM_UP = 0,
	FMODE_DET_SYSTEM_CHECK,
	FMODE_NORMAL,
	FMODE_U_CALIBRATION_MODE,
	FMODE_T_CALIBRATION_MODE,
	FMODE_DET_EEP_CLEAR_MODE,
	FMODE_DET_EEP_EVENT_LOG_CLEAR,
	FMODE_DET_EEP_CALIBRATION_CLEAR,
	FMODE_DET_EEP_FACTRORY_CLEAR,
	FMODE_DET_EEP_CLEAR_RESULT,
	FMODE_DET_TEST_MODE,
	FMODE_DET_CALIB_DEFAULT_SET,
	FMODE_DET_TRIPLE_REL_RATIO_LIMIT_SET,
	FMODE_CONFIGIURATION,
	FMODE_DET_CONFIGIURATION,
	FMODE_DEV_CONFIGIURATION,
	FMODE_MODBUS_CONFIGIURATION,
	FMODE_MODBUS_ADDRESS_SET,
	FMODE_MODBUS_BPS_SET,
	FMODE_DET_RELAY_SET,
	FMODE_DET_IR_RESPONSIVITY_SET,
	FMODE_DET_DETECTION_DELAY_SET,  
	FMODE_DET_FUNCTION_SET,
	FMODE_DEV_MA_CAL_SET,
	FMODE_DEV_MODE_SET,
	//FMODE_DET_MA_CAL_SET,
	FMODE_OPERATION_CANCLE,
	FMODE_NOT_CONNECTOR,
	FMODE_DAC_CONTROL_MODE,
	FMODE_DEV_DAC_CONFIGURATION,
	FMODE_MAX
};

enum {
	ALARM_STATE_NO = 0,
	ALARM_STATE_BIT_PROGRESS,		// 1
	ALARM_STATE_WARNING,			// 2
	ALARM_STATE_IR,					// 3
	ALARM_STATE_UV,					// 4
	ALARM_STATE_FLAME,				// 5
	ALARM_STATE_PWR_FAULT,			// 6
	ALARM_STATE_BIT_FAULT,			// 7
	ALARM_STATE_IR_OFFSET_FAULT,	// 8
	ALARM_STATE_EEP_FAULT,			// 9
	ALARM_STATE_MODBUS_FAULT,		// 10
	ALARM_STATE_NOT_CONNECT,		// 11
	ALAMR_STATE_DET_POWER_OFF,		// 12
	ALAMR_STATE_DET_SYSTEM_CHECK,	// 13
	ALARM_WRITE_SINGLE_COMM_SUC,	// 14
	ALARM_WRITE_SINGLE_COMM_ERR,	// 15
	ALATM_STATE_MODBUS_TIME_OUT,
	ALARM_STATE_MAX
};

enum {
	Det_Power_24V = 0,
	Det_Power_16V,
	Det_Power_32V,
	Det_Power_MAX
};

enum {
	Det_Cal_Normal = 0,
	Det_Cal_CPM_Fault,
	Det_Cal_CPS_LO,
	Det_Cal_CPS_UP,
	Det_Cal_UV_Det_Limit_LO,
	Det_Cal_UV_Det_Limit_UP,
	Det_Cal_IR_Responsivity_LO,
	Det_Cal_IR_Responsivity_UP,
	Det_Cal_CPS_Fault,
	Det_Cal_UV_Det_Limit_Fault,
	Det_Cal_IR_Responsivity_Fault,
};

enum {
	Det_NORMAL_MODE = 0,
	Det_U_CALIBRATION_MODE,
	Det_T_CALIBRATION_MODE,
	Det_DAC_CONTROL_MODE
};

enum {
	DAC_VALUE_MODE = 0,
	DAC_VOLTAGE_MODE,
	DAC_MODE_MAX
};

enum {
	CALIB_READY = 0,
	CALIB_START,
	CALIB_FREQ_ADJUST,
	CALIBRATION,
	CALIB_VERIFY,
	CALIB_ERROR
};

enum {
	ERR_NONE = 0,
	ERR_BUFF,
	ERR_FREQ_LO,
	ERR_FREQ_UP,
	ERR_FREQ_OUT,
	ERR_COMP
};

enum {
	IR_466 = 0,
	IR_395,
	IR_530,
	IR_SENSOR_MAX
};

enum {
	BIT_MODE_OFF = 0,
	BIT_MODE_AUTO,
	BIT_MODE_MANUAL
};


extern U8			det_power_control, det_power_out;
extern U8			bit_operation, det_pwr_ctr_value;
extern S8			det_log_mode_value;
extern const S16 	DET_EEP_SYS_DEFAULT[];

extern U8			ModeSetFlow[FMODE_MAX];
extern U8			ModeSetFlag[FMODE_MAX];
extern U8			Det_System_Fault_Flag;
extern U16 			uv_cps, uv_det_limit; 
extern U16			dac_data;
extern U16			dac_flag;
extern U8			dac_start_flag;
extern U16			blackbody_freq, blackbody_ctr_flag;
extern U8			Det_IR_Responsivity_Cal_Data_1,Det_IR_Responsivity_Cal_Data_2,Det_IR_Responsivity_Cal_Data_3,Det_IR_Responsivity_Cal_Data_4;
extern U8			Calibration_Mode_Flow;	
extern U16			Calibration_Mode_Count;	
extern U8			Device_Mode_Select;
extern U16			Det_IrAvPtoP_Data[IR_SENSOR_MAX * 7];
extern U8			FullScaleMode, LogMode_flag;
extern U8			FlameType;
extern U8			FlameType_temp;
extern U8			FlameDetDelay_Sec;
extern U8			alarm_state, alarm_state_old;
extern U8			mmi_mode, mmi_mode_old;	

extern U8			DAC_Mode_Flow;
extern U16 			DAC_Voltage;
extern U8			DAC_Mode_cousor;

extern U8			Dev_DAC_Value[5];
extern U8			Dev_DAC_Voltage[4];
extern S16			REF_DAC_DATA[8];
extern U8			IR_Size_SetMode, FullScaleTime;
extern U8			ERROR_cousor;

extern U8			Det_calib_chk_flag;

extern const S16* const DET_EEP_RATIO_LIMIT[DET_SIZE+1] PROGMEM;


extern void Initialize_System( void );
extern void Initialize_ModeFlag(void);
extern void LCD_Operating(void);
extern void Func_Warm_Up(void);
extern void Func_Normal(void);
extern void Func_U_Calibration_Mode(void);
extern void Func_T_Calibration_Mode(void);
extern void Func_Configration(void);
extern void Func_Det_Configration(void);
extern void Func_Dev_Configration(void);
extern void Func_ModBus_Configration(void);
extern void Func_Det_Relative_Ratio_Limit_Set(void);
extern void Func_Det_Calib_Default_Set(void);
extern void Ratio_lmt_read(U16 Det_num);
extern void Func_ModBus_Address_Set(void);
extern void Func_ModBus_BPS_Set(void);
extern void Func_Det_mA_Cal_Set(void);
extern void Func_Dev_mA_Cal_Set(void);
extern void Func_Dev_Mode_Set(void);
extern void Func_Det_Relay_Set(void);
extern void Func_Det_IR_Responsivity_Set(void);
extern void Func_Det_Function_Set(void);
extern void Func_Det_Eep_Clear_Mode(void);
extern void Func_Det_Test_Mode(void);
extern void Func_Not_Connector(void);
extern void Func_Operation_Cancle(U8 mode);
extern void Func_Det_System_Check_Mode(void);
extern void Func_Det_System_Data_Check(U8 flag);
extern void Func_Det_System_Check_Result(void);
extern void Det_Status(void);
extern void Mode_Select(void);
extern void Det_Power_Task(void);
extern void LED_Indicator(void);
extern void Log_Handler(void);
extern void Func_Det_Detection_Delay_Set(void);
extern void Func_DAC_Control_Mode(void);
extern void Func_Dev_DAC_Set(void);

#endif // __mmi_h__
