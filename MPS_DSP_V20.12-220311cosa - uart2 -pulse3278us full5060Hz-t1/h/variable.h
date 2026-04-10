//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//系统全局变量声明，局部变量在对应文件内声明  variable.h


#ifndef variable_H
#define variable_H   
//#define TEST_AUTOREADY
//#define TEST_STOP_ON_START_OFF
//#define TEST_LOW_VOL
//#define TEST_NO_RUNCHECK
//#define TEST_AUTOZERO
#include <p30F5011.h>
extern unsigned char ICflag;
extern unsigned char ICcnt;

//****************** 独立全局变量**********************
extern unsigned int EEPROMADDR;		//地址给定 
extern unsigned int _EEDATA(128) EPConfigS[128];	//EEPROM数据存储
extern unsigned int LT;
extern unsigned int phasecompareError;
extern unsigned int compareACnt;
extern unsigned int step11;


//extern unsigned int ad100MsFlag;
extern unsigned int lowVoltageTest;
extern unsigned int zeroCrossCnt1;


extern unsigned int		OUTPUT_EXTERN_READY;
extern unsigned int		OUTPUT_EXTERN_ALARM;
extern unsigned int		OUTPUT_EXTERN_TRIP;
extern unsigned int     OUTPUT_NO_USE;
extern unsigned int     OUTPUT_EXTERN_TRIGGER;
extern unsigned int   Ia_im;
extern unsigned int   Ic_im;




//****************** 结构变量**********************

// 过零信号管理结构体
typedef struct {
    unsigned int actual_interval;      // 实际过零间隔
    unsigned int simulated_interval;   // 模拟过零间隔  
    unsigned int interval_history[4];  // 间隔历史记录
    unsigned char history_index;         // 历史索引
    unsigned char signal_quality;        // 信号质量标志
    unsigned char use_simulated;         // 使用模拟信号标志
    unsigned char switch_simulated;
    unsigned long last_actual_time;     // 上次实际过零时间

} tzcd_mgr;
//volatile ZCD_Manager_t zcd_mgr;


typedef struct {
    unsigned Start: 1;
	unsigned Stop: 	1;
	unsigned Ready: 1;
	unsigned RunCheck: 1;
	unsigned NoUse: 1;
    unsigned reset:1;
    unsigned Local_Remote:1;
    unsigned Startcontact:1;
   
	unsigned: 8;   	
}tInput;

typedef struct{
	unsigned long int buf;
	unsigned long int Iatemp;
	unsigned long int Ictemp;
    unsigned long int IaOri;
    unsigned long int IbOri;
    unsigned long int IcOri;
    unsigned int Ia;
    unsigned int Ib;
    unsigned int Ic;
}tADI;
extern tADI ADI;

typedef union{
	unsigned int Byte;
	struct {
    unsigned RunCheck: 1;
	unsigned Overload: 1;
	unsigned SCR: 1;
	unsigned OverIa: 1;
	unsigned OverTem: 1;
	unsigned OverIc: 1;
	unsigned UnbalanceI: 1;
	unsigned PhaseFault: 1;
	unsigned UnnormalStart: 1;
	unsigned StartTO: 1;
	unsigned PowerOFF: 1;

       unsigned  OverVoltage:1;
       unsigned LowerVoltage:1;
       unsigned ZeroI:1;   //零序保护

      unsigned StartOverFreProtection:1;
     // unsigned ThermalCapacityProtection:1;
	  unsigned phasecompareError:1;
			}Bits;
}tFault;	


typedef struct {
    unsigned Ms1:    1;
    unsigned Ms5:    1;
	unsigned Ms10:    1;
    unsigned Ms15:    1;
	unsigned Ms20:    1;
	unsigned Ms200:    1;
	unsigned AdMs100:  1;
	unsigned: 9;   	
}tSysTime;


typedef enum {WAIT,READY,START_DELAY,START,RUN,STOP,
		START_DELAY_FAULT,START_FAULT}eSysStatus;


typedef struct{
	unsigned RunCloseF:1;
    unsigned RunOPenF:1;
	unsigned :1;
	unsigned NoPowerF:1;
    unsigned Scc:1;
    unsigned Sco:1;
	
	unsigned :6;		
}tRelay;


typedef struct{
	unsigned A:1;
    unsigned B:1;
	unsigned C:1;	
	unsigned :13;		
}tFeedback;

