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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include<stdio.h>
#include<string.h>

#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "dbg_printf.h"

#include "adc.h"
#include "gpio.h"
#include "ir.h"
#include "uv.h"
#include "mmi.h"
#include "eep.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double	UvDetLmt;
double	CpsItgBuf, CpsItgMax;
double	CpsCalSlope;
U16		CpsRawMax, CpsRawBuf[60], CpsRawIndex;
U16		UvRaw[2][UV_BUF_SIZE], UvRawIndex;
U16		UvDetCnt, UvWarCnt, UvSum, UvBitLmt;
U8		UvNonDetCnt, UvPwrCtrlFlag;
U16		UvZrnDvCntLmt, UvZrLmtCpsRef, UvZrCntLmt, UvRocLmtCpsRef, UvRocLmt, UvZrCntDvLmt;

// non-hydrocarbon
double	UvDetLmtOnIrSizeLmtChange;
U16		UvZrLmtCpsRef_NHC, UvZrCntDvLmt_NCH, UvCpsLmtInIrSizeDvPer, UvRocLmtCpsRef_NHC, UvRocLmt_NHC;
U16		FreqAddCntLmtInUvIrValance, FreqAddCntLmtInUvZrCnt, IrDirCntLmtInUvZrCnt;
//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_UV( void );
void UV_Calculation( U16 *pUvBuf, FLAME_HandleTypeDef *flame );
void UV_Detection( U8 index );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_UV( void )
{
	CpsItgBuf = 0;
	CpsItgMax = ((double)epr_sm[EEP_EVT_CPS_ITG_MAX] / 10);
	CpsRawMax = epr_sm[EEP_EVT_CPS_RAW_MAX];
	CpsCalSlope = ((double)epr_sm[EEP_CAL_VAL_UV_CPS] / (double)epr_sm[EEP_CAL_BUF_UV_CPS]);
	
	memset( CpsRawBuf, 0, sizeof(CpsRawBuf) );
	CpsRawIndex = 0;
	
	UvRawIndex = 0;
	
	UvWarCnt = 0;
	UvDetCnt = 0;
	UvSum = 0;
	UvNonDetCnt = 0xFF;
	
	UvPwrCtrlFlag = epr_sm[EEP_UV_PWR_CTRL];
	if( !UvPwrCtrlFlag ){
		SMPS_ON();
	}
	UvDetLmt = ((double)epr_sm[EEP_UV_DET_LMT] / 1000 );
	UvZrnDvCntLmt = epr_sm[EEP_UV_ZERO_DEV_CNT_LMT];
	
	UvZrLmtCpsRef = epr_sm[EEP_UV_ZERO_CNT_LMT_CPS_REF];
	UvRocLmtCpsRef = epr_sm[EEP_UV_ROC_LMT_CPS_REF];
	UvRocLmt = epr_sm[EEP_UV_ROC_LMT];
	
	// non-hydrocarbon
	UvDetLmtOnIrSizeLmtChange = ((double)epr_sm[EEP_UV_DET_LMT_ON_IR_SIZE_LMT_CHANGE] / 1000 );
	UvCpsLmtInIrSizeDvPer = epr_sm[EEP_UV_CPS_LMT_IN_IR_SIZE_DV_PER];
	UvZrLmtCpsRef_NHC = epr_sm[EEP_UV_ZERO_CNT_LMT_CPS_REF_NHC];
	UvZrCntDvLmt_NCH = epr_sm[EEP_UV_ZERO_CNT_DEV_LMT_NHC];
	FreqAddCntLmtInUvIrValance = epr_sm[EEP_FREQ_ADD_CNT_LMT_IN_UVIR_VALANCE];
	FreqAddCntLmtInUvZrCnt = epr_sm[EEP_FREQ_ADD_CNT_LMT_IN_UV_ZERO_CNT];
	IrDirCntLmtInUvZrCnt = epr_sm[EEP_IR_DIR_CNT_LMT_IN_UV_ZERO_CNT];
	UvRocLmtCpsRef_NHC = epr_sm[EEP_UV_ROC_LMT_CPS_REF_NHC];
	UvRocLmt_NHC = epr_sm[EEP_UV_ROC_LMT_NHC];
	//
		
	UvZrCntLmt = epr_sm[EEP_UV_ZERO_CNT_LMT];
	UvZrCntDvLmt = epr_sm[EEP_UV_ZERO_CNT_DEV_LMT];
	UvBitLmt = epr_sm[EEP_UV_BIT_THRESHOLD_CPS];
	
	if( DeviceType == DTYPE_UVIR ){
		TCCR1A = 0x00;
		TCCR1B = 0x06;
		TCNT1 = 0;
		
		DPRINT( "\n\n [ UV ]\n" );
		DPRINT1( "\n DET: %.3f[CPS]", UvDetLmt );
		DPRINT1( "\n ZR&DV CNT DV: %u" , UvZrnDvCntLmt );
		DPRINT2( "\n ZR: %u[CPS], %u", UvZrLmtCpsRef, UvZrCntLmt );
		DPRINT2( "\n ROC: %u[CPS], %u", UvRocLmtCpsRef, UvRocLmt );
		DPRINT1( "\n ZR DV: %u", UvZrCntDvLmt );
		DPRINT1( "\n BIT: %u[CPS]\n\n", UvBitLmt );
	}
	else {
		TCCR1A = 0;
		TCCR1B = 0;
		TCNT1 = 0;
		DPRINT( "\n" );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UV_Calculation( U16 *pUvBuf, FLAME_HandleTypeDef *flame )
{
	FLAME_HandleTypeDef *fp;
	double	max, min, res;
	U16		i, sum;
	S8		dv;
	U32		cpm;
	
	if( flame == &Flame[0] )	fp = &Flame[FLAME_BUFF_SIZE-1];		// fp 직전 인덱스
	else						fp = flame-1;
	
	sum = 0;
	res = 0;
	max = 0;
	
	flame->UvZrCnt = 0;
	flame->UvDvCnt = 0;
	
	for( i=0; i<UV_BUF_SIZE; i++ ){
		sum += *(pUvBuf+i);
		if( *(pUvBuf+i) > max ){
			max = *(pUvBuf+i);
		}
		//CpsItgBuf = CpsItgBuf * ( 1.f - UV_ITG_FACTOR ) + *(pUvBuf+i) * UV_ITG_FACTOR;
	}
	res = sum * CpsCalSlope;
	CpsItgBuf = CpsItgBuf * ( 1.f - UV_ITG_FACTOR ) + res * UV_ITG_FACTOR;	// IIR(지수형 저역통과 필터, Exponential IIR Filter) 계산식
	
	flame->UvRawMax = max;
	max *= 0.7;
	
	for( i=0; i<UV_BUF_SIZE; i++ ){
		if( *(pUvBuf+i) == 0 ){
			flame->UvZrCnt++;		// flicker
		}
		if( *(pUvBuf+i) < max ){
			flame->UvDvCnt++;		// uv 세기 불안정
		}
	}
	
	dv = flame->UvZrCnt - fp->UvZrCnt;
	flame->UvZrCntDv = abs( dv );
	
	dv = flame->UvDvCnt - fp->UvDvCnt;
	flame->UvDvCntDv = abs( dv );
	
	flame->UvCpsRaw = sum;
	flame->UvCpsRes = res;			// 보정된 cps
	flame->UvCpsItg = CpsItgBuf;	// 적산(IIR)
	
	if( BIT_Status == BIT_STATE_IDLE ){
		CpsRawBuf[CpsRawIndex] = sum;
	}
	
	for( cpm=0, i=0; i<60; i++ ){
		cpm += CpsRawBuf[i];
	}
	flame->UvCpmRaw = cpm;
	
	if( CpsRawIndex < 59 ){
		CpsRawIndex++;
	}
	else {
		CpsRawIndex = 0;
	}
	
	if( flame->UvCpsRaw > fp->UvCpsRaw ){
		min = fp->UvCpsRaw;
		max = flame->UvCpsRaw;
	}
	else {
		min = flame->UvCpsRaw;
		max = fp->UvCpsRaw;
	}
	
	if( min == 0 ){
		flame->UvCpsRoc = 0;	// 프레임간 변화율 
	}
	else {
		flame->UvCpsRoc = (U16)(fabs(( min - max ) / min * 100 ));
		if( flame->UvCpsRoc > 999 ){
			flame->UvCpsRoc = 999;
		}
	}
	
	if(( fp->UvCpsRaw == 0 ) && ( flame->UvCpsRaw == 0 ) && ( BIT_Status == BIT_STATE_IDLE )){
		Flame[0].UvCpsRaw = 0;
		Flame[1].UvCpsRaw = 0;
		Flame[2].UvCpsRaw = 0;
		Flame[0].UvCpsItg = 0;
		Flame[1].UvCpsItg = 0;
		Flame[2].UvCpsItg = 0;
		Flame[0].UvCpsRes = 0;
		Flame[1].UvCpsRes = 0;
		Flame[2].UvCpsRes = 0;
		CpsItgBuf = 0;
	}
	
	if( !flame->UvCpsRaw ){				// 무검출
		if( UvNonDetCnt != 0xFF ){
			UvNonDetCnt++;
		}
	}
	else {								// 검출
		if(( UvNonDetCnt >= UV_FSO_TIME ) || ( BIT_Status != BIT_STATE_IDLE )){
			flame->UvCpsItg = 0;
			flame->UvCpsRes = 0;
			CpsItgBuf = 0;
		}
		if( BIT_Status == BIT_STATE_IDLE ){
			UvNonDetCnt=0;
		}
	}
	
	if( BIT_Status == BIT_STATE_IDLE ){
		if( sum > CpsRawMax ){
			CpsRawMax = sum;
			EEP_Sys_Wr( EEP_EVT_CPS_RAW_MAX, CpsRawMax );
		}
		if( CpsItgBuf > CpsItgMax ){
			CpsItgMax = CpsItgBuf;
			EEP_Sys_Wr( EEP_EVT_CPS_ITG_MAX, (S16)(CpsItgMax * 10) );
		}
	}
	
	if(( UvSum + sum ) < 0xFFFF ){
		UvSum += sum;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UV_Detection( U8 index )
{
	U16		i;
	U8		bFlag = 0, Cnt;
	
#if 0
	U16		iPrev;
	
	if( index == 0 ){
		iPrev = FLAME_BUFF_SIZE-1;
	}
	else {
		iPrev = index-1;
	}
#endif
	
	if( Flame[index].UvCpsItg < UvDetLmt )		// 적산값, uv 신호약함
		bFlag |= 0x80;

	if(FlameType == NON_HYDROCARBON)
	{	
		if( ( Flame[index].UvZrCntDv > UvZrnDvCntLmt ) && ( Flame[index].UvDvCntDv > UvZrnDvCntLmt ) &&		// 0x40: UV의 변동(깜빡임/출렁임)이 크고, IR 주기성은 전혀 안 잡힘
			( ( Flame[index].IrLoTime[IR_WL_430] == 0 ) || ( Flame[index].IrHiTime[IR_WL_430] == 0 ) || 
			 ( Flame[index].IrTimeDvSum[IR_WL_430] == 0 ) ) ) { 
			bFlag |= 0x40;
		}
		
		if( ( Flame[index].UvCpsRaw > UvZrLmtCpsRef_NHC ) && ( Flame[index].UvZrCnt > UvZrCntLmt ) &&		// 0x20: UV 원시강도 너무크고, 깜빡거림, IR의 주파수/방향 변화 합계는 작다
			( Flame[index].IrFreqDirCntSum[IR_WL_430] < FreqAddCntLmtInUvZrCnt ) ) {
			bFlag |= 0x20;
		}
		
		if( Flame[index].UvCpsRaw > IrLoSizeLmtCpsRef_NHC ){	// 0x10: UV 강도 너무크고, IR 크기는 작고 IR 변화도 거의 없다
			if( ( Flame[index].IrSize[IR_WL_430] < IrLoSizeLmt_NHC ) &&
			( Flame[index].IrFreqDirCntSum[IR_WL_430] < FreqAddCntLmtInUvIrValance ) ){
				bFlag |= 0x10;
			}
		}
		
		if( ( Flame[index].UvZrCntDv > UvZrCntDvLmt_NCH ) &&	// 0x08: UV 변동(0카운트 변화량)은 큰데 IR 주기성/변동 합이 전부 0
			( Flame[index].IrLoTime[IR_WL_430] == 0 ) && ( Flame[index].IrHiTime[IR_WL_430] == 0 ) &&
			( Flame[index].IrTimeDvSum[IR_WL_430] == 0 ) ) {
			bFlag |= 0x08;
		}
		
		if( Flame[index].UvCpsRaw > UvRocLmtCpsRef_NHC ){		// 0x04: UV 강도 기준 이상이고, UV 변화율(ROC)도 큰데 IR 방향변화 합은 0
			if( ( Flame[index].UvCpsRoc > UvRocLmt_NHC ) && ( Flame[index].IrDirCntSum[IR_WL_430] == 0 ) ){
				bFlag |= 0x04;
			}
		}
	
	}
	else 
	{
		
		if(( Flame[index].UvZrCntDv > UvZrnDvCntLmt ) && ( Flame[index].UvDvCntDv > UvZrnDvCntLmt )){	// 0x40: UV 변동 두 지표가 모두 큼
			bFlag |= 0x40;
		}
		
		if(( Flame[index].UvCpsRes > UvZrLmtCpsRef ) && ( Flame[index].UvZrCnt > UvZrCntLmt )){			// 깜빡임 많은 강한 UV
			bFlag |= 0x20;
		}
		
		if( Flame[index].UvCpsRes > UvRocLmtCpsRef ){	// 0x10: UV 보정강도 크고, 변화율(ROC)도 큼
			if( Flame[index].UvCpsRoc > UvRocLmt ){
				bFlag |= 0x10;
			}
		}
		
		if( Flame[index].UvZrCntDv > UvZrCntDvLmt ){	// 깜빡임 변화량이 너무 크다
			bFlag |= 0x08;
		}	
	}
			
	if( mmi_mode == FMODE_WARM_UP ){
		Flame[index].UvFtFlag = 0xFF;
	}
	else {
		Flame[index].UvFtFlag = bFlag;
	}
	
	for( i=0, Cnt=0; i<3; i++ ){
		if( Flame[i].UvFtFlag == 0 ){
			Cnt++;
		}
	}
	Flame[index].UvCnt = Cnt;
}
