//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//系统初始化函数，init4011.c

//头文件
#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明
/*
*********************************************************************************************************
* LOCAL DEFINES
*********************************************************************************************************
*/
#define 	Cof_U10K	100;	
#define 	Cof_U6K    	60;
unsigned int PULSEWidth;
extern unsigned int StartOFTOK;
/*
*********************************************************************************************************
* LOCAL DEFINES
*********************************************************************************************************
*/	
void StateInit(void)
{

	unsigned int i = 0;
	
//************* 读运行参数 ******************
	for(i = 0; i < 62; i ++)
    {
    	EEPROMADDR = 0xFC00 + 2 * i;
    	ReadEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR,&MainParams.SaveParams[i], WORD);
	}
/*************************触发延时最小值*********************************************/
StartParams.umin=MainParams.SaveParams[48];
if (StartParams.umin==0)
{StartParams.umin=3333;}
StartParams.Ugmin=(long)StartParams.umin <<10;

 int  temp12=MainParams.SaveParams[29]&0x1000; //50Hz60Hz切换功能
if (temp12==0x1000)
  {Functionswitch.Fre=1;}
else
  {Functionswitch.Fre=0;}

//************* //初始电压	 ******************		
	if(MainParams.SaveParams[0]>70)
	{MainParams.SaveParams[0]=21;}
	else if(MainParams.SaveParams[0]<10)
	{MainParams.SaveParams[0]=21;}
	else
	{Nop();}

	MainParams.SaveParams[0] = MainParams.SaveParams[0]*100;
    if(Functionswitch.Fre==0)
      {
    	StartParams.BeginVoltage = 8000 - MainParams.SaveParams[0];//;;;;;;;7500
	    StartParams.BeginVoltage = StartParams.BeginVoltage<<10;

       }
     else
      {
        StartParams.BeginVoltage = 6666 - MainParams.SaveParams[0];//;;;;;;;7500
	    StartParams.BeginVoltage = StartParams.BeginVoltage<<10;
      }


//************* //启动时间	 ******************			
	if(MainParams.SaveParams[1] > 80)
	MainParams.SaveParams[1] = 40;

	StartParams.StartTime = MainParams.SaveParams[1] * 100;

	StartParams.LoopData = ( StartParams.BeginVoltage - StartParams.Ugmin) / 4000;
	StartParams.MaxLoopData = ( StartParams.BeginVoltage - StartParams.Ugmin) / MAXSTEP_TIME;
//************* //额定电流	 ******************			
	if(MainParams.SaveParams[3]>2000)
	{MainParams.SaveParams[3]=200;}
	else if(MainParams.SaveParams[3]<0)
	{MainParams.SaveParams[3]=200;}
	else
	{Nop();}
	ProtectParams.RatingCurrent = MainParams.SaveParams[3];	
//************* //恒流倍数	 ******************		
	if(MainParams.SaveParams[2]>500)
	{MainParams.SaveParams[2]=350;}
	else if(MainParams.SaveParams[2]<100)
	{MainParams.SaveParams[2]=350;}
	else
	{Nop();}
	MainParams.SaveParams[2] = MainParams.SaveParams[2]/10;
	StartParams.LimitCurrent = ( (long)ProtectParams.RatingCurrent * MainParams.SaveParams[2]) / 10;				
//************* //启动方式	 ******************		
	if(MainParams.SaveParams[4]>3)
	{MainParams.SaveParams[4]=0;}
	else if(MainParams.SaveParams[4]<0)
	{MainParams.SaveParams[4]=0;}
	else
	{Nop();}
	StartParams.ControlMode = MainParams.SaveParams[4];
