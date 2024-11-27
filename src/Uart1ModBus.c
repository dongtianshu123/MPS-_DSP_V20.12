/*
*********************************************************************************************************
* MPS SOURCE

*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Uart1 OPERATIONS  
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
#define BAUD_RATE    	((dFcy/16/StartParams.modbusbaud)-1)	/*Low speed = Fcy/16;else Fcy/4/*/
#define RX_INTERVER_TIME    	10			/*unit (10ms)*/
/*
*********************************************************************************************************
* LOCAL VARIABLES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* GLOBAL VARIABLES
*********************************************************************************************************
*/
tUart1	Uart1;
/*
*********************************************************************************************************
*********************************************************************************************************
* LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U1RXInterrupt (void);
void __attribute__((__interrupt__)) _U1TXInterrupt (void);
void Uart1RxApp(void);
void Uart1RxErrApp(void);
void Uart1App(void);
void Uart1TxModbus(void);
void Uart1TxCRCerr(void);
void InitUart1(void);
unsigned int CRC16_Check(unsigned char *Pushdata,unsigned char length);
/*
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*/
tU1TxRx	U1TxRx;
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart1RxErrApp()
{
	if(Uart1.RxByteF)
	{
		U1TxRx.RxIntervalCnt++;
		if(U1TxRx.RxIntervalCnt > RX_INTERVER_TIME)
		{
			U1TxRx.RxTimes = 0;
		}
	}
//	U1TxRx.RxIntervalCnt1++;
//	if(Uart1.RxFrameF)
//	{
//		U1TxRx.RxIntervalCnt1 = 0;	
//	}
//	if(U1TxRx.RxIntervalCnt1 > 300)
//	{
//		InitUart1();
//		U1TxRx.RxIntervalCnt1 = 0;
//	}
	if(U1STAbits.PERR || U1STAbits.OERR || U1STAbits.FERR)
	{InitUart1();}
	
	
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart1App()
{
    U1TxRx.Txcnt++;
    if(U1TxRx.Txcnt>500)
    {InitUart1();
    U1TxRx.RxTimes = 0;
	Uart1.RxByteF = 0;
	LATFbits.LATF6 = 0;	
    Uart1.RxFrameF=1;
    Uart1.RxFrameF1=1;
    }
	if(Uart1.RxFrameF==1 && Uart1.RxFrameF1==1)
	{   
		Uart1.RxFrameF = 0;	
        Uart1.RxFrameF1=0;
        U1TxRx.Txcnt=0;
		Uart1TxModbus();
	}
    if(Uart1.RxFrameF==1 && Uart1.RxFrameF2==1)
	{   
		Uart1.RxFrameF = 0;	
        Uart1.RxFrameF2=0;
        U1TxRx.Txcnt=0;
		Uart1TxModbus2();
	}
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Uart1TxModbus(void)
{
	unsigned int i;
	i = 30;
	EEPROMADDR = 0xFC00 + 2 * i;
    ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	i = 31;
	EEPROMADDR = 0xFC00 + 2 * i;
    ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
   

	U1TxRx.TxData[0] = StartParams.modbusaddr;    //从站地址为1
	U1TxRx.TxData[1] = 0x03;     //读功能码
	U1TxRx.TxData[2] = 0x0E;      //响应14个字节
	U1TxRx.TxData[3] = SysStatus >> 8;
	U1TxRx.TxData[4] = SysStatus;
	U1TxRx.TxData[5] = Fault.Byte >> 8;
	U1TxRx.TxData[6] = Fault.Byte;


  if(SysStatus != START&&MainParams.Ia<=5)
    {U1TxRx.TxData[7] =0;
     U1TxRx.TxData[8] =0;
    }
  if(SysStatus != START&&MainParams.Ia>5)
    {U1TxRx.TxData[7] = MainParams.Ia >> 8; //电流
     U1TxRx.TxData[8] = MainParams.Ia;
    }
	U1TxRx.TxData[9] = MainParams.Ua >> 8; //电压
	U1TxRx.TxData[10] = MainParams.Ua;

	U1TxRx.TxData[11] = MainParams.Temperature >> 8;//温度
	U1TxRx.TxData[12] = MainParams.Temperature;


	U1TxRx.TxData[13] = MainParams.SaveParams[30] >> 8;
	U1TxRx.TxData[14] = MainParams.SaveParams[30];
	U1TxRx.TxData[15] = MainParams.SaveParams[31] >> 8;
	U1TxRx.TxData[16] = MainParams.SaveParams[31];

	U1TxRx.TxDataCrc = CRC16_Check(&U1TxRx.TxData[0],17);
	
	U1TxRx.TxData[17] = U1TxRx.TxDataCrc >> 8;
	U1TxRx.TxData[18] = U1TxRx.TxDataCrc;

	U1TxRx.TxTimes = 20; //发送字节次数
	U1TxRx.TxDataCnt = 0; //当前次数清0
	U1TXREG = U1TxRx.TxData[0];	
}	

void Uart1TxModbus2(void)
{

   

	U1TxRx.TxData[0] = StartParams.modbusaddr;    //从站地址为1
	U1TxRx.TxData[1] = 0x02;     //读功能码
	U1TxRx.TxData[2] = 0x02;      //响应14个字节
	U1TxRx.TxData[3] = Fault.Byte >> 8;
	U1TxRx.TxData[4] = Fault.Byte;


  

	U1TxRx.TxDataCrc = CRC16_Check(&U1TxRx.TxData[0],5);
	
	U1TxRx.TxData[5] = U1TxRx.TxDataCrc >> 8;
	U1TxRx.TxData[6] = U1TxRx.TxDataCrc;

	U1TxRx.TxTimes = 8; //发送字节次数
	U1TxRx.TxDataCnt = 0; //当前次数清0
	U1TXREG = U1TxRx.TxData[0];	
}	
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void InitUart1(void)
{	
	U1BRG = BAUD_RATE;

	U1MODE = 0;
	U1MODEbits.UARTEN = 1;			/*1 = Enable UARTx; UARTx pins are controlled by UARTx module*/
	U1MODEbits.USIDL = 1;			/*1 = Discontinue operation when device enters Idle mode*/
	U1MODEbits.ABAUD = 0;			/*0 = Baud rate measurement disabled or completed*/
	U1MODEbits.PDSEL = 0;			/*00 = 8-bit data, no parity*/
	U1MODEbits.STSEL = 0;			/*0 = 1 Stop bit*/

	U1STA = 0;
	U1STAbits.UTXEN = 1; 			/*1 = transmitter enabled, UxTX pin controlled by UARTx (UARTEN=1)*/
	U1STAbits.UTXISEL = 1;			/*0 = Interrupt when a character is transferred to the Transmit 
									Shift register
									1 = Interrupt when a character is transferred to the Transmit 
									Shift register and as result, the transmit buffer becomes empty*/
	U1STAbits.URXISEL = 0;			/*0x =Interrupt flag bit is set when a character is received*/
	 
	IFS0bits.U1TXIF = 0;
	IFS0bits.U1RXIF = 0;
	IPC2bits.U1TXIP = 4;
	IPC2bits.U1RXIP = 4;
	IEC0bits.U1TXIE = 1;				// 1 允许U1TX中断  
	IEC0bits.U1RXIE = 1;				// 1 允许U1RX中断 

	LATF = 0;
	TRISFbits.TRISF6 = 0;
	LATFbits.LATF6 = 1;				//RS485 RX ENABLE	
	
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U1RXInterrupt (void)
{
	if(U1STAbits.OERR == 1) 
	{
		U1STAbits.OERR = 0; // Clear Overrun Error to receive data
	}
	Uart1.RxByteF = 1;
	U1TxRx.RxIntervalCnt = 0;
	if(!Uart1.RxFrameF)
	{
	if (U1STAbits.FERR ==0 )
	{
		if(U1TxRx.RxTimes == 0)
		{
			U1TxRx.RxAddr =	 U1RXREG;
            
			if(U1TxRx.RxAddr == StartParams.modbusaddr)
			{
				U1TxRx.RxTimes = 1;
			}
			else
			{
				U1TxRx.RxTimes = 0;
			}
		}
		else if(U1TxRx.RxTimes == 1)
		{		
			
			U1TxRx.RxFnctNbr = U1RXREG; 
			
			if(U1TxRx.RxFnctNbr ==0x03||U1TxRx.RxFnctNbr ==0x06)
			{
				U1TxRx.RxTimes = 2;
                Uart1.RxFrameF1=1;
			}
   			else if(U1TxRx.RxFnctNbr ==0x02)
			{
				U1TxRx.RxTimes = 2;
                Uart1.RxFrameF2=1;
			}
			else
			{
				U1TxRx.RxTimes = 0;
			}
		
		}
		else if(U1TxRx.RxTimes == 2)
		{
			U1TxRx.RxTimes = 3;
			U1TxRx.RxDataAddrHi  = U1RXREG;
		}     
		else if(U1TxRx.RxTimes == 3)
		{
			U1TxRx.RxTimes = 4;
			U1TxRx.RxDataAddrLo = U1RXREG;			
		}
		else if(U1TxRx.RxTimes == 4)
		{
			U1TxRx.RxTimes = 5;
			U1TxRx.RxDataCntHi = U1RXREG;
		}
		else if(U1TxRx.RxTimes == 5)
		{
			U1TxRx.RxTimes = 6;
			U1TxRx.RxDataCntLo = U1RXREG;
		}
		else if(U1TxRx.RxTimes == 6)
		{
			U1TxRx.RxTimes = 7;
			U1TxRx.RxDataCrcHi = U1RXREG;
		}
		else if(U1TxRx.RxTimes == 7)
		{
			U1TxRx.RxDataCrcLo = U1RXREG;

            U1TxRx.RxData[0] = U1TxRx.RxAddr;
            U1TxRx.RxData[1] = U1TxRx.RxFnctNbr;
            U1TxRx.RxData[2] = U1TxRx.RxDataAddrHi;
            U1TxRx.RxData[3] = U1TxRx.RxDataAddrLo;
            U1TxRx.RxData[4] = U1TxRx.RxDataCntHi;
            U1TxRx.RxData[5] = U1TxRx.RxDataCntLo;
            U1TxRx.RxData[6] = U1TxRx.RxDataCrcHi;
            U1TxRx.RxData[7] = U1TxRx.RxDataCrcLo;
 
         U1TxRx.RxCalCrc = CRC16_Check(&U1TxRx.RxData[0],6);
         U1TxRx.RxDataCrc =U1TxRx.RxData[7]+(U1TxRx.RxData[6]<<8);


             if(U1TxRx.RxFnctNbr ==0x06)
                    {
                     if(U1TxRx.RxDataAddrHi==0 && U1TxRx.RxDataAddrLo==0x64)
                       {
                        if(U1TxRx.RxDataCntHi==0 &&U1TxRx.RxDataCntLo==0xAA)
                           {Input.Start = 1;}
                              
                  
                      }
                    }
             if(U1TxRx.RxFnctNbr ==0x06)
                    {
                     if(U1TxRx.RxDataAddrHi==0 && U1TxRx.RxDataAddrLo==0x65)
                       {
                        if(U1TxRx.RxDataCntHi==0 &&U1TxRx.RxDataCntLo==0xBB)
                           {Input.Stop = 1;}
                              
                  
                      }
                    }
           
                
          if(U1TxRx.RxCalCrc==U1TxRx.RxDataCrc)
           {
			Uart1.RxFrameF = 1;
				
			U1TxRx.RxTimes = 0;
			Uart1.RxByteF = 0;
			LATFbits.LATF6 = 0;	}
		}
		else 
		{
			U1TxRx.RxTimes = 0;
			U1TxRx.RxAddr = U1RXREG;
		}

		}
	}
	else
	{U1TxRx.RxAddr = U1RXREG;}	
	IFS0bits.U1RXIF = 0;
	return;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U1TXInterrupt (void)
{
            //U1TxRx.RxAddr=0;
           // U1TxRx.RxFnctNbr=0;
            //U1TxRx.RxDataAddrHi=0;
            //U1TxRx.RxDataAddrLo=0;
           // U1TxRx.RxDataCntHi=0;
           // U1TxRx.RxDataCntLo=0;
           // U1TxRx.RxDataCrcHi=0;
          //  U1TxRx.RxDataCrcLo=0;
	IFS0bits.U1TXIF = 0;
	U1TxRx.TxDataCnt++;
	if(U1TxRx.TxDataCnt < U1TxRx.TxTimes )
	{
		U1TXREG = U1TxRx.TxData[U1TxRx.TxDataCnt];
	}
	else
	{
		LATFbits.LATF6 = 1;				//RS485 RX ENABLE
	}

	return;	
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*/
//高字节地位输出，低字节高位输出
unsigned int CRC16_Check(unsigned char *Pushdata,unsigned char length)  
{  
  unsigned int  Reg_CRC = 0xffff;  
  unsigned char Temp_reg = 0x00;  
  unsigned char i,j;   
 	
  for( i = 0; i<length ;i++)
   {  
   Reg_CRC^= *Pushdata++;  
   for (j = 0; j<8; j++)  
   {       
   	if (Reg_CRC & 0x0001)  
   	Reg_CRC=Reg_CRC>>1^0xA001;  
	else  
	Reg_CRC >>=1;  
   }    
   }  
   Temp_reg=Reg_CRC>>8;  
   return (Reg_CRC<<8|Temp_reg);  
} 


