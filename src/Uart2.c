/*
*********************************************************************************************************
* MPS SOURCE

*********************************************************************************************************
*/
/*
*********************************************************************************************************
*
* UART2 OPERATIONS  
*
* Filename : PWMTest.c
* Version : V1.0
* Programmer(s) : JOE
* FGK
*********************************************************************************************************
* Note(s) : (1)OC4 is used , it is used to be pulse phase A for MPS;20110216
* (2) 
*********************************************************************************************************
/*
*********************************************************************************************************
* INCLUDE FILES
*********************************************************************************************************
*/
#include <p30fxxxx.h>
#include "UniversalDef.h"
#include "UserParams.h"	
#include "Function.h"	
#include "Variable.h"
/*
*********************************************************************************************************
* LOCAL DEFINES
*********************************************************************************************************
*/
#define BAUD_RATE    	((dFcy/16/9600)-1)	/*Low speed = Fcy/16;else Fcy/4/*/
#define RX_INTERVER_TIME    	10			/*unit (10ms)*/
/*
*********************************************************************************************************
* LOCAL VARIABLES
*********************************************************************************************************
*/
tU2TxRx	U2TxRx;;

unsigned int SystemStatusBits;
unsigned int inputBits;
unsigned int outputBits;
extern unsigned int StartOFTOK;
/*
*********************************************************************************************************
* GLOBAL VARIABLES
*********************************************************************************************************
*/
tUart2	Uart2;
tUart232	Uart232;