typedef struct{
	
    unsigned phasecompareswitch:1;
    unsigned autotrigger:1;
    unsigned stoptrip:1;
    unsigned singlestart:1;
    unsigned TDetection:1;
    unsigned Ieswitch:1;         //额定电流切换
    unsigned Runcheckswitch:1;   //运行反馈检测
    unsigned Customswitch:1;     //自定义曲线切换
    unsigned FWD_REV:1;          //正反转切换
    unsigned Auto_turn:1;        //正反转自动检测
    unsigned Step:1;                 //突跳
    unsigned X6reset:1;
    unsigned Fre:1;        //频率切换
    unsigned Delay_Zero:1; //触发延时_零序电流切换
    unsigned Contactor:1;  //双接触器使能
    unsigned FBstart:1;//禁止启动

			
}tFunctionswitch;

typedef struct{
	
    unsigned SOV  :1;     //运行保护压板
    unsigned SLV :1;
    unsigned SOC  :1;
    unsigned S_UnbalanceI:1;
    unsigned S_SCR:1;
    unsigned S_PowerOFF:1; 
    unsigned S_ZeroI:1;        
    unsigned ROV:1;      //起动保护压板
    unsigned RLV:1;     
    unsigned ROC:1;          
    unsigned R_UnbalanceI:1; 
    unsigned R_ZeroI:1;      
    unsigned S_overload:1;
    unsigned R_overload:1;        
    unsigned sampletest:1; 
	unsigned runcheckswitch:1;

		
}tProtectswitch;

typedef struct{
	
    //运行保护压板
    unsigned runchecktrip:1;
    unsigned overtem:1;	
    unsigned RLI:1;	
}tProtectswitch1;


typedef struct{
	unsigned TurnRunF:1;
    unsigned AZeroflag:1;				
	unsigned PulseF:1;
	unsigned UpCarIaF:1;

	unsigned UpCarIbF:1;
	unsigned UpCarIcF:1;
	unsigned StartNormF:1;
    unsigned Test:1;

	unsigned stopflag:1;
    unsigned ZeroOk:1;
    unsigned phase:1;
	unsigned :5	
}tStartState;

typedef struct {
	unsigned int SaveParams[64];
	unsigned int FaultLastTime;
	unsigned int LastStartTime;
	unsigned int Ua;
	unsigned int Ub;
	unsigned int Uc;
	unsigned int Ia;
	unsigned int Ib;
	unsigned int Ic;
    unsigned int In;
	unsigned int Temperature;
	unsigned int Uo; 
	signed int IaIb;  
	signed int IbIc;
	signed int IcIa;	
    unsigned int frequency;//频率
    unsigned int residualheat;//剩余热容量
    unsigned int powerfactor;//功率因数
    unsigned int apparentpower;//视在功率
    unsigned int activepower;//有功功率
    unsigned int reactivepower;//无功功率
   


}tMainParams;


typedef struct{
    unsigned long BeginVoltage;
    unsigned int StartTime;	
	unsigned int StartTime1s;	
    unsigned int ControlMode;
	unsigned int LimitCurrent;
    unsigned int RunCount;
	unsigned int SwitchTime;
    unsigned int StopTime;
	unsigned long StopHead;
	unsigned long StopEnd;
	unsigned int StopStep;
	unsigned long StopDif;

    unsigned long Data;
    unsigned int OutData;
    unsigned int LoopData;
 	unsigned int MaxLoopData;
    unsigned long StartTimeCount;
    unsigned int AdjustData;
    long Datastore;
    unsigned int Stoptimestore;
    
    int StartCurrent;
	int StartCurrentOld;
	int StartUpCurrent;
    int StartUpCurrentLimit;
    int StartDownCurrent;
    int StartDownCurrentLimit;
    
    int OverTime;
  
 unsigned int VoltageUpperLimit;
 unsigned int VoltageLowerLimit;
 unsigned int StepVoltage;
 unsigned int StepDelay;
 unsigned int StartOverFre;
 unsigned int LCockedDelay;
 unsigned int ThermalStorageCoe;
 unsigned int ThermalDissipationCoe;
 unsigned int K1;
 unsigned int K2;
 unsigned int K3;
 unsigned int K4;
 unsigned int modbusaddr;
unsigned int modbusbaud;   
unsigned int D1;
unsigned int D2;
unsigned int cosa;

unsigned long Vb;
unsigned long V1;
unsigned long V2;
unsigned long V3;
unsigned long V4;
unsigned long V5;

unsigned int t1;
unsigned int t2;
unsigned int t3;
unsigned int t4;
unsigned int t5;

unsigned long Vb_data;

unsigned int LoopData1;
unsigned int LoopData2;
unsigned int LoopData3;
unsigned int LoopData4;
unsigned int LoopData5;
unsigned int LoopData6;
unsigned int LoopData7;
unsigned int LoopData8;
unsigned int LoopData9;

unsigned int ZeroILimit;
unsigned int ZeroIdelay;
unsigned int ROCV;
unsigned int ROCD;

unsigned int I_increase;
unsigned int OverIdelay;
unsigned int OverTdelay;
unsigned int UnbalanceIdelay;
unsigned int Overvoltagedelay;
unsigned int Lowervoltagedelay;
unsigned int Lowercurrentset;
unsigned int Lowercurrentdelay;

unsigned int umin;
long Ugmin;

unsigned int K5_relay;
unsigned int K6_relay;
unsigned int K7_relay;
unsigned int K8_relay;

unsigned int start_class;
unsigned int run_class;

}tStartParams;




