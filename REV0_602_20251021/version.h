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

/*
*******************************************************************************
Fuses Setting
* EXTENDED.BODLEVEL	: Brown-out detection at VCC=4.3 V
* HIGH.OCDEN	    : X
* HIGH.JTAGEN	    : X
* HIGH.SPIEN	    : O
* HIGH.WDTON	    : X
* HIGH.EESAVE	    : O
* HIGH.BOOTSZ	    : Boot Flash size=4096 words start address=$F000 4096W_F000
* HIGH.BOOTRST	    : X
* LOW.CKDIV8	    : X
* LOW.CKOUT	        : X
* LOW.SUT_CKSEL	    : Ext. Crytal Osc. 8.0- MHz; Start-up time: 258 CK + 4.1 ms

|Fuse Register | Value |
|EXTENDED      | 0xFC  |
|HIGH          | 0xD1  |
|LOW           | 0xCE  |
*******************************************************************************
*/

//*****************************************************************************
//
//*****************************************************************************

#define	_FW_VERSION_MA		0
#define	_FW_VERSION_MI		60

#define	_FW_VERSION_SUB		2  // jmyoo

// REV0.00 (2017-01-17 ���� 3:03:37)
// GTF-1100 ��ٸ��� ���� �ڵ��� �����ϴ�.... YJ.... ������....
// ������ ������ ��ǰ�� ��ٸ��� �ð��̴�....
// ������ Ʈ������ �����ϰ� ��� �԰��ȴٰ� �ϴ� �Ͼ��..
// Triple�̶�.. ���� ������ �� �ְ���....

// REV0.01 (2017-02-13 ���� 2:29:41)
// GTF-1100 ��ٸ��� ���� ȭ���Լ�! ���� 10�� ����µ� ��ü ����� �ɸ���...
// ���� ���߿��� ������ �� ������...
// �����̾� ������ �Ͽ�¡�� ���� ���� �� ������...
// Infratec IR Sensor ������ ���� Gain���� ��� ��������...
// 1�� GAIN �� LIMIT ���� �� �ð� Ȯ���� ���� ����..

// REV0.02 (2017-02-16 ���� 2:04:36)
// Raw Data Log��

// REV0.03 (2017-02-17 ���� 3:49:23)
// Raw Data Log�� (IR Only, CR LF ����)
// LME-336-F 36���� ȭ�� Raw Data �м�����

// REV0.04 (2017-02-18 ���� 9:39:11)
// ADC Sampling �ֱ� 4ms�� ����
// DIP S/W�� Log Mode �����ǵ��� ��� �߰�
// UV Sampling �ֱ� 10ms�� ����
// Triple-IR Buffer �߰�

// REV0.05 (2017-02-21 ���� 6:51:44)
// Triple-IR Buffer �߰�
// Pattern Log ����

// REV0.06 (2017-02-27 ���� 1:59:31)
// 

// REV0.07 (2017-02-27 ���� 1:59:31)
// Data Logging.. Debug Port Rx Logic �߰�

// REV0.08 (2017-03-17 ���� 10:30:14)
// UV Power Control Logic �߰� (2017-03-21 ���� 1:56:04)
// BIT Sequence ���� (2017-03-21 ���� 1:56:06)
// ȭ�簨�� �����̸� Faultüũ �ʵǵ��� ����
// Log Mode Dip S/W ��ġ���� (6 & 7-off = off, 6p-on & 7p-off = raw data, 6p-off & 7p-on = calculated, 6 & 7-on = reseved)
// IR Filter ����
// ModbusMap ����

// REV0.09 (2017-04-04 ���� 7:08:05)
// IR Filter ����

// REV0.10 (2017-04-13 ���� 7:38:09)
// UV & IR DetDistance ���� (UV CPS Integration Factor 0.4 -> 0.5, IR Peak to Peak Limit 600 -> 540mV)
// UV CPS Limit ���� ���� ����� (�����̾� ������������ UV Sensor����, ���� �α׵������� �޺��������� ���� �����߻� ����)
// RAW DATA LOG. MODE���� UV�� ��µǵ��� ����

// REV0.11 (2017-04-14 ���� 9:48:48)
// Sun Light UV �м���

// REV0.12 (2017-05-11 ���� 4:09:56)
// �űԻ����̾� ���� (�ڸ���ũ�����, UV-C ������ 74-84%)
// UV Source warm-up delay�� ���� BIT Logic ���� (10ms -> 900ms)

// REV0.13 (2017-06-07 ���� 9:19:22)
// �������� ����.. ����� 6��.. ȭ���� ���� 4������ ������.
// �̷����� ������ ���� ������ ���� ��ü�Ǿ���.. ���� ��������� �ʿ��� �ð����̾�����, ��ǰ������ �� �׷��� �ƴϰڴ°�..
// �����̾� ������ �� �浵, �ⱸ�� ��������, UV������ ��ü����, UV���� ������ġ ������ ���� ��ǰ������ ��ġ����,
// IFRATEC IR���� ������� ���� �����缳��, ���۰� �ε����� ������ ���� FILTER ��纯��, ����� �ֺ��� ����� ��������
// ���� UV���� ��... ������ �������� ���� �־���.
// �� � �̽��� �߻����� �𸣳� UV/IR Type�� ���� �ű�����(CWL: 4.66um, HPBW: 180nm) ����� IR������ ������ �Ϸ�Ǹ�,
// ����� ������ ���� �� �Ϸ�� �� ����.
// �űԼ����� Triple-IR���� �� ����Ǳ� ������ �ٶ��.. �ٽ� �����غ��� �Ѵ�.
// ���ۼ��� HOLDING REG.�� ���� (Log Mode, BIT Mode, UV Power Control)

// REV0.14 (2017-06-21 ���� 11:22:05)
// HOLDING REGISTER LIMIT ���� (�Է¹��� ����� ����)
// External BIT Logic Error ����
// ���͸� ���ذ� ���� �����ϵ��� ���� (Holding Register)
// CpsBuf Ŭ���� ���� ����(2ȸ ���� '0'�̸� Ŭ����)
// Detection Sequence Error ����
// Triple detection Logic Start
// LED_Indicator() Timer Vector�� �̵�

// REV0.15 (2017-06-28 ���� 2:09:16)
// UV Source Backlight Logic �߰�

// REV0.16 (2017-07-10 ���� 11:39:49)
// �������� ���� (Peak to Peak Limit 260mV, CPS Limit 2.2)
// UvSum Data �߰�
// System Fault ������� ����
// Data Log Function �и�
// Event Log �߰� (BIT Fault Count, Power Fault Count, Min. Temperature, Max Temperature)
// BIT Drive Logic ����

// REV0.17 (2017-07-31 ���� 5:46:25)
// Relay Operation Latch & Operation Mode EEPROM Address �߰�
// Watchdog Enable
// UVIR BIT Mode���� CPS�� ����ġ �̻��̸� UV Source Off �ǵ��� ���� (��������)
// BIT FAULT ���� ��ȯ�� ��õ� Ƚ�� �ʰ��ÿ��� ����
// BIT �ֱ� EEPROM ����
// BIT ���۽� UV �Ӱ谪 ������ �ð����� �� �ִ밪 EEPROM ���

// REV0.18 (2017-08-09 ���� 9:09:55)
// BIT Fault ��õ� Ƚ�� ������ ���� ����

// REV0.19 (2017-08-10 ���� 12:26:14)
// UV Source Drive Polarity ����

// REV0.20 (2017-08-14 ���� 1:30:04)
// BIT ��õ� Ƚ�� �ʰ��ÿ��� �����Ǵ� �������� �߰�

// REV0.21 (2017-08-28 ���� 12:58:14)
// Event Log �߰� (CPS RAW MAX, CPS ITG MAX, ȭ�簨�� �������� �ֱ� ����� 3�� ������)
// Power Fault Delay ���� (5��)
// EEPROM CRC Error �߻� �� EepErrorFlag ������ Write Address ����
// MODBUS Input Register Mapping Error ���� �� �����߰�
// CRC Write Function ���� (All Data Read ����)