/*
*********************************************************************************************************
*********************************************************************************************************
* LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U2RXInterrupt (void);
void __attribute__((__interrupt__)) _U2TXInterrupt (void);
void Uart2RxApp(void);
void Uart2RxErrApp(void);
void Uart2App(void);
void Uart2TxSetSuccess(void);
void InitUART2(void);
/*
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart2RxErrApp()
{
	if(Uart2.RxByteF)
	{
		U2TxRx.RxIntervalCnt++;
		if(U2TxRx.RxIntervalCnt > RX_INTERVER_TIME)
		{
			U2TxRx.RxTimes = 0;
		}
	}
	if(U2STAbits.PERR || U2STAbits.OERR || U2STAbits.FERR)
	{InitUART2();}	
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart2App()
{
  
    U2TxRx.Txcnt++;
    if(U2TxRx.Txcnt>500)
    {InitUART2();
    U2TxRx.RxTimes = 0;
	Uart2.RxByteF = 0;
    Uart2.TxReadDataF3 =1;
    }
   
   
	if(Uart2.RxFrameF)
	{
		Uart2RxApp();
		Uart2.RxFrameF = 0;	
        U2TxRx.Txcnt=0;
	}
	if(Uart2.TxSetSuccessF)//写E2prom1
	{
		EEWriteApp();
		Uart2TxSetSuccess();
             
		StateInit();
		Uart2.TxSetSuccessF = 0;
      
	}
	if(Uart2.TxSetSuccessF2)//写E2prom2
	{
		EEWriteApp2();
		Uart2TxSetSuccess2();
             
		StateInit();
		Uart2.TxSetSuccessF2 = 0;
        
	}
	if(Uart232.TxSetSuccessF3)//写E2prom3
	{
		EEWriteApp3();
		Uart2TxSetSuccess3();
             
		StateInit();
		Uart232.TxSetSuccessF3 = 0;
	}

	if(Uart232.TxSetSuccessF4)   //写E2prom4   200421
	{
		EEWriteApp4();
		Uart2TxSetSuccess4();
             
		StateInit();
		Uart232.TxSetSuccessF4 = 0;
	}


	if(Uart2.TxReadSetF)//读参数1
	{
		Uart2TxReadSet();
		Uart2.TxReadSetF = 0;
        U2TxRx.Txcnt=0;
	}
	if(Uart2.TxReadSetF2)//读写参数2
	{
		Uart2TxReadSet2();
		Uart2.TxReadSetF2 = 0;
        U2TxRx.Txcnt=0;
	}

	if(Uart232.TxReadSetF3)//读写参数3
	{
		Uart2TxReadSet3();
		Uart232.TxReadSetF3 = 0;
       U2TxRx.Txcnt=0;
	}

	if(Uart232.TxReadSetF4)//读写参数4   200421
	{
		Uart2TxReadSet4();
		Uart232.TxReadSetF4 = 0;
        U2TxRx.Txcnt=0;
	}

	if(Uart2.TxReadDataF )//读数据1
	{
		Uart2TxReadData();
		Uart2.TxReadDataF  = 0;
        U2TxRx.Txcnt=0;
	}
	if(Uart2.TxReadDataF2 )//读数据2
	{
		Uart2TxReadData2();
		Uart2.TxReadDataF2  = 0;
        U2TxRx.Txcnt=0;
	}

	if(Uart2.TxReadDataF3 )//读数据2
	{
		Uart2TxReadData3();
		Uart2.TxReadDataF3  = 0;
        U2TxRx.Txcnt=0;
	}



	if(Uart2.TxReadLastMsgF )
	{
		Uart2TxReadLastMsg();
		Uart2.TxReadLastMsgF  = 0;
	}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart2TxSetSuccess()
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X20;		
	U2TxRx.TxOrderID = 0X83;
	U2TxRx.TxDataLength = 0;
	U2TxRx.TxEnd = 0XBB;
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}
void Uart2TxSetSuccess2()
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X20;		
	U2TxRx.TxOrderID = 0X73;
	U2TxRx.TxDataLength = 0;
	U2TxRx.TxEnd = 0XBB;
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

void Uart2TxSetSuccess3()
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X20;		
	U2TxRx.TxOrderID = 0X63;
	U2TxRx.TxDataLength = 0;
	U2TxRx.TxEnd = 0XBB;
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

void Uart2TxSetSuccess4()   //读参数4成功报文  200421
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X20;		
	U2TxRx.TxOrderID = 0X53;
	U2TxRx.TxDataLength = 0;
	U2TxRx.TxEnd = 0XBB;
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void EEWriteApp()
{	
	unsigned int i = 0;
	SET_CPU_IPL(7);	
    IEC0bits.ADIE = 0;
	for(i = 0; i < 14; i ++)
    {
		EEPROMADDR = 0xFC00 + 2 * i;
		EraseEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		WriteEE(&U2TxRx.RxData16Bits[i],__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
	}
	
	SET_CPU_IPL(3);	
   IEC0bits.ADIE = 1;
}	

void EEWriteApp2()
{	
	unsigned int i = 0;
	SET_CPU_IPL(7);	
IEC0bits.ADIE = 0;
	for(i = 0; i < 30; i ++)
    {
		EEPROMADDR = 0xFC00 + 2 * i;
		EraseEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		WriteEE(&U2TxRx.RxData16Bits[i],__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
	}
	
	SET_CPU_IPL(3);	
IEC0bits.ADIE = 1;
}	

void EEWriteApp3()
{	
	unsigned int i = 0,j=0;
	SET_CPU_IPL(7);
IEC0bits.ADIE = 0;	
	for(i = 32; i < 62; i ++)
    {
		EEPROMADDR = 0xFC00 + 2 * i;
		EraseEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		WriteEE(&U2TxRx.RxData16Bits[j],__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
        j++;
	}
	
	SET_CPU_IPL(3);	
IEC0bits.ADIE = 1;
}

void EEWriteApp4()
{	
	unsigned int i = 0,j=0;
	SET_CPU_IPL(7);
IEC0bits.ADIE = 0;	
	for(i = 62; i < 92; i ++)
    {
		EEPROMADDR = 0xFC00 + 2 * i;
		EraseEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		WriteEE(&U2TxRx.RxData16Bits[j],__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
        j++;
	}
	
	SET_CPU_IPL(3);	
IEC0bits.ADIE = 1;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/

void Uart2TxReadSet()//读参数1
{
	unsigned int i,j;
	i = 0;
	j = 0;
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X82;
	U2TxRx.TxDataLength = 28;

	for(i = 0; i < 16; i ++)
    {
    	EEPROMADDR = 0xFC00 + 2 * i;
    	ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	}
    for(i = 0; i < 32; )
    {
		U2TxRx.TxData[i++] = MainParams.SaveParams[j]>>8;
		U2TxRx.TxData[i++] = MainParams.SaveParams[j];
		j++;
	}
	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

void Uart2TxReadSet2()//读参数2
{
	unsigned int i,j;
	i = 0;
	j = 0;
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X72;
	U2TxRx.TxDataLength = 60;

	for(i = 0; i < 32; i ++)
    {
    	EEPROMADDR = 0xFC00 + 2 * i;
    	ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	}
    for(i = 0; i < 64; )
    {
		U2TxRx.TxData[i++] = MainParams.SaveParams[j]>>8;
		U2TxRx.TxData[i++] = MainParams.SaveParams[j];
		j++;
	}
	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

void Uart2TxReadSet3()//读参数2
{
	unsigned int i,j;
	i = 0;
	j = 32;
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X62;
	U2TxRx.TxDataLength = 60;

	for(i = 32; i < 62; i ++)
    {
    	EEPROMADDR = 0xFC00 + 2 * i;
    	ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	}
    for(i = 0; i < 60; )
    {
		U2TxRx.TxData[i++] = MainParams.SaveParams[j]>>8;
		U2TxRx.TxData[i++] = MainParams.SaveParams[j];
		j++;
	}
	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

void Uart2TxReadSet4()//读参数2
{
	unsigned int i,j;
	i = 0;
	j = 32;
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X52;
	U2TxRx.TxDataLength = 60;

	for(i = 62; i < 92; i ++)
    {
    	EEPROMADDR = 0xFC00 + 2 * i;
    	ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	}
    for(i = 0; i < 60; )
    {
		U2TxRx.TxData[i++] = MainParams.SaveParams[j]>>8;
		U2TxRx.TxData[i++] = MainParams.SaveParams[j];
		j++;
	}
	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart2TxReadData()
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X81;
	U2TxRx.TxDataLength = 22;
   if((SysStatus == START)||(SysStatus == RUN))
    {
	U2TxRx.TxData[0] = ADI.Ia >> 8;
	U2TxRx.TxData[1] = ADI.Ia;
	U2TxRx.TxData[2] = ADI.Ib >> 8;
	U2TxRx.TxData[3] = ADI.Ib;
	//U2TxRx.TxData[4] = ADI.Ic >> 8;
	//U2TxRx.TxData[5] = ADI.Ic;


	U2TxRx.TxData[4] = AdcParams.delaylimit >> 8;
	U2TxRx.TxData[5] = AdcParams.delaylimit;
    }
    else
   {U2TxRx.TxData[0] =0;
    U2TxRx.TxData[1] =0;
    U2TxRx.TxData[2] =0;
    U2TxRx.TxData[3] =0;
    U2TxRx.TxData[4] =0;
    U2TxRx.TxData[5] =0;


   }
  	
	U2TxRx.TxData[6] = MainParams.Ua >> 8;
	U2TxRx.TxData[7] = MainParams.Ua;
	U2TxRx.TxData[8] = MainParams.Ub >> 8;
	U2TxRx.TxData[9] = MainParams.Ub;
	U2TxRx.TxData[10] = MainParams.Uc >> 8;
	U2TxRx.TxData[11] = MainParams.Uc;

	
	U2TxRx.TxData[12] = StartParams.StartTime1s >> 8;
	U2TxRx.TxData[13] = StartParams.StartTime1s;


	U2TxRx.TxData[14] = SysStatus >> 8;
	U2TxRx.TxData[15] = SysStatus;
	if(U2TxRx.TxData[15] == 0)
	{
		U2TxRx.TxData[15] = 0;
		if(lowVoltageTest == 0xaaaa)
		{U2TxRx.TxData[15] = 4;}
	}

	else if(U2TxRx.TxData[15] == 1)
	{
		U2TxRx.TxData[15] = 1;
		if(lowVoltageTest == 0xaaaa)
		{U2TxRx.TxData[15] = 4;}
	}
	else if(U2TxRx.TxData[15] == 2)
	{U2TxRx.TxData[15] = 2;}
	else if(U2TxRx.TxData[15] == 3)
	{U2TxRx.TxData[15] = 2;}
	else if(U2TxRx.TxData[15] == 4)
	{U2TxRx.TxData[15] = 3;}
	else if(U2TxRx.TxData[15] ==5)
	{U2TxRx.TxData[15] = 5;}
	else 
	{U2TxRx.TxData[15] = 0;}
	
	U2TxRx.TxData[16] = MainParams.Temperature >> 8;
	U2TxRx.TxData[17] = MainParams.Temperature;

	U2TxRx.TxData[18] = Fault.Byte >> 8;
	U2TxRx.TxData[19] = Fault.Byte;

	U2TxRx.TxData[20] = StartParams.StartTime1s >> 8;
	U2TxRx.TxData[21] = StartParams.StartTime1s;

	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}

void Uart2TxReadData2()
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X71;
	U2TxRx.TxDataLength = 42;
  
  if((SysStatus == START)||(SysStatus == RUN))
    {
	U2TxRx.TxData[0] = ADI.Ia >> 8;
	U2TxRx.TxData[1] = ADI.Ia;
	U2TxRx.TxData[2] = ADI.Ib >> 8;
	U2TxRx.TxData[3] = ADI.Ib;
	U2TxRx.TxData[4] = ADI.Ic >> 8;
	U2TxRx.TxData[5] = ADI.Ic;




                       
    }
      else
   {U2TxRx.TxData[0] =0;
    U2TxRx.TxData[1] =0;
    U2TxRx.TxData[2] =0;
    U2TxRx.TxData[3] =0;
    U2TxRx.TxData[4] =0;
    U2TxRx.TxData[5] =0;
   }
  

 
	U2TxRx.TxData[6] = MainParams.Ua >> 8;
	U2TxRx.TxData[7] = MainParams.Ua;

/*************************IO显示输入点处理************************************/
    if(INPUT_START==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0001;}
    else
    {inputBits=inputBits&0xFFFE;}

    if(INPUT_STOP==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0002;}
    else
    {inputBits=inputBits&0xFFFD;}

    if(INPUT_READY==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0004;}
    else
    {inputBits=inputBits&0xFFFB;}

    if(INPUT_RUN_CHECK==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0008;}
    else
    {inputBits=inputBits&0xFFF7;}

    if(INPUT_NO_USE==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0010;}
    else
    {inputBits=inputBits&0xFFEF;}

    if(INPUT_in6==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0020;}
    else
    {inputBits=inputBits&0xFFDF;}

   if(INPUT_in7==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0040;}
    else
    {inputBits=inputBits&0xFFBF;}

   if(INPUT_in8==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0080;}
    else
    {inputBits=inputBits&0xFF7F;}

   if(INPUT_in9==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0100;}
    else
    {inputBits=inputBits&0xFEFF;}

   if(INPUT_in10==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0200;}
    else
    {inputBits=inputBits&0xFDFF;}