typedef struct {
    unsigned int RatingCurrent;
	unsigned int FastCurrent;
	unsigned int FastCurrent1;
	
	unsigned int NoVoltage;

	unsigned int CurrentUpperLimit;
	unsigned int NoCurrent;
    

	unsigned int ImbalanceDegree;
	unsigned int Temperature;
     unsigned int intevalsec;
     unsigned int intevalmin;
     unsigned int faultflag;
     unsigned long temp1,temp2,temp3;
     unsigned long temp4,temp5,temp6;
     signed long class_overload,runclass_overload;
    unsigned int phase;

}tProtectParams;


typedef struct{	
	unsigned int Count;
    unsigned int this_sam1;
	signed int siAdTemp1;
	signed int siAdTemp2;
	signed int siAdTemp3;
   long siAdTemp4;
   long siAdTemp5;
   long siAdTemp6;
   long siAdTemp7;
   long siAdTemp8;
   signed int siAdTemp9;
    long siAdTemp10;
   long siAdTemp11;
   long siAdTemp12;
   long siAdTemp13;


	signed int CofI;
	signed int CofU;
	signed int CofTem;
    signed int CofI_zero;

    signed int Ia_Iv;
    signed int Ib_Iv;
    signed int Ic_Iv;
    signed int In_Iv;
    signed int Uab_Iv;
    signed int Ubc_Iv;
    signed int Uca_Iv;

    long Uab_value;
    long Ubc_value;
    long Uca_value;
    long Ia_value;
    long Ib_value;
    long Ic_value;
    long In_value;

    long Ps_value;
    long Pa_value;
    double cos_value;   
    double acos_double;
    signed int acos_int; 
    signed int acos_max;
    signed int acos_min;
    signed int pf;
    signed int pf_int;
    signed int pfmax[3];
    
    unsigned int aiflag;
    unsigned int Laststarttime;
    unsigned int Imax;
    unsigned int Umax;
    unsigned int Umin;
    unsigned int Ud;
    signed int ad_z;
/*
    signed int abc0[4];
    signed int abc1[4];
    signed int abc2[4];

    long alf[4];
    long blt[4];
 
    signed int wt[4];
    signed int Dlta_t[4];
   */ 
    unsigned int pulse;
    unsigned int delaylimit;


   unsigned int FILTER_N;
   unsigned int FILTER_N1;
    
    unsigned long TemperatureAccumulate;
    unsigned int TemperatureOrigin;
   
     
        
    int CarDescendTimesIa;
    int CarUpTimesIa;
    int AgoIa;
    int MaxIa;
    
    int CarDescendTimesIb;
    int CarUpTimesIb;
    int AgoIb;
    int MaxIb;
    
    int CarDescendTimesIc;
    int CarUpTimesIc;
    int AgoIc;
    int MaxIc;    
}tAdcParams;
/*
*********************************************************************************************************
*  DATA TYPES
*********************************************************************************************************
*/ 
typedef struct{
	unsigned TxSetSuccessF3:1; 
	unsigned TxReadSetF3 :1; 
	unsigned TxReadDataF3:1; 
	unsigned TxReadLastMsgF3:1;

	unsigned TxSetSuccessF4:1; 
	unsigned TxReadSetF4 :1; 
	unsigned TxReadDataF4:1; 
	unsigned TxReadLastMsgF4:1;
	unsigned :8;  

}tUart232;
typedef struct{
	
	unsigned RxByteF:1;
	unsigned RxFrameF:1;
	unsigned TxFrameF:1;

	unsigned TxSetSuccessF:1;	
	unsigned TxReadSetF:1;
	unsigned TxReadDataF:1;
	unsigned TxReadLastMsgF:1;

	unsigned commandreset:1;
	unsigned commandtest:1;

	unsigned TxSetSuccessF2:1; 
	unsigned TxReadSetF2 :1; 
	unsigned TxReadDataF2:1; 
	unsigned TxReadLastMsgF2:1;

    unsigned TxReadDataF3:1;

	unsigned :2;  	
}tUart2;

