                                                      #include <p30F5011.h>
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明
#include "UserParams.h"

#define START_DELAY_CNT 100
#define STARTCOMPARE 50
unsigned int startDelayCnt;
unsigned int startcompare;
unsigned int stopDelayCnt;
unsigned int stopFlowCnt = 0;
unsigned int stopAppCnt = 0;
unsigned int runOpenCnt = 0;
unsigned int runCloseCnt = 0;
unsigned int orderClose;
unsigned int orderCloseCnt;
unsigned int orderOpen;
unsigned int turnRunCnt;
unsigned int runfeedbackcnt=0;
extern unsigned int feedBackACnt;
extern unsigned int feedBackBCnt;
extern unsigned int feedBackCCnt;
extern unsigned int PULSEWidth;
extern unsigned int PULSEWidth1;
extern unsigned int pwturnflag;

unsigned int step11=0; //起动步骤
unsigned int step12=0; //运行反馈检测步骤

unsigned int normalstopCnt;


unsigned int stopFlag;
extern unsigned int step3;
extern unsigned int StartOFTOK;
extern unsigned int  step4;
extern unsigned int  step5;


eSysStatus SysStatus;
tRelay Relay;

void StartDataIni(void)
{	
    PULSEWidth1=600;
    pwturnflag=0;
	StartState.PulseF = 0;
    StartState.stopflag=0;

	StartState.StartNormF = 0;
	StartState.TurnRunF = 0;
	StartState.UpCarIaF = 0;
	StartState.UpCarIbF = 0;
	StartState.UpCarIcF = 0;


	StartParams.RunCount = 0;
    runCloseCnt = 0;
  if(StartParams.ControlMode<=2)
{StartParams.Data = StartParams.BeginVoltage;}
else if (StartParams.ControlMode==3)
{StartParams.Data = StartParams.Vb_data;}
else
{}
	step3=0;
	StartParams.RunCount = 0;
	StartParams.StartTimeCount = 0;
	StartParams.StartTime1s = 0;
	StartParams.OverTime = 0;
	StartParams.StartCurrentOld = 0;

	Fault.Bits.UnnormalStart = 0;
	Fault.Bits.StartTO = 0;
	
	Relay.NoPowerF = 0;
	Relay.RunCloseF = 0;
	Relay.RunOPenF = 0;

	AdcParams.CarDescendTimesIa = 0;
	AdcParams.CarUpTimesIa = 0;
	AdcParams.MaxIa = 0;
	AdcParams.AgoIa = 0;

	AdcParams.CarDescendTimesIb = 0;
	AdcParams.CarUpTimesIb = 0;	
	AdcParams.MaxIb = 0;
	AdcParams.AgoIb = 0;

	AdcParams.CarDescendTimesIc = 0;
	AdcParams.CarUpTimesIc = 0;	
	AdcParams.MaxIc = 0;
	AdcParams.AgoIc = 0;
		
}
void StopFlow(void)
{
	StartState.PulseF = 0;
    StartState.stopflag=0;
    Uart2.commandtest=0;
//	Delay60ms();
	OUTPUT_START = 0;
	//OUTPUT_TRIGGER = 0;
 
			IFS1bits.IC4IF = 0;
			IC4CONbits.ICM = 3;
			IEC1bits.IC4IE = 1;
	OUTPUT_PA = 0;
	OUTPUT_PB = 0;
	OUTPUT_PC = 0;
     InitTmr2();   
     step4=0;
     step5=0;
}


 void RunClose(void)/*10ms*/
{	
	if(!Relay.RunCloseF)
	{
		Relay.RunOPenF = 0;
        if(INPUT_STOP)
        {
            OUTPUT_RUN_OFF = 0;
            OUTPUT_RUN_ON = 1;
        }
        else
        {
            OUTPUT_RUN_ON = 0;
        }
		runCloseCnt++;
		if(runCloseCnt > 500)
		{    
			OUTPUT_RUN_ON = 0;
			runCloseCnt = 0;
			Relay.RunCloseF = 1;
			StartState.TurnRunF = 0;	
			StopFlow();
			EEWrLastStartTime();
           
			StartParams.StartTime1s = 0;
            
			if((!Input.RunCheck)&&(Functionswitch.Runcheckswitch==1)&&Protectswitch.runcheckswitch)
			{Fault.Bits.RunCheck = 1;}
		}
	}

}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void NormalStop(void)
{
	StartParams.StartTimeCount=0;
              step3=0;
        Relay.Sco=1;
        Relay.Scc=0;
		OUTPUT_RUN_ON = 0;
		StartState.TurnRunF = 0;
		StopFlow();/*60ms*/
		OUTPUT_RUN_OFF = 1;
       if(Functionswitch.stoptrip==1)
       { OUTPUT_EXTERN_TRIP =1;	 }
                normalstopCnt++;
	   if (normalstopCnt>300) 
           {    normalstopCnt=0;
		     OUTPUT_RUN_OFF = 0;
             OUTPUT_EXTERN_TRIP =0;
		     Relay.RunOPenF= 1;
		if((Input.RunCheck)&&(Functionswitch.Runcheckswitch==1)&&Protectswitch.runcheckswitch)
		{Fault.Bits.RunCheck = 1;}
		//Input.RunCheck = 0;
		SysStatus = WAIT;
             step11=0;
           
	}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/



/***********************************起动逻辑*****************************************/
void RelayFlow(void)
{                  //等待
   	if(step11==0 && Input.Ready && (ProtectParams.faultflag==0) && (StartOFTOK==1)&&(Functionswitch.FBstart==0))
				{
					if((MainParams.Temperature < ProtectParams.Temperature)|| (Functionswitch.TDetection==0))
					{
						SysStatus = READY;
                         step11=1;
						OUTPUT_READY = 1;
						OUTPUT_EXTERN_READY = 1;
                        OUTPUT_ALARM=0;
                        OUTPUT_EXTERN_TRIP=0;
                        OUTPUT_EXTERN_ALARM	 = 0;
                        ProtectParams.phase=0; //相序检测清0

					}	
				 }

   else if(step11==1)   //备妥
            {
                if(!Input.Ready)
				{
					SysStatus = WAIT;
                    step11=0;
					OUTPUT_READY = 0;
					OUTPUT_EXTERN_READY = 0;	
				}
				if(MainParams.Temperature > ProtectParams.Temperature && Functionswitch.TDetection==1)
				{
					SysStatus = WAIT;
                    step11=0;
					OUTPUT_READY = 0;
					OUTPUT_EXTERN_READY = 0;
				}
                if(Functionswitch.FBstart==1)
                {	SysStatus = WAIT;
                    step11=0;
					OUTPUT_READY = 0;
					OUTPUT_EXTERN_READY = 0;
                 }

				
				if(Input.Start)
				{   
                    if(Functionswitch.Contactor==1)
                    {Relay.Scc=1;} //起动接触器闭合命令
					SysStatus = START_DELAY;
                    feedBackACnt = 0;
		            feedBackBCnt = 0;
		            feedBackCCnt = 0;
                    step11=2;
					startDelayCnt = 0;
					OUTPUT_START = 1;
					//OUTPUT_TRIGGER = 1;	
					OUTPUT_READY = 0;
					OUTPUT_EXTERN_READY = 0;
					StateInit();
					StartDataIni();	
                   if(ProtectParams.intevalmin>StartParams.StartOverFre)
                   {Fault.Bits.StartOverFreProtection=1;}
                 ProtectParams.intevalsec=0;
                 ProtectParams.class_overload=0;
                AdcParams.pfmax[2]=AdcParams.pfmax[1];
                AdcParams.pfmax[1]=AdcParams.pfmax[0]; 
                AdcParams.pfmax[0]=AdcParams.acos_max;
                AdcParams.acos_max=0;
	            AdcParams.Imax=0;
                AdcParams.Laststarttime=0;
				}
            }
  else  if (step11==2)   //起动延时
      {	         
        startDelayCnt++;
		if(startDelayCnt > START_DELAY_CNT )
		{
           if(phasecompareError==1&&(lowVoltageTest!=0xaaaa)&&Functionswitch.phasecompareswitch==1)
		      {
 
               Fault.Bits.phasecompareError=1;
		      }
           else
             {
               AdcParams.Umax=MainParams.Ua;
               Fault.Bits.phasecompareError=0;
               SysStatus = START;
			   startDelayCnt = 0;
               startcompare=0;
               step11=3;
               zeroCrossCnt1 = 1;
  
             }

  					
		}
      }

  else if (step11==15)
                    { if(Functionswitch.Step==1)            
                       {step11=8;}
                     else
                       {step11=3;}
      

                   } 
   else if (step11==3) //起动
      {
		        StartState.PulseF = 1;
                
                StartState.stopflag=0;               
                 ProtectParams.intevalmin=MainParams.SaveParams[18];//将起动过频时间写入起动间隔保护倒计时

                 ProtectParams.intevalsec=0;
                 StartOFTOK=0;

            



      }
   else if (step11==5) //正常停机
       {NormalStop();
   
       }

  else if (step11==7)  //故障
     {
   	   if (Uart2.commandreset==1)
         {ProtectParams.faultflag=0;
          Relay.Sco=1;
          Fault.Byte=0;
          Uart2.commandreset=0;
          OUTPUT_ALARM=0;
          OUTPUT_EXTERN_TRIP=0;
          OUTPUT_EXTERN_ALARM = 0;
          OUTPUT_RUN_OFF =0;
          step11=0;
         }


     }
 else if (step11==8)  //突跳
   {
		   StartState.PulseF = 1;
                StartState.stopflag=0;               
                 ProtectParams.intevalmin=MainParams.SaveParams[18];//将起动过频时间写入起动间隔保护倒计时
                 ProtectParams.intevalsec=0;
                 StartOFTOK=0;

        StartParams.Data =((8000 - MainParams.SaveParams[0])*(100-StartParams.StepVoltage)/100+StartParams.Ugmin);//装入突跳电压
	 StartParams.OutData = StartParams.Data >> 10;

           if(StartParams.StartTimeCount>(StartParams.StepDelay*10))//2s后导通角回到初始电压
             {step11=3;
              StartParams.Data = StartParams.BeginVoltage;
             }

   }
 
  else if (step11==10)  // 确认软停
    {
        OUTPUT_START = 1;
		//OUTPUT_TRIGGER = 1;
		stopDelayCnt++;
		if(stopDelayCnt > 200)	
		{
			StateInit();
			stopFlag = 55;
			StartParams.Data = StartParams.StopHead;
			StartParams.StartTimeCount = 0;
			SysStatus = STOP;
                   step11=11;
             }
   }
 else if (step11==11) //进入导通角减小逻辑，在主逻辑1ms扫描周期   SoftStop1Ms()
   {  step11=12;
   } 
else if (step11==17)
   {    
        OUTPUT_START = 1;
		//OUTPUT_TRIGGER = 1;
		stopDelayCnt++;
		if(stopDelayCnt > 20)	
		{
			StateInit();
			stopFlag = 55;
			StartParams.Data = StartParams.Datastore;
            if(StartParams.StartTimeCount>=StartParams.StopTime)
            {StartParams.Stoptimestore=StartParams.StartTimeCount;}
            else
            {StartParams.Stoptimestore=StartParams.StopTime;}
			StartParams.StartTimeCount = 0;
            SysStatus = STOP;
            step11=18;
         }

    }

else if (step11==13)
   {NormalStop();}

  else {}

/******************运行逻辑***********************************/
  	       if(Input.RunCheck==0 && step12 ==0)
			{step12=1;}
             if (Input.RunCheck==1 && step12 ==1)
                  {
                step12=2;
				SysStatus = RUN;
                step11=4;
 
				Relay.RunOPenF = 0;
               if(Functionswitch.Contactor==1)
                    {Relay.Sco=1;} 
			}
             if (Input.RunCheck==0 && step12 ==2)
                  {step12=0;
                    

                  }
            if ((Input.RunCheck==1)&&(Functionswitch.Runcheckswitch==1) && step11 <=3)
            {
				SysStatus = RUN;
                step11=4;
 
				Relay.RunOPenF = 0;
               if(Functionswitch.Contactor==1)
                    {Relay.Sco=1;} 
             }
            else
             {}

/*******************停机逻辑************************************/
           
 if (Input.Stop==1 && step11==4 && StartParams.StopTime!=0) //软停
    {           step3=0;
                StartParams.StartTimeCount=0;
                SysStatus = 60;	
		   stopDelayCnt = 0;
		   stopFlag = 0;
           step11=10;
           OUTPUT_READY = 0;
		   OUTPUT_EXTERN_READY = 0;
    }
else if (Input.Stop==1 && step11>=2 && step11<=3 && StartParams.StopTime!=0)
    {
              step11=17;  
          	OUTPUT_READY = 0;
			OUTPUT_EXTERN_READY = 0;
          StartParams.Datastore=StartParams.Data;
          
   }
else if  (Input.Stop==1 &&  StartParams.StopTime==0 && step11 >=2 && step11 <=4) //正常停机
    {
            step11=5;   
            OUTPUT_READY = 0;
			OUTPUT_EXTERN_READY = 0;
    }

  else
   {}







/******************故障逻辑***********************************/
  if (Fault.Byte !=0  ) //故障处理
     {
      		    StopFlow();             //???
				OUTPUT_READY = 0;
				OUTPUT_EXTERN_READY = 0;
				OUTPUT_ALARM = 1;
				OUTPUT_EXTERN_ALARM	 = 1;
                Relay.Sco=1;
                Relay.Scc=0;
           if(OUTPUT_RUN_ON==0)
			{OUTPUT_EXTERN_TRIP = 1;}
				EEWrFaultLastTime(); 
                
				StartParams.StartTime1s = 0; 
				EEWrLastStartTime();
           ProtectParams.faultflag=1;     //??
              SysStatus = WAIT;
               step11=7;

     }

/*************************************************************/

	if(StartState.TurnRunF)
	{
		if(!Input.Stop)
	  {RunClose();}		
	}
    if(step11==4)
    { 			
      if((!Input.RunCheck)&&(Functionswitch.Runcheckswitch==1)&&Protectswitch.runcheckswitch)
		{runfeedbackcnt++;
          if(runfeedbackcnt>500)
          {Fault.Bits.RunCheck = 1;
           runfeedbackcnt=0;
          } 
        }
      else
        {runfeedbackcnt=0;}
    

    }

    if((SysStatus == WAIT)||(SysStatus == READY))    //调试模式
    {
    if ((Input.NoUse==1)||(Uart2.commandtest==1))
     {lowVoltageTest = 0xaaaa;
     }
	else
     {lowVoltageTest = 0;
     }	
    }

	



	


    }			

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void EEWrFaultLastTime()
{
	if(MainParams.FaultLastTime != Fault.Byte)
	{
		SET_CPU_IPL(7);	
		EEPROMADDR = 0xFC00 + 2*30;
		EraseEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		WriteEE(&Fault.Byte,__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		MainParams.FaultLastTime = Fault.Byte;
		SET_CPU_IPL(3);	
	}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void EEWrLastStartTime()
{
	if(MainParams.LastStartTime != StartParams.StartTime1s)
	{
		SET_CPU_IPL(7);	
		EEPROMADDR = 0xFC00 + 2*31;
		EraseEE(__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		WriteEE(&StartParams.StartTime1s,__builtin_tblpage(&EPConfigS[0]),EEPROMADDR, WORD);
		MainParams.LastStartTime = StartParams.StartTime1s;
		SET_CPU_IPL(3);	
	}
}


	