/*******************************************************************/
/***********************正反转检测****************************/

if (Functionswitch.FWD_REV==1)// 0为正转 1为反转
    {inputBits=inputBits|0x8000;}
    else
    {inputBits=inputBits&0x7FFF;}


/*******************************************************************/
/***********************IO显示输出点处理****************************/
    if(OUTPUT_READY==1)//备妥输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0001;}
    else
    {outputBits=outputBits&0xFFFE;}

    if(OUTPUT_START==1)//起动输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0002;}
    else
    {outputBits=outputBits&0xFFFD;}

    if(OUTPUT_TRIGGER==1)//触发输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0004;}
    else
    {outputBits=outputBits&0xFFFB;}

    if(OUTPUT_RUN_ON==1)//运行合闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0008;}
    else
    {outputBits=outputBits&0xFFF7;}

    if(OUTPUT_RUN_OFF==1)//运行分闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0010;}
    else
    {outputBits=outputBits&0xFFEF;}

    if(OUTPUT_ALARM==1)//报警输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0020;}
    else
    {outputBits=outputBits&0xFFDF;}

    if(OUTPUT_K5==1)//可编程-默认备妥输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0040;}
    else
    {outputBits=outputBits&0xFFBF;}

    if(OUTPUT_K6==1)//可编程-默认故障跳闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0080;}
    else
    {outputBits=outputBits&0xFF7F;}

    if(OUTPUT_K7==1)//可编程-默认起动接触器合闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0100;}
    else
    {outputBits=outputBits&0xFEFF;}

    if(OUTPUT_K8==1)//可编程-默认起动接触器合闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0200;}
    else
    {outputBits=outputBits&0xFDFF;}