// REV0.22 (2017-08-31 ���� 9:48:47)
// Trouble Relay �⺻�� Energized�� ����
// Warm-up ��� ���� Trouble Relay ���ۻ��·� ���� (GTF-1000�� �Ѿ��� ��û���� �ݿ�)
// BIT Logic ����

// REV0.23 (2017-09-05 ���� 10:24:56)
// Flame Structure Member �߰� (Ir1sPtoPDv = Peak to Peak ����)

// REV0.24 (2017-09-05 ���� 3:07:33)
// BIT UV OFF �ð� 50ms ���� (BIT �Ϸ� �� UV���� ���ɼ� ����)
// UV_Calculation() ���� (UvCpsItg, CpsItgBuf, UvNonDetCnt Clear ���� ����)
// Temperature Log. Delay �߰�
// IR Detection Filter ���� �� �߰� (���� �α׽÷� 4EA�� ��� ��ȭ�� ��α� �� �������, 9�� 26�� KFI�����α� �� ����� �ʿ�)
// IR Detection Logic ���� (������ 3�ʿ��� �ֱ� 2�ʰ� �������� �����Ǿ��� ��쿡�� �ҷ�����)
// KFI ���Ľ��� ���ؿ��� ������ ������ IR Filter ����

// REV0.25 (2017-09-22 ���� 7:08:00)
// �ҷΰ� 20 ~ 22K Lux. �ҹ����� ���͸����� �߰� (IR 1sec. Average Deviation Limit �߰�, ���ذ� EEP MAP �߰�)
// EEPROM LOAD �� ������ 0.5�� �߰�
// IR Lo & Hi Average ������� ����
// IR Average Peak to Peak �� �߰�
// IR Lo + Hi Time �� ���� �ִ� 249 �ۿ� �ȳ����� ���� ����
// Triple ���� �˰����� ����
// Triple Detection Logic 3�� ��� �����ؾ� �����ǵ��� ����

// REV0.26 (2017-10-16 ���� 5:47:47)
// Calculated Log �� Tera Term Time Stamp �и��� ���� �޸� �߰�
// IrAvPtoP�� Max�� �߰�

// REV0.27 (2017-10-17 ���� 11:57:00)
// Warm-up �� UvSum, IrAvPtoPMax �� Ŭ���� (BIT Manual Mode���� �ʱⰪ �ݿ���)
// Triple Calculated Log Parameter ����
// BIT Mode �⺻�� Manual�� ����
// Triple KFI ���������� ���� ����

// REV0.28 (2017-10-27 ���� 10:44:28)
// BIT Manual Mode���� �ܺ��Է����� ���۵��� �ʴ� ���� ����
// IR Size Value Calculation Logic �߰�
// System Power(P24)�� 9V ���ϸ� EEPROM Write �ʵǵ��� ����

// REV0.28 (2017-10-27 ���� 10:44:28)
// IR Size Value Scale ���� (������ * 10)

// REV0.29 (2017-10-27 ���� 10:44:28)
// Triple Mode���� Raw Data Log �� Tera Term Time Stamp �и��� ���� �޸� �߰�
// �µ� �ʱⰪ ���� ���ϴ� ���� ����
// Average P.P. ���� 0�� ��� ���� ��ȣ�� ũ�Ⱑ ū���̸� 5.000[V]�� ó��

// REV0.30 (2017-11-09 ���� 12:46:16)
// Max. & Min. Temperature EEPROM Write Casting ���� (U16 -> S16)
// IR Relative Ratio Filtering ����
// IR Peak to Peak Limit�� ��� Size Limit�� ����

// REV0.31 (2017-11-17 ���� 5:52:48)
// IR Relative Ratio Limit Table ����
// EEP_TRIP_IR_SIZE_LMT Default ���� (60 -> 42)
// IrPtoPLmt ������ �� ������ Ÿ�� ���� (IrPtoPLmt -> IrSizeLmt, double -> U16)
// UVIR Calculated Mode Log���� AvPtoP & IrSize �� �߰�
// Triple 0x08 Filter �߰�
// Triple KFI ���Ľ��� �÷ῡ ���� ����

// REV0.32 (2017-12-08 ���� 4:47:01)
// ALARM_STATE_BIT_PROGRESS ���ۻ��� �߰�
// MODBUS Input Register�� DeviceType �߰�
// Triple Mode���� UV Alarm Decision ���� �ʵ��� ����

// REV0.33 (2017-12-26 ���� 3:05:47)
// ������� Calculated Log. Parameter �߰�
// Node Address DIP S/W �Է� ���� ( All Off  = 1 -> 0, ����: 1 ~ 31 )
// Triple Mode���� �˶��۵� �ȵǴ� ���� ����
// Alarm Latch DIP S/W�� ���õǵ��� ���� (Holding Register ����)
// Detection Distance DIP S/W �߰� (�Ÿ��� ���������� �������� �� �˰����� �Ϸ� �� ���࿹��)
// Calculated Log. Mode�� BIT DECISION ����� BIT�� ù ��° ����Ŭ ����� �������� ", B"�� ǥ�õǵ��� ����
// IR���� MODBUS Mapping ���� (Input & Hoding)

// REV0.34 (2018-01-24 ���� 9:31:25)
// UVIR Mode���� IR Sensor�� Responsivity �Է� �� Calibration Parameter�� �ڵ� ���Ǿ� ����Ǵ� Logic �߰�
// IR Sensor Responsivity �Է��� ���� Register �߰� (EEP_CAL_BUF_IR430_RESPONSIVITY)
// IR_Rel_Ratio_Limit_Table ����
// UVIR Mode���� IR Filter 0x04�� ���ǿ��� ����

// REV0.35 (2018-02-21 ���� 8:49:26)
// Input Register Map ����
// MODBUS �������� �� Ÿ�Ӿƿ�ó�� (Packet Check �Ϸ� �� 60ms �̳��� Packet Send ���� ���� ���)
// MODBUS baud rate DIP S/W(PIN6)�� ���õǵ��� ���� ( ON = 115200bps, OFF = 9600bps)
// Triple KFI ���Ľ��� �÷ῡ ���� ���� (�Ż�� ������ 2018-02-23 ���� 3:55:21)

// REV0.36 (2018-02-28 ���� 8:55:05)
// RS-485 packet error rate ���� (baud rate, packet size ����)
// RS-485 Packet Error Count MODBUS Input Register�� �߰�
// 30006: Rx Packet ���̰� 8byte�����̸� ����
// 30007: Packet ���� �� 50ms �̳��� �������� ���� ��� ����
// MODBUS �������� �� Ÿ�Ӿƿ� �ֱ� ���� (60ms -> 50ms)

// REV0.37 (2018-02-28 ���� 11:59:33)
// MODBUS Input Register Map ����
// MODBUS �������� �� Ÿ�Ӿƿ� �ֱ� ���� (50ms -> 56ms)

// REV0.38 (2018-03-15 ���� 7:32:21)
// UV Calibration Logic �߰�

// REV0.39 (2018-03-27 ���� 9:16:03)
// UV CpmRaw Value �߰�
// IR Calibration Point Ȯ�� ( 5 -> 7 point )
// IR430_Calibration() ���� �� ������ ����

// REV0.40 (2018-03-29 ���� 8:01:34)
// Serial Scope Mode ������ ���� ��� ���� ( �ʴ� 1ȸ, �ʴ� 250ȸ )
// IrLoTime + IrHiTime ���� 250�� �Ѵ� ���� ����

// REV0.41 (2018-04-10 ���� 7:36:06)
// UVraw[n][0] ���� '0'�� �Ǵ� ���� ����

