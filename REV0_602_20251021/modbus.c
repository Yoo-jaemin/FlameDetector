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

#include<stdio.h>
#include<string.h>

#include <avr/io.h>
#include<avr/pgmspace.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "global.h"
#include "dbg_printf.h"

#include "modbus.h"
#include "uart1281.h"
#include "gpio.h"
#include "sw_lib.h"
#include "mmi.h"
#include "ir.h"
#include "uv.h"
#include "eep.h"
#include "version.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const U16 wCRCTable[] = {
   0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
   0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
   0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
   0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
   0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
   0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
   0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
   0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
   0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
   0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
   0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
   0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
   0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
   0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
   0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
   0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
   0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
   0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
   0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
   0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
   0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
   0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
   0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
   0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
   0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
   0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
   0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
   0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
   0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
   0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
   0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
   0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16	Modbus_InputReg[MAX_INPUT_REGISTER];
U32	Modbus_Baud;
U8	Modbus_Addr;
U8	Modbus_Init;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void MODBUS_Read_Input_Register_Slave( U8 *rxbuf );
//void MODBUS_Read_Holding_Register_Slave( U8 *rxbuf );
//void MODBUS_Write_Single_Register_Slave( U8 *rxbuf );
//void MODBUS_Write_Multiple_Register_Slave( U8 *rxbuf );

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

U16 CRC16( const U8 *nData, U16 wLength )
{
	U8 nTemp;
	U16 wCRCWord = 0xFFFF;
	
	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord  ^= wCRCTable[nTemp];
	}
	return wCRCWord;
}