typedef struct{
	unsigned int RxTimes;
	unsigned int RxHead;  
	unsigned int RxOrderType; 
	unsigned int RxOrderID;
	unsigned int RxDataLengthRec;	
	unsigned int RxDataLength;	
	unsigned int RxDataCnt;
	unsigned int RxDataCnt1;
	unsigned int RxDataCnt2;
	unsigned int RxData[60];
	unsigned int RxEnd; 
	unsigned int RxDataAddrH;
	unsigned int RxDataAddrL;
	unsigned int RxData16Bits[60];
	unsigned int RxIntervalCnt;
	unsigned int RxIntervalCnt1;

	unsigned int TxTimes;
	unsigned int TxHead;  
	unsigned int TxOrderType; 
	unsigned int TxOrderID;
	unsigned int TxDataLengthRec;	
	unsigned int TxDataLength;	
	unsigned int TxDataCnt;
	unsigned int TxDataCnt1;
	unsigned int TxDataCnt2;
	unsigned int TxData[60];
	unsigned int TxEnd; 
	unsigned int TxDataAddrH;
	unsigned int TxDataAddrL;
    unsigned int Txcnt;
}tU2TxRx;
/*
typedef struct{
	unsigned :1;
	unsigned RxByteF:1;
	unsigned RxFrameF:1;
	unsigned TxFrameF:1;

	unsigned TxSetSuccessF:1;	
	unsigned TxReadSetF:1;
	unsigned TxReadDataF:1;
	unsigned TxReadLastMsgF:1;

	unsigned RxFrameF1;
	unsigned RxFrameF2;
	unsigned :1;  
	unsigned :1; 

	unsigned :4;  	
}tUart1;
typedef struct{
	unsigned int RxTimes;
	unsigned char RxAddr;  
	unsigned char RxFnctNbr; 
	unsigned char RxDataAddrHi;
	unsigned char RxDataAddrLo;
	unsigned char RxDataCntHi;
	unsigned char RxDataCntLo;
	unsigned char RxDataCrcLo;
	unsigned char RxDataCrcHi;
	

	unsigned int RxIntervalCnt;
	unsigned int RxIntervalCnt1;

	unsigned int TxDataCrc;
    unsigned int RxDataCrc;
    unsigned int RxCalCrc;
	unsigned int TxTimes;
	unsigned int TxDataCnt;
	unsigned char TxData[128];
    unsigned char RxData[128];
    unsigned int Txcnt;
	

}tU1TxRx;


*/



typedef struct {
    volatile unsigned char  RxBuf[128];
    volatile unsigned char RxCnt;
    volatile unsigned char  TxBuf[128];
    volatile unsigned char  TxLen;
    volatile unsigned char  TxPtr;
    volatile unsigned char  FrameReady;    // 帧接收完成标志
    volatile unsigned int   Timer35T;      // 3.5T 超时计数器
    volatile unsigned long  ErrorCount;    // 统计溢出次数，用于诊断
} tModbus_Ctrl;




/*
*********************************************************************************************************
* INCLUDE GLOBAL VARIABLES
*********************************************************************************************************
*/ 
extern tRelay Relay;
extern eSysStatus SysStatus;
extern tSysTime SysTime;
extern tFault Fault;
extern tFeedback Feedback;
extern tFunctionswitch Functionswitch;
extern tProtectswitch Protectswitch;
extern tProtectswitch1 Protectswitch1;
extern tMainParams MainParams;  
extern tStartState StartState;
extern tStartParams StartParams;
extern tProtectParams ProtectParams;  
extern tAdcParams AdcParams;
extern tUart2	Uart2;
extern tInput  Input;
extern unsigned int stopFlag;
//extern tUart1	Uart1;
//extern 	tU1TxRx U1TxRx;
extern tzcd_mgr zcd_mgr;

extern tModbus_Ctrl MB;
#endif  