//************* 读标定参数 ******************	
    	

	AdcParams.CofU = MainParams.SaveParams[5];





	AdcParams.CofI = MainParams.SaveParams[6]/5;
		
	StartParams.SwitchTime = MainParams.SaveParams[7];

	if(MainParams.SaveParams[8]>60)
	{MainParams.SaveParams[8] = 20;}
	StartParams.StopTime = MainParams.SaveParams[8]*100;          //停止时间

	ProtectParams.CurrentUpperLimit = MainParams.SaveParams[9];  //电流上限

	if(MainParams.SaveParams[10]>300)
	{MainParams.SaveParams[10]=50;}
	else if(MainParams.SaveParams[10]<10)
	{MainParams.SaveParams[10]=50;}
	else
	{Nop();}
	ProtectParams.ImbalanceDegree = MainParams.SaveParams[10] ;
    
	if(MainParams.SaveParams[11]>70)
	{MainParams.SaveParams[11] = 35;}
	ProtectParams.Temperature = MainParams.SaveParams[11];

	if(MainParams.SaveParams[12]>70)
	{MainParams.SaveParams[12]=40;}
	else if(MainParams.SaveParams[12]<30)
	{MainParams.SaveParams[12]=40;}
	else
	{Nop();}
	StartParams.StopHead = MainParams.SaveParams[12]*100;
	
	if(MainParams.SaveParams[13]>70)
	{MainParams.SaveParams[13]=60;}
	else if(MainParams.SaveParams[13]<30)
	{MainParams.SaveParams[13]=60;}
	else
	{Nop();}
	StartParams.StopEnd = MainParams.SaveParams[13]*100;

	if(StartParams.StopEnd > StartParams.StopHead)
	{StartParams.StopDif = StartParams.StopEnd - StartParams.StopHead;}
	else
	{StartParams.StopDif = StartParams.StopEnd - StartParams.StopHead;}

    StartParams.StopDif = StartParams.StopDif<<10;
	StartParams.StopStep = StartParams.StopDif / StartParams.StopTime;

	StartParams.StopHead = StartParams.StopHead<<10;
	StartParams.StopEnd = StartParams.StopEnd<<10;

//************* 电压上限 ******************
StartParams.VoltageUpperLimit=MainParams.SaveParams[14];


//************* 电压下限 ******************
StartParams.VoltageLowerLimit=MainParams.SaveParams[15];


//************* 突跳电压******************
StartParams.StepVoltage=MainParams.SaveParams[16];


//************* 突跳延时 ******************
StartParams.StepDelay=MainParams.SaveParams[17];


//************* 起动过频 ******************
StartParams.StartOverFre=MainParams.SaveParams[18];

if(StartOFTOK==0)
{
ProtectParams.intevalmin=MainParams.SaveParams[18];//将起动过频时间写入起动间隔保护倒计时
ProtectParams.intevalsec=0;

}
//************* 低电流保护时间 ******************
StartParams.LCockedDelay=MainParams.SaveParams[19];


//************* 热容量保护蓄热系数*****************
//StartParams.ThermalStorageCoe=MainParams.SaveParams[20];


//************* 热容量保护散热系数 ******************
//StartParams.ThermalDissipationCoe=MainParams.SaveParams[21];

//************* 相位比对延时上限D1系数*****************
StartParams.D1=MainParams.SaveParams[20];


//************* 相位比对延时下限D2系数 ******************
StartParams.D2=MainParams.SaveParams[21];


//************* 恒流step1比例系数K1 ******************
StartParams.K1=MainParams.SaveParams[22];

//************* 恒流step2比例系数K2 ******************
StartParams.K2=MainParams.SaveParams[23];

//************* 恒流step3比例系数K3 ******************
StartParams.K3=MainParams.SaveParams[24];

//************* 恒流step4比例系数K4 ******************
StartParams.K4=MainParams.SaveParams[25];

//************* modbus站号 ******************
StartParams.modbusaddr=MainParams.SaveParams[26];

//************* modbus波特率 ******************
StartParams.modbusbaud=MainParams.SaveParams[27];
//************* 额定功率因数 ******************
StartParams.cosa=MainParams.SaveParams[28];
//************* 压板 ******************
int temp=MainParams.SaveParams[29]&0x0001;
if (temp==0x0001)
  {Functionswitch.phasecompareswitch=1;}
else
  {Functionswitch.phasecompareswitch=0;}

 int  temp1=MainParams.SaveParams[29]&0x0002;
if (temp1==0x0002)
  {Functionswitch.autotrigger=1;}
else
  {Functionswitch.autotrigger=0;}

 int  temp2=MainParams.SaveParams[29]&0x0004;
if (temp2==0x0004)
  {Functionswitch.stoptrip=1;}
else
  {Functionswitch.stoptrip=0;}

 int  temp3=MainParams.SaveParams[29]&0x0008; //单点起动压板
if (temp3==0x0008)
  {Functionswitch.singlestart=1;}
else
  {Functionswitch.singlestart=0;}

 int  temp4=MainParams.SaveParams[29]&0x0010; //温度检测
if (temp4==0x0010)
  {Functionswitch.TDetection=1;}
