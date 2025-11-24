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

#ifndef __ir_H__
#define __ir_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mmi.h"
#include "adc.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAL_TEST

#define IR_BUF_SIZE							250

#define IR_395_OFFSET_LMT					945
#define IR_530_OFFSET_LMT					975

#define SPECTRUM_POSI_C						0
#define SPECTRUM_POSI_L						1
#define SPECTRUM_POSI_H						2

// IR Data Direction Check
#define IR_DIR_INIT							0
#define IR_DIR_UP							1
#define IR_DIR_DOWN							2
#define IR_DIR_DET_LMT						2
#define IR_DIR_CNT_LMT						2

#define IR_PULSE_WIDTH_LMT					7

#define IR_LOW_SIZE_LMT_CPS_REF				30 
#define IR_LOW_SIZE_LMT						100

// non-hydrocarbon Filter
#define UVIR_SIZE_LMT_NHC					43
#define UVIR_SIZE_LMT_NON_OPER_FLAME		80
#define IR_FREQ_SUM_LMT						3
#define IR_DIR_CNT_SUM_LMT					2
#define IR_TIME_DV_SUM_LMT					5
#define IR_TIME_DV_SUM_LMT_IN_CNT_ZERO		3
#define IR_TIME_DV_LMT_IN_FREQ_SAME_NHC		5  // non-hydrocarbon
#define IR_LOW_SIZE_LMT_CPS_REF_NHC			200 // non-hydrocarbon
#define IR_LOW_SIZE_LMT_NHC					300 // non-hydrocarbon
#define IR_SIZE_LMT_IN_IR_SIZE_DV_PER		900
#define IR_FREQ_ADD_CNT_LMT					4
#define IR_DIR_CNT_LMT_IN_TIME_SAME			2
#define IR_SIZE_DV_PER_LMT					0.05f
#define IR_PTOP_DV_PER_LMT					0.05f
#define UV_CPS_LMT_IN_IR_SIZE_DV_PER		130
#define IR_PtoP_SATURATION					4.995f

//#define IR_FREQ_DV_LMT_IN_WIDEST_TIME_DV	2
//#define IR_WIDEST_TIME_DV_LMT_IN_FREQ_DV	3

#define IR_FLICKER_FACTOR					0.0008f

#define IR_TIME_DV_LMT_IN_FREQ_SAME			10 // 10 * 4 = 40ms

#define IR_FSO_TIME							3 // sec. (Non-flame retention time for first signal override)

#define IR_CAL_POINT_MAX					7

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern U16		IrRaw[2][IR_WL_MAX][IR_BUF_SIZE];
extern U16		IrRawIndex;
extern U16		IrDetCnt, IrWarCnt;
extern U16		IrHiLmt, IrLoLmt, IrDebTime;
extern double	IrFlickerFactor, IrBitLmt, Ir1sAvrgDvOverLmt;

//non-hydrocarbon Ãß°¡
extern U16		IrSizeLmt_NHC, IrSizeLmtOnNonOperFlame;
extern U16		IrFreqSumLmt, IrDirCntSumLmt, IrTimeDvSumLmt, IrTimeDvSumLmtInCntZr;
extern U16		IrLoSizeLmtCpsRef_NHC, IrLoSizeLmt_NHC, IrSizeLmtInSizeDvPer;
extern U16		IrFreqAddCntLmt, IrDirCntLmtInTimeSame, IrTimeDvLmtInFreqSame_NHC;
extern double	IrSizeDvPerLmt, IrPtoPDvPerLmt;
//

extern U16		IrSizeLmt; 
extern U8		IrTimeDvLmtInFreqSame;
extern double	Ir1sAvDvLmtInFreqSame;
extern U16		IrSizeDvLmtInFreqSame;
extern U16		IrLoSizeLmtCpsRef, IrFlickerCpsRefLmt;
extern U16		IrLoSizeLmt;
extern U8		IrNonDetCnt[IR_WL_MAX];
extern double	IrAvPtoPMax;
extern double	IrSpanBuf[IR_WL_MAX][IR_CAL_POINT_MAX], IrSpanVal[IR_WL_MAX][IR_CAL_POINT_MAX], IrSlope[IR_WL_MAX][IR_CAL_POINT_MAX];
extern U16		IrScale[IR_WL_MAX];
extern S8		IrRelRatioCorrVal_395,IrRelRatioCorrVal_530;
extern U16		IrSizeRocLmt;
extern U16		IrLoDriftChkTick[IR_WL_MAX], IrHiDriftChkTick[IR_WL_MAX];
extern U16		TripModeIrTimeDvLmt;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_IR( void );
extern void IR_Calculation( U16 *pIrBuf, FLAME_HandleTypeDef *flame );
extern void IR_Detection( U8 index );
extern void IR_Slope_Calculator( U8 wl, U8 n );
extern void IR430_Calibration( S16 Responsivity );

#endif // __ir_H__