U16 Ratio_CRC16( void )
{
	U8 nTemp;
	U16 wCRCWord = 0xFFFF;
	U16 wStart_add, wEnd_add, wData;
	
	//wStart_add = EEP_PREFIX_1;
	//wEnd_add = EEP_EVT_BIT_IR430_1S_PTOP_MIN;
	
	wStart_add = EEP_RATIO_PREFIX;
	wEnd_add = TRIPLE_IR_REL_RATIO_MAX;
		
	for (; wStart_add < wEnd_add; wStart_add++)
	{
		wData = EEP_Rd(wStart_add);
		nTemp = (U8)wData ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord  ^= wCRCTable[nTemp];
		nTemp = (U8)(wData >> 8) ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord  ^= wCRCTable[nTemp];
	}
	
	return wCRCWord;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Initialize_MODBUS( void )
{
	U16		i, cnt;
	U8		dat, buf, cop;
	
	Modbus_Addr = 0;
	Modbus_Init = 0;
	
	for( i=0, cnt=0, buf=0xff; i<0xffff; i++ ){
		
		Delay_ms( 1 );
		
		dat = PINC;
		cop = dat ^ buf;
		
		if( cop ){
			cnt = 0;
		}
		else {
			cnt++;
			if( cnt == 50 ){
				Modbus_Addr = ~dat & 0x1F;
				break;
			}
		}
		buf = dat;
	}
	
	if( Modbus_Addr == 0 ){
		Modbus_Addr = 1;
	}
	
	memset( Modbus_InputReg, 0, sizeof(Modbus_InputReg) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Read_Input_Register_Slave( U8 *rxbuf )
{
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U16		bc;
	U8		bData[256], Len;
	
	wCRC = CRC16( rxbuf, MRX_CHKSUM_LOW );
	if( !( ((wCRC&255) == *(rxbuf+MRX_CHKSUM_LOW)) && ((wCRC>>8&255) == *(rxbuf+MRX_CHKSUM_HGH)) )){
		if( !LogMode ){
			DPRINT("CRC Fail\n");
		}
		return;
	}
	
	ad = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	wc = (((U16)*(rxbuf+MRX_DATACHGH))<<8) | ((U16)*(rxbuf+MRX_DATACLOW));
	bc = wc*2;
	
	if( ad >= MAX_INPUT_REGISTER ){
		if( !LogMode ){
			DPRINT("Buff Size 1 Fail\n");
		}
		return;
	}
	if( wc == 0 || wc > 125 || wc > MAX_INPUT_REGISTER ){
		if( !LogMode ){
			DPRINT("Buff Size 2 Fail\n");
		}
		return;
	}
	if( ad+wc > MAX_INPUT_REGISTER ){
		if( !LogMode ){
			DPRINT("Buff Size 3 Fail\n");
		}
		return;
	}
	
#if 0
    if( Modbus_Init == 0 ){
    	Modbus_Init = 1;
    	
    	Modbus_InputReg[15] = 0;
    }
#endif
	
	bData[MTX_STATNO] = *(rxbuf+MRX_STATNO);
	bData[MTX_FUNCODE] = *(rxbuf+MRX_FUNCODE);
	bData[MTX_BYTECNT] = bc;
	
	for( i=0; i<wc; i++ ){
		bData[MTX_DATACHGH+i*2] = Modbus_InputReg[ad+i] >> 8 & 255;
		bData[MTX_DATACLOW+i*2] = Modbus_InputReg[ad+i] & 255;
	}
	
	wCRC = CRC16( &bData[0], bc+MTX_DATA );
	bData[bc+3] = wCRC & 255;
	bData[bc+4] = wCRC >> 8 & 255;
	
	Len = bc+5;
	
	USART1_Packet_Send( bData, Len );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Read_Holding_Register_Slave( U8 *rxbuf )
{
	U16		ad, wc;
	U16		wCRC;
	U16		i;
	U16		bc;
	U8		bData[256], Len;
	
	wCRC = CRC16( rxbuf, MRX_CHKSUM_LOW );
	if( !( ((wCRC&255) == *(rxbuf+MRX_CHKSUM_LOW)) && ((wCRC>>8&255) == *(rxbuf+MRX_CHKSUM_HGH)) )){
		if( !LogMode ){
			DPRINT("CRC Fail\n");
		}
		return;
	}
	
	ad = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	wc = (((U16)*(rxbuf+MRX_DATACHGH))<<8) | ((U16)*(rxbuf+MRX_DATACLOW));
	bc = wc*2;
	
	if( ad >= TRIPLE_IR_REL_RATIO_MAX /*EEP_MAX*/ ){		// jmyoo
		if( !LogMode ){
			DPRINT("Buff Size 1 Fail\n");
		}
		return;
	}
	if( wc == 0 || wc > 125 || wc > EEP_MAX ){
		if( !LogMode ){
			DPRINT("Buff Size 2 Fail\n");
		}
		return;
	}
	if( ad+wc > TRIPLE_IR_REL_RATIO_MAX /*EEP_MAX*/ ){
		if( !LogMode ){
			DPRINT("Buff Size 3 Fail\n");
		}
		return;
	}
	
	bData[MTX_STATNO] = *(rxbuf+MRX_STATNO);
	bData[MTX_FUNCODE] = *(rxbuf+MRX_FUNCODE);
	bData[MTX_BYTECNT] = bc;
	
	for( i=0; i<wc; i++ ){
		//bData[MTX_DATACHGH+i*2] = epr_sm[ad+i] >> 8 & 255;
		//bData[MTX_DATACLOW+i*2] = epr_sm[ad+i] & 255;
		if( ad + wc > EEP_MAX )
		{
			bData[MTX_DATACHGH+i*2] = EEP_Rd(ad+i) >> 8 & 255;
			bData[MTX_DATACLOW+i*2] = EEP_Rd(ad+i) & 255;
		}
		else
		{
			bData[MTX_DATACHGH+i*2] = epr_sm[ad+i] >> 8 & 255;
			bData[MTX_DATACLOW+i*2] = epr_sm[ad+i] & 255;
		}
	}
	
	wCRC = CRC16( &bData[0], bc+MTX_DATA );
	bData[bc+3] = wCRC & 255;
	bData[bc+4] = wCRC >> 8 & 255;
	
	Len = bc+5;
	
	USART1_Packet_Send( bData, Len );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Write_Single_Register_Slave( U8 *rxbuf )
{
	U16		ad;
	S16		data;
	U16		wCRC;
	
	wCRC = CRC16( rxbuf, MRX_CHKSUM_LOW );
	if( !( ((wCRC&255) == *(rxbuf+MRX_CHKSUM_LOW)) && ((wCRC>>8&255) == *(rxbuf+MRX_CHKSUM_HGH)) )){
		if( !LogMode ){
			DPRINT("CRC Fail\n");
		}
		return;
	}
	
	ad = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	data = (((S16)*(rxbuf+MRX_DATACHGH))<<8) | ((S16)*(rxbuf+MRX_DATACLOW));
	
	if(( ad <= EEP_PREFIX_1 ) || ( ad >= TRIPLE_IR_REL_RATIO_MAX/*EEP_MAX*/ )){			// jmyoo
		if( LogMode == LMODE_NO ){
			DPRINT("Buff Size 1 Fail\n");
		}
		return;
	}
	
	if(( data >= (S16)pgm_read_word( &EEP_SYS_LO_LIMIT[ad] )) && ( data <= (S16)pgm_read_word( &EEP_SYS_UP_LIMIT[ad] ))){
		EEP_Sys_Wr( ad, data );
		if( !EepErrorFlag ){
			USART1_Packet_Send( rxbuf, 8 );
		}
	}
	else {
		DPRINT3(" OVER RANGE, IN: %d, LO: %d, UP: %d\n", data, pgm_read_word( &EEP_SYS_LO_LIMIT[ad] ), pgm_read_word( &EEP_SYS_UP_LIMIT[ad] ));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Write_Multiple_Register_Slave( U8 *rxbuf )
{
	U16		ad, wc;
	S16		data;
	U16		wCRC;
	U16		i;
	U8		bc, Eep_Block;
	U8		bData[8];
	
	bc = *(rxbuf+MRX_BYTECNT);
	wCRC = CRC16( rxbuf, bc+7 );
	if( !( ((wCRC&255) == *(rxbuf+bc+7)) && ((wCRC>>8&255) == *(rxbuf+bc+8)) )){
		if( !LogMode ){
			DPRINT("CRC Fail\n");
		}
		return;
	}
	
	ad = (((U16)*(rxbuf+MRX_ADDHGH))<<8) | ((U16)*(rxbuf+MRX_ADDLOW));
	wc = (((U16)*(rxbuf+MRX_DATACHGH))<<8) | ((U16)*(rxbuf+MRX_DATACLOW));
	
	if(( ad <= EEP_PREFIX_1 ) || ( ad >= TRIPLE_IR_REL_RATIO_MAX /*EEP_MAX*/ )){	
		if( !LogMode ){
			DPRINT("Buff Size 1 Fail\n");
		}
		return;
	}
	
	if( wc == 0 || wc > EEP_MAX ){
		if( !LogMode ){
			DPRINT("Buff Size 2 Fail\n");
		}
		return;
	}
	
	if( ad+wc > TRIPLE_IR_REL_RATIO_MAX /*EEP_MAX*/ ){
		if( !LogMode ){
			DPRINT("Buff Size 3 Fail\n");
		}
		return;
	}
	
	for( i=0; i<wc; i++ ){
		data = (((S16)*(rxbuf+7+i*2))<<8) | ((S16)*(rxbuf+8+i*2));
		if(( data >= (S16)pgm_read_word( &EEP_SYS_LO_LIMIT[ad+i] )) && ( data <= (S16)pgm_read_word( &EEP_SYS_UP_LIMIT[ad+i] )))
		{			
			if (ad < EEP_MAX) 
			{
				epr_sm[ad+i] = data;
			}
			EEP_Wr( ad+i, data );	
		}
		else {
			DPRINT3(" OVER RANGE, IN: %d, LO: %d, UP: %d\n", data, pgm_read_word( &EEP_SYS_LO_LIMIT[ad+i] ), pgm_read_word( &EEP_SYS_UP_LIMIT[ad+i] ));
		}
	}
	
	Eep_Block = EEP_Block_Check( ad );
	EEP_CRC_Wr(Eep_Block);
	
	if ( EEP_CRC_Check(Eep_Block) )	EepErrorFlag = ad;
	else					EepErrorFlag = 0;
	
	if( !EepErrorFlag ){
		for( i=0; i<6; i++ ){
			bData[i] = *(rxbuf+i);
		}
		wCRC = CRC16( bData, 6 );
		bData[6] = wCRC & 255;
		bData[7] = wCRC >> 8 & 255;
		USART1_Packet_Send( bData, 8 );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FlameData_Mapping( FLAME_HandleTypeDef *f )
{
	double	Cps;
		
	Modbus_InputReg[ 0] = _FW_VERSION_MA*100+_FW_VERSION_MI;	//30001
	Modbus_InputReg[ 1] = AlarmState;							//30002
	Modbus_InputReg[ 2] = (U16)(P24*10);						//30003
	Modbus_InputReg[ 3] = (U16)(Tempareture*10);				//30004
	Modbus_InputReg[ 4] = AlarmLatch;							//30005
	Modbus_InputReg[ 5] = DetDistance;							//30006
	Modbus_InputReg[ 6] = FlameType;							//30007  1 : non-hydrocarbon, 0 : hydrocarbon
	Modbus_InputReg[ 7] = 0;									//30008
	Modbus_InputReg[ 8] = 0;									//30009
	Modbus_InputReg[ 9] = 0;									//30010
	Modbus_InputReg[10] = 0;									//30011		
	Modbus_InputReg[11] = 0;									//30012		
	Modbus_InputReg[12] = 0;									//30013
	Modbus_InputReg[13] = 0;									//30014
	Modbus_InputReg[14] = 0;									//30015	
	Modbus_InputReg[15] = EepErrorFlag;							//30016
	
	if( DeviceType == DTYPE_UVIR ) {
		Cps = f->UvCpsRes;
		if( Cps > IrFlickerCpsRefLmt ){
			Cps = IrFlickerCpsRefLmt;
		}
		Modbus_InputReg[16] = DeviceType;								//30017
		Modbus_InputReg[17]++;											//30018
		Modbus_InputReg[18] = 0;										//30019
		Modbus_InputReg[19] = 0;										//30020
		Modbus_InputReg[20] = 0;						 				//30021
		Modbus_InputReg[21] = 0;										//30022
		Modbus_InputReg[22] = f->IrLmtOffset[IR_WL_430];				//30023
		Modbus_InputReg[23] = f->IrDirCntSum[IR_WL_430];				//30024
		Modbus_InputReg[24] = f->IrFreqDirCntSum[IR_WL_430];			//30025
		Modbus_InputReg[25] = f->IrWidestTimeSum[IR_WL_430];			//30026
		Modbus_InputReg[26] = f->IrTimeDvSum[IR_WL_430];				//30027
		Modbus_InputReg[27] = (U16)(f->IrSizeDvPer[IR_WL_430]*1000);	//30028
		Modbus_InputReg[28] = f->IrLoFreq[IR_WL_430];					//30029
		Modbus_InputReg[29] = f->IrLoTime[IR_WL_430];					//30030
		Modbus_InputReg[30] = (U16)(f->IrLoPeak[IR_WL_430]*1000);		//30031
		Modbus_InputReg[31] = (U16)(f->IrLoAvrg[IR_WL_430]*1000);		//30032
		Modbus_InputReg[32] = f->IrHiFreq[IR_WL_430];					//30033
		Modbus_InputReg[33] = f->IrHiTime[IR_WL_430];					//30034
		Modbus_InputReg[34] = (U16)(f->IrHiPeak[IR_WL_430]*1000);		//30035
		Modbus_InputReg[35] = (U16)(f->IrHiAvrg[IR_WL_430]*1000);		//30036
		Modbus_InputReg[36] = f->IrFreqDv[IR_WL_430];					//30037
		Modbus_InputReg[37] = f->IrTimeDv[IR_WL_430];					//30038
		Modbus_InputReg[38] = (U16)(f->Ir1sAvrg[IR_WL_430]*1000);		//30039
		Modbus_InputReg[39] = (U16)(f->Ir1sPtoP[IR_WL_430]*1000);		//30040
		Modbus_InputReg[40] = (U16)(f->IrAvPtoP[IR_WL_430]*1000);		//30041
		Modbus_InputReg[41] = f->IrSize[IR_WL_430];						//30042
		Modbus_InputReg[42] = (U16)(f->Ir1sAvrgDv[IR_WL_430]*1000);		//30043
		Modbus_InputReg[43] = (U16)(f->IrAvPtoPDv[IR_WL_430]*1000);		//30044
		Modbus_InputReg[44] = (U16)(Cps * IrFlickerFactor * 1000);		//30045
		Modbus_InputReg[45] = f->UvZrCnt;								//30046
		Modbus_InputReg[46] = f->UvDvCnt;								//30047
		Modbus_InputReg[47] = f->UvZrCntDv;								//30048
		Modbus_InputReg[48] = f->UvDvCntDv;								//30049
		Modbus_InputReg[49] = f->UvRawMax;								//30050
		Modbus_InputReg[50] = f->UvCpsRaw;								//30051
		Modbus_InputReg[51] = (U16)(f->UvCpsRes*10);					//30052
		Modbus_InputReg[52] = (U16)(f->UvCpsItg*10);					//30053
		Modbus_InputReg[53] = f->UvCpsRoc;								//30054
		Modbus_InputReg[54] = (U16)(f->UvCpmRaw>>16&0xFFFF);			//30055
		Modbus_InputReg[55] = (U16)(f->UvCpmRaw&0xFFFF);				//30056
		Modbus_InputReg[56] = f->IrFtFlag;								//30057
		Modbus_InputReg[57] = f->UvFtFlag;								//30058
		Modbus_InputReg[58] = (U16)(P24*10);							//30059
		Modbus_InputReg[59] = (U16)(PUV*10);							//30060	
		Modbus_InputReg[60] = (U16)(Tempareture*10);					//30061
		Modbus_InputReg[61] = IrDetCnt;									//30062
		Modbus_InputReg[62] = UvDetCnt;									//30063
		Modbus_InputReg[63] = FlameCnt;									//30064
	}
	else {
		Modbus_InputReg[16] = DeviceType;
		Modbus_InputReg[17]++;
		Modbus_InputReg[18] = 0;
		Modbus_InputReg[19] = f->IrLoFreq[IR_WL_430];
		Modbus_InputReg[20] = f->IrLoTime[IR_WL_430];
		Modbus_InputReg[21] = (U16)(f->IrLoPeak[IR_WL_430]*1000);
		Modbus_InputReg[22] = (U16)(f->IrLoAvrg[IR_WL_430]*1000);
		Modbus_InputReg[23] = f->IrHiFreq[IR_WL_430];
		Modbus_InputReg[24] = f->IrHiTime[IR_WL_430];
		Modbus_InputReg[25] = (U16)(f->IrHiPeak[IR_WL_430]*1000);
		Modbus_InputReg[26] = (U16)(f->IrHiAvrg[IR_WL_430]*1000);
		Modbus_InputReg[27] = (U16)(f->Ir1sAvrg[IR_WL_430]*1000);
		Modbus_InputReg[28] = (U16)(f->IrAvPtoP[IR_WL_430]*1000);
		Modbus_InputReg[29] = f->IrSize[IR_WL_430];
		Modbus_InputReg[30] = f->IrSizeRoc[IR_WL_430];
		Modbus_InputReg[31] = f->IrLoFreq[IR_WL_395];
		Modbus_InputReg[32] = f->IrLoTime[IR_WL_395];
		Modbus_InputReg[33] = (U16)(f->IrLoPeak[IR_WL_395]*1000);
		Modbus_InputReg[34] = (U16)(f->IrLoAvrg[IR_WL_395]*1000);
		Modbus_InputReg[35] = f->IrHiFreq[IR_WL_395];
		Modbus_InputReg[36] = f->IrHiTime[IR_WL_395];
		Modbus_InputReg[37] = (U16)(f->IrHiPeak[IR_WL_395]*1000);
		Modbus_InputReg[38] = (U16)(f->IrHiAvrg[IR_WL_395]*1000);
		Modbus_InputReg[39] = (U16)(f->Ir1sAvrg[IR_WL_395]*1000);
		Modbus_InputReg[40] = (U16)(f->IrAvPtoP[IR_WL_395]*1000);
		Modbus_InputReg[41] = f->IrSize[IR_WL_395];
		Modbus_InputReg[42] = f->IrSizeRoc[IR_WL_395];
		Modbus_InputReg[43] = f->IrLoFreq[IR_WL_530];
		Modbus_InputReg[44] = f->IrLoTime[IR_WL_530];
		Modbus_InputReg[45] = (U16)(f->IrLoPeak[IR_WL_530]*1000);
		Modbus_InputReg[46] = (U16)(f->IrLoAvrg[IR_WL_530]*1000);
		Modbus_InputReg[47] = f->IrHiFreq[IR_WL_530];
		Modbus_InputReg[48] = f->IrHiTime[IR_WL_530];
		Modbus_InputReg[49] = (U16)(f->IrHiPeak[IR_WL_530]*1000);
		Modbus_InputReg[50] = (U16)(f->IrHiAvrg[IR_WL_530]*1000);
		Modbus_InputReg[51] = (U16)(f->Ir1sAvrg[IR_WL_530]*1000);
		Modbus_InputReg[52] = (U16)(f->IrAvPtoP[IR_WL_530]*1000);
		Modbus_InputReg[53] = f->IrSize[IR_WL_530];
		Modbus_InputReg[54] = f->IrSizeRoc[IR_WL_530];
		Modbus_InputReg[55] = (U16)(f->IrRatioLmt[IR_WL_395]*10);
		Modbus_InputReg[56] = (U16)(f->IrRatioLmt[IR_WL_530]*10);
		Modbus_InputReg[57] = (U16)(f->IrRelRatio[IR_WL_395]*10);
		Modbus_InputReg[58] = (U16)(f->IrRelRatio[IR_WL_530]*10);
		Modbus_InputReg[59] = f->IrFtFlag;
		Modbus_InputReg[60] = (U16)(P24*10);
		Modbus_InputReg[61] = (U16)(Tempareture*10);
		Modbus_InputReg[62] = IrWarCnt;
		Modbus_InputReg[63] = FlameCnt;
	}
	Modbus_InputReg[64] = _FW_VERSION_SUB;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MODBUS_Packet_Decoder( U8* pBuf )
{
	switch( *(pBuf+MRX_FUNCODE) ){
		case 0x03:
			MODBUS_Read_Holding_Register_Slave( pBuf );
			break;
			
		case 0x04:
			MODBUS_Read_Input_Register_Slave( pBuf );
			break;
			
		case 0x06:
			MODBUS_Write_Single_Register_Slave( pBuf );
			break;
			
		case 0x10:
		case 0x41:  // Ratio Write User Code
			MODBUS_Write_Multiple_Register_Slave( pBuf );
			break;
	}
}
