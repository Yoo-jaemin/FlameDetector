
//*****************************************************************************
// Author : jmyoo
//*****************************************************************************

/*
[v0.1]
UV, IR 캘리브레이션 통합 지그 펌웨어 개발!
- UV, IR 센서 통합 캘리브레이션 개선 -> IR 캘리브레이션 시간 단축!
- 운영 중, MODBUS RTU 통신에러 처리! -> 코드 재설계
- 불 필요한 메모리 제거, RAM 용량 확보!
- MODBUS MAP 변경 -> RATIO LIMIT 추가됨
- TRIPLE RATIO LIMIT Write 기능 지원
- LCD UI 변경

2025-10-24 [v0.1]
- IR,UV 캘리브레이션 검증 완료 

2025-11-17 [v0.2]
- 감지기 FW v0601, v0602 모두 지원 

2025-11-18 [v0.3]
- IR 캘리브레이션 오동작 개선 -> 플래시 영역(RATIO LMT) 축소
- BIT 테스트 모드 지원 -> TE short key
*/


#define	_FW_VERSION_MA		0
#define	_FW_VERSION_MI		3








/*

**** [ History ] ****

Rev 0.26 2023-11-21
1. IR Sensor Offset Fault State
2. GTF-1100T Commnication_Default_Set (if condition add)
   Det_Holding_Reg_Data[DET_EEP_CAL_BUF_IR430_01] == 0 or ffff -> IR 3
3. if) reg_Data != reg_data_old : add LCD Clear all 
4. modified ModeSetFlow[FMODE_DET_TEST_MODE] 

Rev 0.25 2023-03-02
1. LED_Indicator Function
   - Det_Analog_Data Lmt modify (3%)

///////////////////////////////////////////////////////////////   
Fuse Bits	Extended:0xFD	High:0xD1	Low:0xCE
Lock Bits	0xFF   
///////////////////////////////////////////////////////////////   
    
// Rev 0.06 2019-05-28 ���� 8:50:16
1. Data Memory�� ���̱� ���� Display �κ� ����
2. �� ��� ���� �κ� ���� ����

// Rev 0.05 2019-05-24 ���� 4:21:21
1. Normal Mode
   BIT Flag, UV Power Control Flag Ȯ�� �߰�
   Detector Set���� BIT. UV Power Control, Log Mode ���� �����ϵ��� ��� �߰�
2. U Calibration Mode �߰�
   Calibration���۽� UV Power Control OFF�� �Ͽ� 300V�� ��� �ΰ��ǵ��� ����
   70sec �� UV CPM���� �޾Ƽ� ���� ����
   ���� �Ϸ� �� Fault �߻��� UV Power Control�� �⺻����(ON)���� ����
   ��� ���� �Ϸ�

// Rev 0.04 2019-04-16 ���� 11:26:52
1. Detector�� EEPROM ������� ���� �����ϵ��� ����
2. BIT, Relay Test ����� �������� ����

// Rev 0.03 2019-03-25 ���� 5:04:35
1. Test Mode, 4~20mA Calibration Mode ����� Detector�� EEPROM�� �����Ͽ� �����ϴ� ����� F/W

// Rev 0.02 2019-03-15 ���� 11:45:34
1. Dev�� Det���� 485��� 1��
2. Det EEPROM CLEAR�� Multi Write Func�� Single Write�� Det EEP_CLEAR_MODE�� Set�Ͽ� Det���� Clear�ϵ��� ����
3. Det 4~20mA Calibration �� Det Test��ɵ� Single Write�� �̿��Ͽ� EEP_CALIBRATION_MODE����
4. Det mA Calibration ���� �ڵ����� AutoTest �����ϵ��� ����

// Rev 0.01	2019-03-07 ���� 9:43:58
1. Dev�� Det���� 485��� 2�� �������� ����
2. Write ��ȣ �Է½� Holding �Է� ���� Write ����

// Rev 0.00	2018-11-21 ���� 9:36:04
1. AIC-100 PCB�� �̿��Ͽ� ���� ����
2. Timer Setting
3. LCD Setting

*/