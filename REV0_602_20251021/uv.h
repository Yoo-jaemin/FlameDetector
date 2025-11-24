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

#ifndef __uv_H__
#define __uv_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mmi.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UV_BUF_SIZE							25

#define UV_ITG_FACTOR						(0.5f)

#define UV_DET_LIMIT						(1.2f)
#define UV_ZERO_DEV_CNT_LMT					8
#define UV_ZERO_CNT_LMT						1
#define UV_CAL_SLOPE_LMT					8 // 0.8

#define UV_ZERO_CNT_LMT_CPS_REF				63	
#define UV_ROC_LMT							105 
#define UV_ROC_LMT_CPS_REF					24
#define UV_ZERO_CNT_DEV_LMT					10

// non-hydrocarbon
#define UV_DET_LIMIT_ON_IR_SIZE_LMT_CHANGE	(3.5f)
#define UV_ZERO_CNT_LMT_CPS_REF_NHC			150	
#define UV_ZERO_CNT_DEV_LMT_NHC				9
#define FREQ_ADD_CNT_LMT_IN_UVIR_VALANCE	10
#define FREQ_ADD_CNT_LMT_IN_UV_ZERO_CNT		5
#define IR_DIR_CNT_LMT_IN_UV_ZERO_CNT		4
#define UV_ROC_LMT_CPS_REF_NHC				26
#define UV_ROC_LMT_NHC						70


// #define IR_FREQ_SUM_LMT_IN_UVCPS_ROC		3


#define UV_POWER_RET_TIME					5 // sec.

#define UV_FSO_TIME							2 // sec. (Non-flame retention time for first signal override)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern double	UvDetLmt;
extern double	CpsItgMax;
extern double	CpsCalSlope;
extern U16		CpsRawMax;
extern U16		UvRaw[2][UV_BUF_SIZE], UvRawIndex;
extern U16		UvDetCnt, UvWarCnt, UvSum, UvBitLmt;
extern U8		UvNonDetCnt, UvPwrCtrlFlag;
extern U16		UvZrnDvCntLmt, UvZrLmtCpsRef, UvZrCntLmt, UvRocLmtCpsRef, UvRocLmt, UvZrCntDvLmt;

extern double	UvDetLmtOnIrSizeLmtChange;
extern U16		UvZrLmtCpsRef_NHC, UvZrCntDvLmt_NCH, UvCpsLmtInIrSizeDvPer, UvRocLmtCpsRef_NHC, UvRocLmt_NHC;; 
extern U16		FreqAddCntLmtInUvIrValance, FreqAddCntLmtInUvZrCnt, IrDirCntLmtInUvZrCnt;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern void Initialize_UV( void );
extern void UV_Calculation( U16 *pUvBuf, FLAME_HandleTypeDef *flame );
extern void UV_Detection( U8 index );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __uv_H__