else
  {Functionswitch.TDetection=0;}

 int  temp5=MainParams.SaveParams[29]&0x0020; //额定电流旁路
if (temp5==0x0020)
  {Functionswitch.Ieswitch=1;}
else
  {Functionswitch.Ieswitch=0;}

 int  temp6=MainParams.SaveParams[29]&0x0040; //运行反馈检测
if (temp6==0x0040)
  {Functionswitch.Runcheckswitch=1;}
else
  {Functionswitch.Runcheckswitch=0;}

 int  temp7=MainParams.SaveParams[29]&0x0080; //自定义曲线旁路
if (temp7==0x0080)
  {Functionswitch.Customswitch=1;}
else
  {Functionswitch.Customswitch=0;}



 int  temp9=MainParams.SaveParams[29]&0x0200; //正反转检测
if (temp9==0x0200)
  {Functionswitch.Auto_turn=1;}
else
  {Functionswitch.Auto_turn=0;}

if(Functionswitch.Auto_turn==0)
{
 int  temp8=MainParams.SaveParams[29]&0x0100; //正反转切换 如果自动检测投入，就不读取设定的转向
if (temp8==0x0100)
  {Functionswitch.FWD_REV=1;}
else
  {Functionswitch.FWD_REV=0;}
}

 int  temp10=MainParams.SaveParams[29]&0x0400; //突跳起动19.61
if (temp10==0x0400)
  {Functionswitch.Step=1;}
else
  {Functionswitch.Step=0;}


 int  temp11=MainParams.SaveParams[29]&0x0800; //X6复位功能
if (temp11==0x0800)
  {Functionswitch.X6reset=1;}
else
  {Functionswitch.X6reset=0;}

// int  temp12=MainParams.SaveParams[29]&0x1000; //50Hz60Hz切换功能
//if (temp12==0x1000)
//  {Functionswitch.Fre=1;}
//else
//  {Functionswitch.Fre=0;}


if(Functionswitch.Fre==0)
{AdcParams.FILTER_N=100;
 AdcParams.FILTER_N1=50;

}
else
{AdcParams.FILTER_N=84;
 AdcParams.FILTER_N1=42;


}

 int  temp13=MainParams.SaveParams[29]&0x2000; //零序电流显示
if (temp13==0x2000)
  {Functionswitch.Delay_Zero=1;}
else
  {Functionswitch.Delay_Zero=0;}

 int  temp14=MainParams.SaveParams[29]&0x4000; //接触器
if (temp14==0x4000)
  {Functionswitch.Contactor=1;}
else
  {Functionswitch.Contactor=0;}

 int  temp16=MainParams.SaveParams[29]&0x8000; //禁止起动
if (temp16==0x8000)
  {Functionswitch.FBstart=1;}
else
  {Functionswitch.FBstart=0;}


//************* 读上次故障状态 ******************
	MainParams.FaultLastTime = MainParams.SaveParams[30];
	MainParams.LastStartTime = MainParams.SaveParams[31];

//***************自定义曲线********************************************************************
StartParams.Vb=MainParams.SaveParams[32]*102400;//导通角乘以倍率
StartParams.V1=MainParams.SaveParams[33]*102400;
StartParams.V2=MainParams.SaveParams[34]*102400;
StartParams.V3=MainParams.SaveParams[35]*102400;
StartParams.V4=MainParams.SaveParams[36]*102400;
StartParams.V5=MainParams.SaveParams[37];//231109 修改为电流振荡抑制系数，旁路切换时导通角增加幅度


StartParams.t1=MainParams.SaveParams[38]*100;
StartParams.Lowercurrentset=MainParams.SaveParams[39];//260115 修改为欠电流保护定值
StartParams.Lowercurrentdelay=MainParams.SaveParams[40];//260115 修改为欠电流保护延时
StartParams.OverTdelay=MainParams.SaveParams[41];//260115 修改为过温延时
StartParams.t5=MainParams.SaveParams[42]*100;//231109 修改为切换延时，旁路切换时判断以10ms为单位的延时







 StartParams.Vb_data= 8000-(MainParams.SaveParams[32]*100);
 StartParams.Vb_data = StartParams.Vb_data<<10;//定义晶闸管输出的初始触发角

if(StartParams.V1>=StartParams.Vb &&StartParams.t1>0)
{StartParams.LoopData1 = ( StartParams.V1 - StartParams.Vb) / StartParams.t1;}//自定义曲线第一段触发角变化的步长
else
{StartParams.LoopData1=0;}

