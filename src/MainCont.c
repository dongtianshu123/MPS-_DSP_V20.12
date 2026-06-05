//�差������ҵ���ƹɷ����޹�˾����ѹ��̬��װ�ã�DSPIC30F5011ϵͳ
//���ļ���MainCont.c

//ͷ�ļ�
#include "UserParams.h"	//�û�ϵͳ���ã��������弰����ֵ
#include "Function.h"	//ϵͳ�ⲿ���ú����������ڲ����ú����ڶ�Ӧ�ļ�������
#include "variable.h"	//ϵͳȫ�ֱ����������ֲ������ڶ�Ӧ�ļ�������
#include <p30fxxxx.h>

tMainParams MainParams;  
tStartParams StartParams;
tProtectParams ProtectParams;  
tAdcParams AdcParams;
tStartState StartState;
tFault Fault;
tFunctionswitch Functionswitch;
tProtectswitch Protectswitch;
tProtectswitch1 Protectswitch1;
tzcd_mgr zcd_mgr;
tModbus_Ctrl MB;

unsigned int powerOnDly1;
unsigned int lowVoltageTest;
unsigned int lowVoltageCnt;
unsigned int Ia_imcont;
unsigned int Ic_imcont;
extern unsigned int   Ia_im;
extern unsigned int   Ic_im;



void switch_signal_source(void)
{
  if(zcd_mgr.switch_simulated>0) //�л�ģ���ź�
     {
       if(zcd_mgr.use_simulated==0)
          {
            zcd_mgr.use_simulated=1;
             T4CONbits.TON=1;
          } 
     }
  else if(zcd_mgr.switch_simulated==0) //�л�ʵ���ź�
     {
        if(zcd_mgr.use_simulated>0)
           {
             zcd_mgr.use_simulated=0;
             T4CONbits.TON=0;
           }
     }
     
}

void monitor_signal_quality(void)
{
 static unsigned char stable_count=0;
  if(!zcd_mgr.signal_quality)
    {
      zcd_mgr.switch_simulated=1;
      switch_signal_source();
      stable_count=0;
    }
  else
   {
     stable_count++;
     //if(stable_count >3 && zcd_mgr.use_simulated)
     if(zcd_mgr.use_simulated)
       {
        zcd_mgr.switch_simulated=0;
        switch_signal_source();
         }


   }

}
 
//�����趨����EEPROM�洢���飬64������
//�����趨����EEPROM�洢���飬64������
unsigned int _EEDATA(128) EPConfigS[128]={
							24,60,350,57,	//�𶯵�ѹ ����ʱ�� �������� �����
							2,100,300,63,	//��ʽ ��ѹ�궨 �����궨����  �л�ʱ��
							0,500,50,35,	//ͣ��ʱ��	�������� ��ƽ�����	�¶� 
							40,60,120,85,		//ͣ����ʼ��ѹ ͣ����ֹ��ѹ ��ѹ���� ��ѹ����
															
							
							70,20,0,5000, //     ͻ����ѹ ͻ����ʱ �𶯹�Ƶ���͵�������ʱ��
                             8000,6600,4,1,     // ��λ�ȶ�����D1����λ�ȶ�����D2,K1,K2,
							 2,50,1,9600,     //      K3,K4,modbus��վ��,modbus������,
                             75,512,55,40,  //	X,X,�ϴι��� ,����ʱ��	   

                             21,25,34,35,    //�Զ���-Vb       V1       V2       V3
                             35,5,3,1000,    //         V4       V5       T1       Ƿ������ֵ
                             300,300,3,10,      //         Ƿ����T3��ʱ      ������ʱ        �л���ʱ       ����ֵ
                             6000,50,500,10,      //        ������ʱ  ����CT  ���й�����ֵ ���й�����ʱ
                             100,3,95,20,     //        ������ʱ     ����     ����ѹ��   ��������ʱ��
                            10,30,10,50,//        ������ʱ ������ƽ�� ��ѹ��ʱ  Ƿѹ��ʱ
							0,0,0,0,        //        K5        K6      K7        K8
                            130,65            //       s-class   r-class
                         };