/*********************************************************************/
	U2TxRx.TxData[8] = inputBits >> 8;
	U2TxRx.TxData[9] = inputBits;
	U2TxRx.TxData[10] = outputBits >> 8;
	U2TxRx.TxData[11] = outputBits;

	
	U2TxRx.TxData[12] = ProtectParams.intevalmin >> 8;
	U2TxRx.TxData[13] = ProtectParams.intevalmin;


	U2TxRx.TxData[14] = SysStatus >> 8;
	U2TxRx.TxData[15] = SysStatus;
	if(U2TxRx.TxData[15] == 0)
	{
		U2TxRx.TxData[15] = 0;
		if(lowVoltageTest == 0xaaaa)
		{U2TxRx.TxData[15] = 4;}
	}

	else if(U2TxRx.TxData[15] == 1)
	{
		U2TxRx.TxData[15] = 1;
		if(lowVoltageTest == 0xaaaa)
		{U2TxRx.TxData[15] = 4;}
	}
	else if(U2TxRx.TxData[15] == 2)
	{U2TxRx.TxData[15] = 2;}
	else if(U2TxRx.TxData[15] == 3)
	{U2TxRx.TxData[15] = 2;}
	else if(U2TxRx.TxData[15] == 4)
	{U2TxRx.TxData[15] = 3;}
	else if(U2TxRx.TxData[15] == 5)
	{U2TxRx.TxData[15] = 5;}
	else 
	{U2TxRx.TxData[15] = 0;}
	
	U2TxRx.TxData[16] = MainParams.Temperature >> 8;
	U2TxRx.TxData[17] = MainParams.Temperature;

	U2TxRx.TxData[18] = Fault.Byte >> 8;
	U2TxRx.TxData[19] = Fault.Byte;

	U2TxRx.TxData[20] = StartParams.StartTime1s >> 8;
	U2TxRx.TxData[21] = StartParams.StartTime1s;
 
	U2TxRx.TxData[22] = MainParams.frequency >> 8;
	U2TxRx.TxData[23] = MainParams.frequency;
 
    U2TxRx.TxData[24] = Version >> 8;
	U2TxRx.TxData[25] = Version;  

    U2TxRx.TxData[26] = compareACnt >> 8;
	U2TxRx.TxData[27] = compareACnt;

    if(phasecompareError==1)//相位比对错误  0为错误 1为正常
    {SystemStatusBits=SystemStatusBits|0x0001;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFFE;}

    if(Input.Ready==1)    //柜外允许  0为禁止 1为允许
    {SystemStatusBits=SystemStatusBits|0x0002;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFFD;}

     if(StartOFTOK==1)    //起动过频  0为禁止 1为允许
    {SystemStatusBits=SystemStatusBits|0x0004;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFFB;}

     if(MainParams.Temperature < ProtectParams.Temperature || Functionswitch.TDetection==0 )    //温度保护  温度小于设定或压板切除都满足条件
    {SystemStatusBits=SystemStatusBits|0x0008;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFF7;}


    U2TxRx.TxData[28] = SystemStatusBits>>8;
	U2TxRx.TxData[29] = SystemStatusBits;

if (Functionswitch.Delay_Zero==0)
   {
    U2TxRx.TxData[30] = LT >> 8;
	U2TxRx.TxData[31] = LT;
   }
else
   {
    U2TxRx.TxData[30] = MainParams.In >> 8;
	U2TxRx.TxData[31] = MainParams.In; 
   }

 //   U2TxRx.TxData[32] = ProtectParams.intevalsec >> 8;
