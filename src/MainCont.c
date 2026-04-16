//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//主文件，MainCont.c

//头文件
#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明
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
  if(zcd_mgr.switch_simulated>0) //切换模拟信号
     {
       if(zcd_mgr.use_simulated==0)
          {
            zcd_mgr.use_simulated=1;
             T4CONbits.TON=1;
          } 
     }
  else if(zcd_mgr.switch_simulated==0) //切换实际信号
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
 
//键盘设定参数EEPROM存储数组，64个参数
//键盘设定参数EEPROM存储数组，64个参数
unsigned int _EEDATA(128) EPConfigS[128]={
							24,60,350,57,	//起动电压 启动时间 启动倍数 额定电流
							2,100,300,63,	//方式 电压标定 电流标定参数  切换时间
							0,500,50,35,	//停车时间	电流上限 不平衡参数	温度 
							40,60,120,85,		//停车初始电压 停车终止电压 电压上限 电压下限
															
							
							70,20,0,5000, //     突跳电压 突跳延时 起动过频，低电流保护时间
                             8000,6600,4,1,     // 相位比对上限D1，相位比对下限D2,K1,K2,
							 2,50,1,9600,     //      K3,K4,modbus从站号,modbus波特率,
                             75,512,55,40,  //	X,X,上次故障 ,启动时间	   

                             21,25,34,35,    //自定义-Vb       V1       V2       V3
                             35,5,3,1000,    //         V4       V5       T1       欠电流定值
                             300,300,3,10,      //         欠电流T3延时      过温延时        切换延时       零序定值
                             6000,50,500,10,      //        零序延时  零序CT  运行过流定值 运行过流延时
                             100,3,95,20,     //        触发延时     备用     保护压板   电流上升时间
                            10,30,10,50,//        过流延时 电流不平衡 过压延时  欠压延时
							0,0,0,0,        //        K5        K6      K7        K8
                            130,65            //       s-class   r-class
                         };
unsigned int EEPROMADDR;	//EEPROM存储地址给定
unsigned int Dsp_State_Start=0;


_FOSC(CSW_FSCM_OFF & XT_PLL8);
_FWDT(WDT_OFF);
_FBS(NO_BOOT_RAM);
_FSS(NO_SEC_RAM);
//_FGS(HIGH_PROT);
_FGS(GEN_PROT);
__FICD( ICS_PGD1 );
//***********************************主程序 ***********************************************************//
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

ProtectParams.intevalmin=StartParams.StartOverFre;  //当前允许起动间隔写为参数设置的起动间隔
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
            StartOFTProtect();   //起动过频保护
            SR_overload();   //双电子过载曲线计算
            //abc_dq();

         if((step11==3 && Protectswitch.S_UnbalanceI==1) || (step11==4 && Protectswitch.R_UnbalanceI==1))   //电流不平衡
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

                 
			if(step11== 12)
			{SoftStop1Ms();}
            if(step11== 18)
			{SoftStop2();}
            phasecompare();
            Frequencydetect1();
			SysTime.Ms10 = 0;
			
            Update_Modbus_Registers();
			Uart2RxErrApp();
		//	Uart1RxErrApp();
			Uart2App();
			//Uart1App();
            Modbus_Slave_App();
            Write_Modbus_Command();
			
			if(step11== 3) //正常起动步骤
			{
				SoftStart();
			}
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