if(StartParams.V2>=StartParams.V1 &&StartParams.t2>0)
{StartParams.LoopData2 = ( StartParams.V2 - StartParams.V1) / StartParams.t2;}
else
{StartParams.LoopData2=0;}

if(StartParams.V3>=StartParams.V2 &&StartParams.t3>0)
{StartParams.LoopData3 = ( StartParams.V3 - StartParams.V2) / StartParams.t3;}
else
{StartParams.LoopData3=0;}

if(StartParams.V4>=StartParams.V3 &&StartParams.t4>0)
{StartParams.LoopData4 = ( StartParams.V4 - StartParams.V3) / StartParams.t4;}
else
{StartParams.LoopData4=0;}
/*
if(StartParams.V5>=StartParams.V4 &&StartParams.t5>0)
{StartParams.LoopData5 = ( StartParams.V5 - StartParams.V4) / StartParams.t5;}
else
{StartParams.LoopData5=0;}


if(StartParams.V6>=StartParams.V5 &&StartParams.t6>0)
{StartParams.LoopData6 = ( StartParams.V6 - StartParams.V5) / StartParams.t6;}
else
{StartParams.LoopData6=0;}

if(StartParams.V7>=StartParams.V6 &&StartParams.t7>0)
{StartParams.LoopData7 = ( StartParams.V7 - StartParams.V6) / StartParams.t7;}
else
{StartParams.LoopData7=0;}

if(StartParams.V8>=StartParams.V7 &&StartParams.t8>0)
{StartParams.LoopData8 = ( StartParams.V8 - StartParams.V7) / StartParams.t8;}
else
{StartParams.LoopData8=0;}

if(StartParams.V9>=StartParams.V8 &&StartParams.t9>0)
{StartParams.LoopData9 = ( StartParams.V9 - StartParams.V8) / StartParams.t9;}
else
{StartParams.LoopData9=0;}
*/

/*************************零序定值整定**********************************************/
StartParams.ZeroILimit = MainParams.SaveParams[43];
if (StartParams.ZeroILimit==0)
{StartParams.ZeroILimit=10;}

/*************************************************************************************/


/*************************零序保护延时**********************************************/
StartParams.ZeroIdelay = MainParams.SaveParams[44];
if (StartParams.ZeroIdelay==0)
{StartParams.ZeroIdelay=300;}

/*************************************************************************************/


/*************************零序互感器变比**********************************************/
AdcParams.CofI_zero = MainParams.SaveParams[45];
if (AdcParams.CofI_zero==0)
{AdcParams.CofI_zero=50;}

/*************************************************************************************/
/*************************运行过流定值整定**********************************************/
StartParams.ROCV=MainParams.SaveParams[46];
if (StartParams.ROCV==0)
{StartParams.ROCV=500;}

/*************************************************************************************/
/*************************运行过流延时整定**********************************************/
StartParams.ROCD=MainParams.SaveParams[47];
if (StartParams.ROCD==0)
{StartParams.ROCD=10;}


                                              

                                              //MainParams.SaveParams[49];
int temp35=MainParams.SaveParams[49]&0x0001;
if (temp35==0x0001)
  {Protectswitch.runcheckswitch=1;}
else
  {Protectswitch.runcheckswitch=0;}

 int  temp36=MainParams.SaveParams[49]&0x0002;
if (temp36==0x0002)
  {Protectswitch1.runchecktrip=1;}
else
  {Protectswitch1.runchecktrip=0;}

int temp20=MainParams.SaveParams[49]&0x0004;
if (temp20==0x0004)
  {Protectswitch1.overtem=1;}
else
  {Protectswitch1.overtem=0;}

 temp20=MainParams.SaveParams[49]&0x0008;
if (temp20==0x0008)
  {Protectswitch1.RLI=1;}
else
  {Protectswitch1.RLI=0;}


/*******************************保护压板******************************************************/

 temp20=MainParams.SaveParams[50]&0x0001;
if (temp20==0x0001)
  {Protectswitch.SOV=1;}
else
  {Protectswitch.SOV=0;}

 int  temp21=MainParams.SaveParams[50]&0x0002;
if (temp21==0x0002)
  {Protectswitch.SLV=1;}
else
  {Protectswitch.SLV=0;}

 int  temp22=MainParams.SaveParams[50]&0x0004;