//	U2TxRx.TxData[33] = ProtectParams.intevalsec;
    U2TxRx.TxData[32] = StartParams.OutData >> 8;
	U2TxRx.TxData[33] = StartParams.OutData;

    U2TxRx.TxData[34] = step11 >> 8;
	U2TxRx.TxData[35] = step11;


	U2TxRx.TxData[36] = AdcParams.pf_int >> 8;
	U2TxRx.TxData[37] = AdcParams.pf_int;
    U2TxRx.TxData[38] = MainParams.Ub>>8;
    U2TxRx.TxData[39] = MainParams.Ub;
    U2TxRx.TxData[40] = MainParams.Uc>>8;
    U2TxRx.TxData[41] = MainParams.Uc;

    //U2TxRx.TxData[38] = AdcParams.Dlta_t[0]>>8;
    //U2TxRx.TxData[39] = AdcParams.Dlta_t[0];
    //U2TxRx.TxData[40] = AdcParams.Dlta_t[1]>>8;
    //U2TxRx.TxData[41] = AdcParams.Dlta_t[1];

	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}
/*
*********************************************************************************************************
*/
void Uart2TxReadData3()
{
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X75;
	U2TxRx.TxDataLength = 60;
  
  if((SysStatus == START)||(SysStatus == RUN))
    {
	U2TxRx.TxData[0] = ADI.Ia >> 8;
	U2TxRx.TxData[1] = ADI.Ia;
	U2TxRx.TxData[2] = ADI.Ib >> 8;
	U2TxRx.TxData[3] = ADI.Ib;
	U2TxRx.TxData[4] = ADI.Ic >> 8;
	U2TxRx.TxData[5] = ADI.Ic;







                       
    }
      else
   {U2TxRx.TxData[0] =0;
    U2TxRx.TxData[1] =0;
    U2TxRx.TxData[2] =0;
    U2TxRx.TxData[3] =0;
    U2TxRx.TxData[4] =0;
    U2TxRx.TxData[5] =0;
   }
 
	U2TxRx.TxData[6] = MainParams.Ua >> 8;
	U2TxRx.TxData[7] = MainParams.Ua;

/*************************IO显示输入点处理************************************/
    if(INPUT_START==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0001;}
    else
    {inputBits=inputBits&0xFFFE;}

    if(INPUT_STOP==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0002;}
    else
    {inputBits=inputBits&0xFFFD;}

    if(INPUT_READY==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0004;}
    else
    {inputBits=inputBits&0xFFFB;}

    if(INPUT_RUN_CHECK==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0008;}
    else
    {inputBits=inputBits&0xFFF7;}

    if(INPUT_NO_USE==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0010;}
    else
    {inputBits=inputBits&0xFFEF;}

    if(INPUT_in6==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0020;}
    else
    {inputBits=inputBits&0xFFDF;}

   if(INPUT_in7==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0040;}
    else
    {inputBits=inputBits&0xFFBF;}

   if(INPUT_in8==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0080;}
    else
    {inputBits=inputBits&0xFF7F;}

   if(INPUT_in9==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0100;}
    else
    {inputBits=inputBits&0xFEFF;}

   if(INPUT_in10==0)//输入点都取反  0为有效有输入 1为无效
    {inputBits=inputBits|0x0200;}
    else
    {inputBits=inputBits&0xFDFF;}
/*******************************************************************/
/***********************正反转检测****************************/

if (Functionswitch.FWD_REV==1)// 0为正转 1为反转
    {inputBits=inputBits|0x8000;}
    else
    {inputBits=inputBits&0x7FFF;}


/*******************************************************************/
/***********************IO显示输出点处理****************************/
    if(OUTPUT_READY==1)//备妥输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0001;}
    else
    {outputBits=outputBits&0xFFFE;}

    if(OUTPUT_START==1)//起动输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0002;}
    else
    {outputBits=outputBits&0xFFFD;}

    if(OUTPUT_TRIGGER==1)//触发输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0004;}
    else
    {outputBits=outputBits&0xFFFB;}

    if(OUTPUT_RUN_ON==1)//运行合闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0008;}
    else
    {outputBits=outputBits&0xFFF7;}

    if(OUTPUT_RUN_OFF==1)//运行分闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0010;}
    else
    {outputBits=outputBits&0xFFEF;}

    if(OUTPUT_ALARM==1)//报警输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0020;}
    else
    {outputBits=outputBits&0xFFDF;}

    if(OUTPUT_K5==1)//可编程-默认备妥输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0040;}
    else
    {outputBits=outputBits&0xFFBF;}

    if(OUTPUT_K6==1)//可编程-默认故障跳闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0080;}
    else
    {outputBits=outputBits&0xFF7F;}

    if(OUTPUT_K7==1)//可编程-默认起动接触器合闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0100;}
    else
    {outputBits=outputBits&0xFEFF;}

    if(OUTPUT_K8==1)//可编程-默认起动接触器合闸输出点  1为有效有输入 0为无效
    {outputBits=outputBits|0x0200;}
    else
    {outputBits=outputBits&0xFDFF;}
/*********************************************************************/
	U2TxRx.TxData[8] = inputBits >> 8;
	U2TxRx.TxData[9] = inputBits;
	U2TxRx.TxData[10] = outputBits >> 8;
	U2TxRx.TxData[11] = outputBits;

	
	U2TxRx.TxData[12] = ProtectParams.intevalmin >> 8;
	U2TxRx.TxData[13] = ProtectParams.intevalmin;


	U2TxRx.TxData[14] = SysStatus >> 8;
	U2TxRx.TxData[15] = SysStatus;
	if(U2TxRx.TxData[15] == 0)
	{
		U2TxRx.TxData[15] = 0;
		if(lowVoltageTest == 0xaaaa)
		{U2TxRx.TxData[15] = 4;}
	}

	else if(U2TxRx.TxData[15] == 1)
	{
		U2TxRx.TxData[15] = 1;
		if(lowVoltageTest == 0xaaaa)
		{U2TxRx.TxData[15] = 4;}
	}
	else if(U2TxRx.TxData[15] == 2)
	{U2TxRx.TxData[15] = 2;}
	else if(U2TxRx.TxData[15] == 3)
	{U2TxRx.TxData[15] = 2;}
	else if(U2TxRx.TxData[15] == 4)
	{U2TxRx.TxData[15] = 3;}
	else if(U2TxRx.TxData[15] == 5)
	{U2TxRx.TxData[15] = 5;}
	else 
	{U2TxRx.TxData[15] = 0;}
	
	U2TxRx.TxData[16] = MainParams.Temperature >> 8;
	U2TxRx.TxData[17] = MainParams.Temperature;

	U2TxRx.TxData[18] = Fault.Byte >> 8;
	U2TxRx.TxData[19] = Fault.Byte;

	U2TxRx.TxData[20] = StartParams.StartTime1s >> 8;
	U2TxRx.TxData[21] = StartParams.StartTime1s;
 
	U2TxRx.TxData[22] = MainParams.frequency >> 8;
	U2TxRx.TxData[23] = MainParams.frequency;
 
    U2TxRx.TxData[24] = Version >> 8;
	U2TxRx.TxData[25] = Version;  

    U2TxRx.TxData[26] = compareACnt >> 8;
	U2TxRx.TxData[27] = compareACnt;

    if(phasecompareError==1)//相位比对错误  0为错误 1为正常
    {SystemStatusBits=SystemStatusBits|0x0001;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFFE;}

    if(Input.Ready==1)    //柜外允许  0为禁止 1为允许
    {SystemStatusBits=SystemStatusBits|0x0002;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFFD;}

     if(StartOFTOK==1)    //起动过频  0为禁止 1为允许
    {SystemStatusBits=SystemStatusBits|0x0004;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFFB;}

     if(MainParams.Temperature < ProtectParams.Temperature || Functionswitch.TDetection==0 )    //温度保护  温度小于设定或压板切除都满足条件
    {SystemStatusBits=SystemStatusBits|0x0008;}
    else
    {SystemStatusBits=SystemStatusBits&0xFFF7;}


    U2TxRx.TxData[28] = SystemStatusBits>>8;
	U2TxRx.TxData[29] = SystemStatusBits;

if (Functionswitch.Delay_Zero==0)
   {
    U2TxRx.TxData[30] = LT >> 8;
	U2TxRx.TxData[31] = LT;
   }
else
   {
    U2TxRx.TxData[30] = MainParams.In >> 8;
	U2TxRx.TxData[31] = MainParams.In; 
   }

 //   U2TxRx.TxData[32] = ProtectParams.intevalsec >> 8;
//	U2TxRx.TxData[33] = ProtectParams.intevalsec;
    U2TxRx.TxData[32] = StartParams.OutData >> 8;
	U2TxRx.TxData[33] = StartParams.OutData;

    U2TxRx.TxData[34] = step11 >> 8;
	U2TxRx.TxData[35] = step11;


	U2TxRx.TxData[36] = AdcParams.pf >> 8;
	U2TxRx.TxData[37] = AdcParams.pf;
    U2TxRx.TxData[38] = MainParams.Ub>>8;
    U2TxRx.TxData[39] = MainParams.Ub;
    U2TxRx.TxData[40] = MainParams.Uc>>8;
    U2TxRx.TxData[41] = MainParams.Uc;

    U2TxRx.TxData[42] = AdcParams.acos_max>>8;
    U2TxRx.TxData[43] = AdcParams.acos_max;
    U2TxRx.TxData[44] = AdcParams.Imax>>8;
    U2TxRx.TxData[45] = AdcParams.Imax;
    U2TxRx.TxData[46] = AdcParams.Laststarttime>>8;
    U2TxRx.TxData[47] = AdcParams.Laststarttime;
    U2TxRx.TxData[48] = AdcParams.Ud>>8;
    U2TxRx.TxData[49] = AdcParams.Ud;



	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;	
}
/***************************************************************************************************
*********************************************************************************************************
*/