// REV0.42 (2018-04-17 ���� 11:23:01)
// UV Filtering Logic ������ �����ͷ� ����
// labserve ����� ��� (è�� �� ��Ȥ���� -40 ~ 80)

// REV0.43 (2018-04-18 ���� 4:09:16)
// Intro Message�� IR Responsivity �߰� (GTF-1100U Only)

// REV0.44 (2018-05-21 ���� 4:57:02)
// ��ǰ �ø���ѹ� ���� (40004~40005, 2-WORD)
// IR_FLICKER_FACTOR ���� (0.0007 -> 0.0005)
// IR Zero Frequency Filtering ����
// Warning ���� ���� (���� Detection Count ���� Ŭ ��츸)

// REV0.45 (2018-05-23 ���� 2:11:18)
// Triple Balance Filtering ���� �и�
// Holding Register �Է¹��� ����
// IR Calculation Logic ����
// IR Calculation ó���ð� ������ ���� Modbus Timeout Check �ð� ����
// Filtering �˰����� �߰� �� ����
// Alarm Decision Logic ����
// Flame State ���� �� �����ð� ���� (3��)

// REV0.46 (2018-05-30 ���� 5:06:22)
// Calculated Log Mode ��µ����� ����
// Input Register Map ����
// MODBUS_Write_Multiple_Register_Slave() CRC �߰�
// Modbus Address�� '0'�̸� '1'�� ���� (DIP S/W �ʱ���¿��� �����ϱ� ����)
// BIT ���� �� Ir1sPtoP�� ������ ���

// REV0.47 (2018-05-31 ���� 7:22:12)
// Calibration Voltage Limit ����

// REV0.48 (2018-06-01 ���� 9:41:15)
// ȭ��˶� �����ð� ���� (3�� -> 5��)
// Modbus ID Check ���� ����

// REV0.49 (2018-06-08 ���� 3:55:26)
// Relative Ratio Limit ���� (Relative_Response_Ratio_20180604.xlsm ����)
// IR & UV Calibration Limit ����
// EEP_SysFormat()���� �ø���ѹ� & ������ ����
// IR Offset Drift Check �߰�

// REV0.50 (2018-06-11 ���� 5:39:14)
// UV Zero Count Deviation Limit Filter �߰�
// Fault ���� ������� ����
// Power Fault Check �����ð� ���� (5sec -> 60sec)
// EEPROM Write Protection Voltage ���� (9V -> 12V)

// REV0.51 (2018-06-13 ���� 4:47:02)
// EEPROM UV Detection Limit Scale ���� (x10 -> x1000)
// EEP_CAL_VAL_UV_CPS ���� (200 -> 85)
// EEP_CAL_BUF_UV_CPS LIMIT ����
// UV & IR CAL_VAL EEPROM FORMAT ���� �ּҿ��� ����
// EEP_UVIR_IR_SIZE_LMT ���� (34 -> 38)
// Power Fault Check �����ð� ���� (60sec -> 5sec)

// REV0.52 (2018-06-13 ���� 4:47:02)
// 1���� �� ������ ���� Relative Ratio Limit ���� (Relative_Response_Ratio_20180615.xlsm ����)
// Triple IR Time Deviation Limit ����

// REV0.53 (2018-08-27 ���� 11:16:35)
// IR Offset Drift Check Logic ���� (FM ȯ����� 80������ Offset Drift Fault ���� -> Ir1sAvrg �� 2.7[V])
// ���� ������ذ� ���������� �µ���ȭ�� ���� ������ ����

// REV0.54 (2018-10-24 ���� 10:57:25)
// EEPROM CRC EVENT���� ����
// Power Fault ���� ���� (18[V]�̸� or >32[V]�ʰ� -> 17[V]�̸� or >33[V]�ʰ�)
// System Fault Current Output Level ����
// EEPROM ������� ���� ���� ���� (12[V]���� -> 17[V]����)