if (temp22==0x0004)
  {Protectswitch.SOC=1;}
else
  {Protectswitch.SOC=0;}

 int  temp23=MainParams.SaveParams[50]&0x0008; 
if (temp23==0x0008)
  {Protectswitch.S_UnbalanceI=1;}
else
  {Protectswitch.S_UnbalanceI=0;}

int temp24=MainParams.SaveParams[50]&0x0010;
if (temp24==0x0010)
  {Protectswitch.S_SCR=1;}
else
  {Protectswitch.S_SCR=0;}

 int  temp25=MainParams.SaveParams[50]&0x0020;
if (temp25==0x0020)
  {Protectswitch.S_ZeroI=1;}
else
  {Protectswitch.S_ZeroI=0;}

 int  temp26=MainParams.SaveParams[50]&0x0040;
if (temp26==0x0040)
  {Protectswitch.S_PowerOFF=1;}
else
  {Protectswitch.S_PowerOFF=0;}

 int  temp27=MainParams.SaveParams[50]&0x0080; 
if (temp27==0x0080)
  {Protectswitch.ROV=1;}
else
  {Protectswitch.ROV=0;}

int temp28=MainParams.SaveParams[50]&0x0100;
if (temp28==0x0100)
  {Protectswitch.RLV=1;}
else
  {Protectswitch.RLV=0;}

 int  temp29=MainParams.SaveParams[50]&0x0200;
if (temp29==0x0200)
  {Protectswitch.ROC=1;}
else
  {Protectswitch.ROC=0;}

 int  temp30=MainParams.SaveParams[50]&0x0400;
if (temp30==0x0400)
  {Protectswitch.R_UnbalanceI=1;}
else
  {Protectswitch.R_UnbalanceI=0;}

 int  temp31=MainParams.SaveParams[50]&0x0800; 
if (temp31==0x0800)
  {Protectswitch.R_ZeroI=1;}
else
  {Protectswitch.R_ZeroI=0;}

 int  temp32=MainParams.SaveParams[50]&0x1000; 
if (temp32==0x1000)
  {Protectswitch.S_overload=1;}
else
  {Protectswitch.S_overload=0;}

 int  temp33=MainParams.SaveParams[50]&0x2000; 
if (temp33==0x2000)
  {Protectswitch.R_overload=1;}
else
  {Protectswitch.R_overload=0;}

 int  temp34=MainParams.SaveParams[50]&0x4000; 
if (temp34==0x4000)
  {Protectswitch.sampletest=1;}
else
  {Protectswitch.sampletest=0;}



/*************************电流上升时间**********************************************/

StartParams.I_increase=MainParams.SaveParams[51]*10;
if (StartParams.I_increase==0)
{StartParams.I_increase=150;}

/*************************************************************************************/
/*************************过流延时整定**********************************************/
StartParams.OverIdelay=MainParams.SaveParams[52];
if (StartParams.OverIdelay==0)
{StartParams.OverIdelay=10;}

/*************************************************************************************/
/*************************电流不平衡延时整定**********************************************/
StartParams.UnbalanceIdelay=MainParams.SaveParams[53];
if (StartParams.UnbalanceIdelay==0)
{StartParams.UnbalanceIdelay=100;}

/*************************************************************************************/
/*************************过压延时整定**********************************************/
StartParams.Overvoltagedelay=MainParams.SaveParams[54];
if (StartParams.Overvoltagedelay==0)
   { StartParams.Overvoltagedelay=100;}

/*************************************************************************************/

/*************************欠压延时整定**********************************************/
StartParams.Lowervoltagedelay = MainParams.SaveParams[55];
if (StartParams.Lowervoltagedelay==0)
{StartParams.Lowervoltagedelay=100;}

/*************************************************************************************/

/*************************Ready_relay K5**********************************************/
StartParams.K5_relay = MainParams.SaveParams[56];


/*************************************************************************************/


/*************************Alarm_relay K6**********************************************/
StartParams.K6_relay = MainParams.SaveParams[57];


/*************************************************************************************/


/*************************Trip_relay K7**********************************************/
StartParams.K7_relay = MainParams.SaveParams[58];


/*************************************************************************************/



/*************************no_use_relay K8**********************************************/
StartParams.K8_relay = MainParams.SaveParams[59];


/*************************************************************************************/

/*************************起动曲线**********************************************/
StartParams.start_class = MainParams.SaveParams[60];