void Uart2TxReadLastMsg(void)
{
	unsigned int i;
	U2TxRx.TxHead = 0XAA;
	U2TxRx.TxOrderType = 0X10;		
	U2TxRx.TxOrderID = 0X84;
	U2TxRx.TxDataLength = 4;

	i = 30;
	EEPROMADDR = 0xFC00 + 2 * i;
    ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	i = 31;
	EEPROMADDR = 0xFC00 + 2 * i;
    ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
   
	U2TxRx.TxData[0] = MainParams.SaveParams[30] >> 8;
	U2TxRx.TxData[1] = MainParams.SaveParams[30];

	U2TxRx.TxData[2] = MainParams.SaveParams[31] >> 8;
	U2TxRx.TxData[3] = MainParams.SaveParams[31];

	U2TxRx.TxEnd = 0XBB;
	
	U2TxRx.TxTimes = 0;	
	U2TXREG = 0xfe;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void InitUART2(void)
{	
	U2BRG = BAUD_RATE;

	U2MODE = 0;
	U2MODEbits.UARTEN = 1;			/*1 = Enable UARTx; UARTx pins are controlled by UARTx module*/
	U2MODEbits.USIDL = 1;			/*1 = Discontinue operation when device enters Idle mode*/
	U2MODEbits.ABAUD = 0;			/*0 = Baud rate measurement disabled or completed*/
	U2MODEbits.PDSEL = 0;			/*00 = 8-bit data, no parity*/
	U2MODEbits.STSEL = 0;			/*0 = 1 Stop bit*/

	U2STA = 0;
	U2STAbits.UTXEN = 1; 			/*1 = transmitter enabled, UxTX pin controlled by UARTx (UARTEN=1)*/
	U2STAbits.UTXISEL = 0;			/*0 = Interrupt when a character is transferred to the Transmit 
									Shift register
									1 = Interrupt when a character is transferred to the Transmit 
									Shift register and as result, the transmit buffer becomes empty*/
	U2STAbits.URXISEL = 0;			/*0x =Interrupt flag bit is set when a character is received*/
	 
	IFS1bits.U2TXIF = 0;
	IFS1bits.U2RXIF = 0;
	IPC6bits.U2TXIP = 4;
	IPC6bits.U2RXIP = 4;
	IEC1bits.U2TXIE = 1;				// 1 允许U2TX中断  
	IEC1bits.U2RXIE = 1;				// 1 允许U2RX中断 
	
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U2RXInterrupt (void)
{
	if(U2STAbits.OERR == 1) 
	{
		U2STAbits.OERR = 0; // Clear Overrun Error to receive data
	}
	Uart2.RxByteF = 1;
	U2TxRx.RxIntervalCnt = 0;
//	U2TxRx.RxHead = U2RXREG;
	if(!Uart2.RxFrameF)
	{
		if (U1STAbits.FERR ==0 )
		{
			if(U2TxRx.RxTimes == 0)
			{
				U2TxRx.RxHead = U2RXREG;
				if(U2TxRx.RxHead == 0xaa)
				{
					U2TxRx.RxTimes = 1;
					U2TxRx.RxDataCnt = 0;
				}
			}
			else if(U2TxRx.RxTimes == 1)
			{		
				U2TxRx.RxTimes = 2;
				U2TxRx.RxOrderType = U2RXREG;
		
			}
			else if(U2TxRx.RxTimes == 2)
			{
				U2TxRx.RxTimes = 3;
				U2TxRx.RxOrderID = U2RXREG;
			}     
			else if(U2TxRx.RxTimes == 3)
			{
				U2TxRx.RxTimes = 4;
				U2TxRx.RxDataLength = U2RXREG;
				U2TxRx.RxDataLengthRec = U2TxRx.RxDataLength;
				if(U2TxRx.RxDataLength == 28)//参数1
				{
					U2TxRx.RxTimes = 55;
				}
 	            else if(U2TxRx.RxDataLength == 60)//参数2
				{
					U2TxRx.RxTimes = 56;
				}
                else if(U2TxRx.RxDataLength== 1)//命令
				{
					U2TxRx.RxTimes = 4;
				}
                else if (U2TxRx.RxDataLength== 0)//读报文
                {  U2TxRx.RxTimes = 4;
                }


				else
				{
					U2TxRx.RxTimes = 0;
				}
				
				
			
			}
			else if(U2TxRx.RxTimes == 4)
			{
				U2TxRx.RxEnd = U2RXREG;
				if(U2TxRx.RxEnd == 0xbb)
				{
					Uart2.RxFrameF = 1;
					Uart2.RxByteF = 0;
//					IEC1bits.U2RXIE = 0;
				}
				U2TxRx.RxTimes = 0;
			}
			else if(U2TxRx.RxTimes == 55)
			{
				U2TxRx.RxDataLength--;
				U2TxRx.RxData[U2TxRx.RxDataCnt] = U2RXREG;
				U2TxRx.RxDataCnt++;
				if(U2TxRx.RxDataLength == 0)
				{
					U2TxRx.RxTimes = 4;
				}	
			}
           	else if(U2TxRx.RxTimes == 56)
			{
				U2TxRx.RxDataLength--;
				U2TxRx.RxData[U2TxRx.RxDataCnt] = U2RXREG;
				U2TxRx.RxDataCnt++;
				if(U2TxRx.RxDataLength == 0)
				{
					U2TxRx.RxTimes = 4;
				}	
			}
			
			else 
			{
				U2TxRx.RxTimes = 0;
			}
		}
	}
	else
	{U2TxRx.RxHead = U2RXREG;}	
	IFS1bits.U2RXIF = 0;
	return;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U2TXInterrupt (void)
{
	if(U2TxRx.TxTimes == 0)
	{
		U2TXREG = U2TxRx.TxHead;
		U2TxRx.TxTimes = 1;
	}
	else if(U2TxRx.TxTimes == 1)
	{		
		U2TxRx.TxTimes = 2;
		U2TXREG = U2TxRx.TxOrderType;

	}
	else if(U2TxRx.TxTimes == 2)
	{
		U2TxRx.TxTimes = 3;
		U2TXREG = U2TxRx.TxOrderID;
	}     
	else if(U2TxRx.TxTimes == 3)
	{
		U2TxRx.TxTimes = 4;
		U2TXREG = U2TxRx.TxDataLength;
		U2TxRx.TxDataCnt = 0;
	}
	else if(U2TxRx.TxTimes == 4)
	{
		if(U2TxRx.TxDataLength == 0)
		{
			U2TxRx.TxTimes = 66;
			U2TXREG = 0xbb;
			Uart2.TxFrameF = 1;
		}
		else
		{
			U2TxRx.TxTimes = 4;
			U2TXREG = U2TxRx.TxData[U2TxRx.TxDataCnt];//读参数
			U2TxRx.TxDataCnt++;		
			U2TxRx.TxDataLength--;
		}
	}	
	else 
	{
		U2TxRx.TxTimes = 66;
	}	
	IFS1bits.U2TXIF = 0;
	return;	
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart2RxApp()
{
	IFS1bits.U2RXIF = 0;
	IEC1bits.U2RXIE = 0;
	
	U2TxRx.RxDataCnt1 = 0;
	U2TxRx.RxDataCnt2 = 0;
	U2TxRx.RxDataCnt2 = 0;
	

	while(U2TxRx.RxDataLengthRec >= 2)
	{
		U2TxRx.RxDataLengthRec--;
		U2TxRx.RxDataLengthRec--;
		
		U2TxRx.RxDataAddrH = U2TxRx.RxData[U2TxRx.RxDataCnt1++];
		U2TxRx.RxDataAddrL = U2TxRx.RxData[U2TxRx.RxDataCnt1++];
				
		U2TxRx.RxDataAddrH = U2TxRx.RxDataAddrH << 8;
		U2TxRx.RxData16Bits[U2TxRx.RxDataCnt2++] = U2TxRx.RxDataAddrH + U2TxRx.RxDataAddrL;
	}

	if(U2TxRx.RxOrderType == 0x10)
	{
		if(U2TxRx.RxOrderID == 0x81)      //收到读数据1
		{
			Uart2.TxReadDataF = 1;
		}
        else if(U2TxRx.RxOrderID == 0x71) //收到读数据2
		{
			Uart2.TxReadDataF2 = 1;
		}

        else if(U2TxRx.RxOrderID == 0x75) //收到读数据2
		{
			Uart2.TxReadDataF3 = 1;
		}


		else if(U2TxRx.RxOrderID == 0x82)
		{
			Uart2.TxReadSetF = 1;         //收到读参数1
		}
		else if(U2TxRx.RxOrderID == 0x72)
		{
			Uart2.TxReadSetF2 = 1;        //收到读参数2
		}
		else if(U2TxRx.RxOrderID == 0x62)
		{
			Uart232.TxReadSetF3 = 1;        //收到读参数3
		}

		else if(U2TxRx.RxOrderID == 0x52)
		{
			Uart232.TxReadSetF4 = 1;        //收到读参数4  200421
		}
		else if(U2TxRx.RxOrderID == 0x84)
		{
			Uart2.TxReadLastMsgF = 1;
		}
        else if (U2TxRx.RxOrderID==0x85)
         {  if(U2TxRx.RxDataLength== 1)
             {
               Uart2.commandreset=1;
               Uart2.commandtest=0; //故障复位
               
             }
         }
        else if (U2TxRx.RxOrderID==0x86)
         {  if(U2TxRx.RxDataLength== 1)
             {
               Uart2.commandtest=1;//低压调试
             }
         }
        else if (U2TxRx.RxOrderID==0x87)
         {  if(U2TxRx.RxDataLength== 1)
             {
               Input.Start=1;//起动
             }
         }
        else if (U2TxRx.RxOrderID==0x88)
         {  if(U2TxRx.RxDataLength== 1)
             {
               Input.Stop=1;//停机
             }
         }


		else
		{Nop();}
		
	}
	else if(U2TxRx.RxOrderType == 0x20)
	{
		if(U2TxRx.RxOrderID == 0x83)
		{
		  	Uart2.TxSetSuccessF = 1;
		}
		if(U2TxRx.RxOrderID == 0x73)
		{
		  	Uart2.TxSetSuccessF2 = 1;
		}
		if(U2TxRx.RxOrderID == 0x63)
		{
		  	Uart232.TxSetSuccessF3 = 1;
		}	

		if(U2TxRx.RxOrderID == 0x53)    //读参数成功200421
		{
		  	Uart232.TxSetSuccessF4 = 1;
		}			
	}

	else
	{Nop();}
	IEC1bits.U2RXIE = 1;	
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/