unsigned int EEPROMADDR;	//EEPROM�洢��ַ����
unsigned int Dsp_State_Start=0;


_FOSC(CSW_FSCM_OFF & XT_PLL8);
// P0-1: 启用看门狗 WDT — 超时≈284ms (主循环10ms,足够)
_FWDT(WDT_EN & WDT_POST_1024 & WDT_PRE_256);
_FBS(NO_BOOT_RAM);
_FSS(NO_SEC_RAM);
//_FGS(HIGH_PROT);
_FGS(GEN_PROT);
__FICD( ICS_PGD1 );
//***********************************������ ***********************************************************//
int main(void)
{
	Delay60ms();
//	U1TxRx.TxData[0] = 0xaa;
//	U1TxRx.TxData[1] = 0x04;
//	U1TxRx.TxData[3] = 0; 
//	U1TxRx.TxData[4] = 0;
//	U1TxRx.TxData[5] = 3;
//
//
//	U1TxRx.TxDataCrc = CRC16_Check(&U1TxRx.TxData,6);
//	
//	U1TxRx.TxData[11] = U1TxRx.TxDataCrc >> 8;
//	U1TxRx.TxData[12] = U1TxRx.TxDataCrc;
//	Delay3s();
	while(!RCONbits.BOR);
	StateInit();
    SetupPorts();	
    InitTmr1();
	InitTmr2();
    InitAD();	
	InitIC();
	InitUART2();
	InitUart1();
	InitCN();
	SET_CPU_IPL(3);
	//PowerONTest();

ProtectParams.intevalmin=StartParams.StartOverFre;  //��ǰ�����𶯼��дΪ�������õ��𶯼��
	while(1)
    {
	    
	    asm("CLRWDT");
        
       
    	if(SysTime.Ms1)
		{   
			SysTime.Ms1 = 0;
          	
		}

  
		
		if(SysTime.AdMs100 )
		{   SysTime.AdMs100 = 0; 
            
            
            NoPowerCheck();
            StartOFTProtect();   //�𶯹�Ƶ����
            SR_overload();   //˫���ӹ������߼���
            //abc_dq();

         if((step11==3 && Protectswitch.S_UnbalanceI==1) || (step11==4 && Protectswitch.R_UnbalanceI==1))   //������ƽ��
         {UnbalanceIProtect();}

               IORead();

			if(lowVoltageTest==0xaaaa && 
		   	StartParams.ControlMode==0 &&
		 	MainParams.Ua<30)
			{StartState.Test = 1;}
			else
			{StartState.Test = 0;}
		}

		if(SysTime.Ms5)
        {

           SysTime.Ms5=0;
        }





		if(SysTime.Ms10)
		{   
//            monitor_signal_quality();
            AdcDescend();
            CurrentCheck();
			if(!StartState.Test)
			{
              Protect();
            }
            RelayFlow();
		     if(step11== 3) //�����𶯲���
			{
				SoftStart();
			}
                 
			if(step11== 12)
			{SoftStop1Ms();}
            if(step11== 18)
			{SoftStop2();}
            phasecompare();
            Frequencydetect1();
			SysTime.Ms10 = 0;
			
   
			Uart2RxErrApp();
		//	Uart1RxErrApp();
			Uart2App();
			//Uart1App();
            Modbus_Slave_App();
            Update_Modbus_Registers();
		}
	}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*
void PowerONTest()
{
	powerOnDly1 = 0;
	lowVoltageTest = 0;
	lowVoltageCnt = 0;
	while(powerOnDly1 < 5) 
	{
		if(SysTime.Ms10)
		{
			SysTime.Ms10 = 0;
			IORead();
			if(Input.NoUse)
		    {lowVoltageCnt++;}
			else
			{lowVoltageCnt = 0;}
			if(lowVoltageCnt > 10)
			{lowVoltageTest = 0xaaaa;}
		}	
		
		if(SysTime.Ms200)
		{
			SysTime.Ms200 = 0;	
			powerOnDly1++;								
		}
	}	
}
*/