/*************************************************************************************/
/*************************运行曲线**********************************************/
StartParams.run_class = MainParams.SaveParams[61];

/*************************************************************************************/







	StartParams.StartUpCurrentLimit = ( (long)ProtectParams.RatingCurrent * 10) / 100;
	StartParams.StartDownCurrentLimit = ( (long)ProtectParams.RatingCurrent * 20) / 100;
	ProtectParams.FastCurrent = ( (long)StartParams.LimitCurrent * 93) / 100;
	ProtectParams.FastCurrent1 = ( (long)StartParams.LimitCurrent * 98) / 100;
	
	ProtectParams.NoVoltage = MainParams.SaveParams[5]/10;			
	ProtectParams.NoCurrent = (long)ProtectParams.RatingCurrent / 10;

   ProtectParams.temp2= __builtin_muluu(ProtectParams.RatingCurrent,ProtectParams.RatingCurrent);
   ProtectParams.temp3= __builtin_muluu(80,StartParams.start_class);
   ProtectParams.temp3= ProtectParams.temp3*ProtectParams.temp2;
   ProtectParams.temp4= __builtin_muluu(80,StartParams.run_class);
   ProtectParams.temp4= ProtectParams.temp4*ProtectParams.temp2;

}




void SetupPorts( void )
{	
	TRISDbits.TRISD7 = 1;
	TRISFbits.TRISF0 = 1;
	TRISFbits.TRISF1 = 1;
	TRISGbits.TRISG1 = 1;
	TRISGbits.TRISG0 = 1;
	TRISDbits.TRISD11 = 1;

	LATC = 0;
	LATG = 0;
	LATD = 0;

	TRISGbits.TRISG14 = 0;
	TRISGbits.TRISG12 = 0;
	TRISGbits.TRISG13 = 0;
	TRISGbits.TRISG15 = 0;
	TRISCbits.TRISC1  = 0;


	TRISCbits.TRISC2  = 0;
	TRISGbits.TRISG6 = 0;
	TRISGbits.TRISG7 = 0;
	TRISGbits.TRISG8 = 0;
	TRISGbits.TRISG9 = 0;

	TRISDbits.TRISD3  = 0;
	TRISDbits.TRISD2  = 0;
	TRISDbits.TRISD1  = 0;
    OUTPUT_READY=0;			
    OUTPUT_START=0;
    OUTPUT_ALARM=0;

    OUTPUT_EXTERN_READY=0;
    OUTPUT_EXTERN_ALARM=0;
    OUTPUT_EXTERN_TRIP=0;
    SysStatus=WAIT;
}


void InitTmr1( void )
{
    T1CON = 0;					// 关闭定时器1
    TMR1 = 0;          			// clear timer1 register 
    T1CONbits.TSIDL = 0;        // 空闲模式下停止工作   
    T1CONbits.TCS = 0; 			// set internal clock source
	T1CONbits.TCKPS = 0;		// Tcy / 1 作为输入时钟  
	T1CONbits.TON = 1;          // 启动定时器1
    PR1 = dTImer1Tcy;			// set period1 register 
    IPC0bits.T1IP = 5; 			// set priority level 
    IFS0bits.T1IF = 0; 			//clear interrupt flag 
    IEC0bits.T1IE = 1; 			// enable interrupts 	
    
	SRbits.IPL = 3;    			//enable CPU priority levels 4-7????????????????
}





void InitCN(void)
{
    TRISDbits.TRISD6 = 1;
	TRISDbits.TRISD5 = 1;
	TRISDbits.TRISD4 = 1;

	CNEN1 = 0;
    CNEN2 = 0;
    CNEN1bits.CN13IE = 1; 				//开启 CN0 电平跳变引脚
    CNEN1bits.CN14IE = 1; 				//开启 CN1 电平跳变引脚
    CNEN1bits.CN15IE = 1; 				//开启 CN2 电平跳变引脚
   
    CNPU1 = 0;
    CNPU2 = 0;
    CNPU1bits.CN13PUE = 0;				//开启 CN0 电平变化上拉
    CNPU1bits.CN14PUE = 0;				//开启 CN1 电平变化上拉
    CNPU1bits.CN15PUE = 0;				//开启 CN2 电平变化上拉
  
    IPC3bits.CNIP = 5;              //设置中断优先级为5
    IFS0bits.CNIF = 0;              //清零中断标志
    IEC0bits.CNIE = 1;              //允许CN中断
    
}