// REV0.55 (2019-08-07 ���� 03:40:25)
// �׽�Ʈ ����
// limit OffSet 3.95, 5.3 �и�
// Offset���� ������ ���� 99.8 Max�� ���� ũ�� 99.8�� ������ ����

// Rev 0.54 2021-05-11
// 1. 수소 불꽃 감지를 위한 Sensor 변경 (4.66 -> 2.7)
// 2. Sensor 변경으로 인한 IR 계산 및 Filter 변경 (Demo 시연 용)

// Rev 0.543 2021-05-24
// 1. Sub Version 2에서 3으로 변경 (Non-Hydrocarbon Type 추가)
// 2. Flame Type 추가 (Hydrocarbon, non-Hydrocarbon)
// 3. EEP Data에 non-hydrocarbon 관련 Data 추가 (EEP MAX 215->220)
// 4. InputReg에 non-hydrocarbon 관련 Data 추가
// 5. Flame type에 따라 Ir 파라미터 값 계산 및 Flame Filter 구분
// 6. Triple-IR BIT 불량시료 증가(530 감도 낮음), IR Source On Time 20ms 증가. (기존 50ms -> 변경 70ms)

// Rev 0.543 2021-08-27
// 1. Sub Version 3 유지
// 2. non-hydrocarbon IR 및 UV 알고리즘 변경
// 3. Filtering 조건 변경
// 4. 수소모델 FM 인증 및 말레이시아 2ea 전달

// Rev 0.60 2021-10-22
// 1. 수소모델 Version 정보 변경
//	- Rev0.60 / Sub 0
// 2. EEPROM non-hydrocarbon 관련 Data 뒤쪽으로 이동
//  - 기존 U모델 감지기와 호환성
// 3. 입고된 Sensor의 Responsivity가 너무 낮아 Blackbody 교정으로 변경 (2022.03.08)
//  - 변경으로 인해 IR slope 계산식 변경 
// 4. 부동작 IR size 와 수소 IR size Limit을 다르게 하기 위해 UV Filter 추가 (2022.04.05)
//  - UV Itg가 3.5 이하일 경우 IR Size Limit 80

// Rev 0.601 2022-05-31
// 1. 수소 모델 PP 최종 시험 이후 Sub version 1로 관리
// 2. EEPROM_MARKING 0xDC94
// 3. 기존 모델도 blackbody로 교정 진행. IrSpanVal eep_sm에서 read


/* ****************************************************************************************** 
Rev 0.602 2025-09-11 (jmyoo)

1. FlameDetDelay_Sec 추가 -> 감지시간 조정 (0 ~ 20 sec)
2. EEPROM_MARKING         -> 0xDC94+1 (유지하고 eeprom 구조 변경으로 수정)
3. eep address 추가       -> Rel_ratio_395, Rel_ratio_530 (모드버스로 read/write)
						  -> Triple rel_ratio_limit 테이블 개별 설정하기 위해
						  

4. eeprom EEP_CAL_VAL_IR430_01 ~ EEP_CAL_VAL_IR530_07 변경 (Blackbody 교정 값)
5. eeprom 구조 변경 // 2025-09-25
 - 기존 CRC, PREFIX 구조 유지
 - DelayTime까지 CRC-2, PREFIX-2 추가
 - Ratio Limit CRC-ratio, PREFIX-ratio 추가
 - write 할때 나눈 구조 확인 -> EEP_Block_Check() 추가
 - Ratio값 eeprom 에서 바로 Read하여 CRC -> 기존 구조와 달라 Ratio_CRC16() 추가
 - CRC write, CRC check 나눈 구조마다 따로 진행
 
 6. Mudbus User Function code 추가 // 2025-10-01
 - 0x41 Ratio write code
 - Ratio Limit Data 를 쓰기 위해 통신 중 eeprom write에서 Timeout 발생 (100ms)
 - 0x41 Function code 에서는 Timeout 관련 Timer 미설정
 - Paket Decoder -> MODBUS_Write_Multiple_Register_Slave()에 0x41 추가
****************************************************************************************** */