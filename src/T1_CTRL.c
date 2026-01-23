//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//主定时器控制文件，T1_CTRL.c

//头文件
#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明



unsigned int sysTimeCnt1;
unsigned int sysTimeCnt2;
unsigned int sysTimeCnt3;
unsigned int sysTimeCnt4;


unsigned int msCnt1;
unsigned int step3=0;


tSysTime SysTime;
//**********T1中断服务程序*************//
void __attribute__((__interrupt__)) _T1Interrupt(void)
{
	if(ICflag==1)
	{
		ICcnt++;
		if(ICcnt>14)
		{
			ICcnt=0;
			ICflag=0;
           // OUTPUT_TRIGGER = 0;
            OUTPUT_EXTERN_TRIGGER=0;
			IFS1bits.IC4IF = 0;
			IC4CONbits.ICM = 3;
			IEC1bits.IC4IE = 1;
		}
	}

	SysTime.Ms1 = 1;	
	sysTimeCnt1++;
	sysTimeCnt2++;
	sysTimeCnt3++;
    sysTimeCnt4++;

	if(sysTimeCnt1 > 9)
	{
		SysTime.Ms10 = 1;
		sysTimeCnt1 = 0;
	}


	if(sysTimeCnt2 > 4)
	{
		SysTime.Ms5 = 1;
		sysTimeCnt2 = 0;
	}
	if(sysTimeCnt3 > 14)
	{
		SysTime.Ms15 = 1;
		sysTimeCnt3 = 0;
	}

	if(sysTimeCnt4 > 99)
	{
		SysTime.AdMs100 = 1;
		sysTimeCnt4 = 0;
	}

	#ifdef TEST_AUTOZERO

	#endif
	IFS0bits.T1IF=0;				//clear interrupt flag//
    return;
}  
void SoftStart(void)//1ms执行周期
{
	StartParams.StartTimeCount= StartParams.StartTimeCount+1;
	if(StartParams.StartTimeCount > 100)	
	{
		if(!StartState.TurnRunF )
		{StartParams.StartTime1s = __builtin_divud(StartParams.StartTimeCount,100);}

	}

	if(StartParams.ControlMode == 0)
		{				
			if(!StartState.Test)
			{
				if(StartParams.StartCurrent > ProtectParams.RatingCurrent)
				{
					if(StartParams.StartUpCurrent > StartParams.StartUpCurrentLimit)
					{			
						StartParams.Data = StartParams.Data - (StartParams.LoopData >> 1);
					}
					else
					{
						StartParams.Data = StartParams.Data - StartParams.LoopData;
					}				
				}
				else
				{		
					StartParams.Data = StartParams.Data - StartParams.LoopData;
				}
					
				if(StartParams.StartDownCurrent > StartParams.StartDownCurrentLimit)
				{
					StartParams.Data = StartParams.Data - StartParams.LoopData;
					StartParams.Data = StartParams.Data - StartParams.LoopData;
				}

				if(StartParams.Data < StartParams.Ugmin)
				{
					StartParams.Data = StartParams.Ugmin;
				}
				if(!StartState.StartNormF)//
				{
					StartParams.OverTime = StartParams.OverTime + 10;
					if(StartParams.OverTime > StartParams.LCockedDelay)
					{
						Fault.Bits.UnnormalStart = 1;			
					}
				}
	
				if(StartParams.StartTimeCount > StartParams.StartTime)
				{
					Fault.Bits.StartTO = 1;
				}		
				StartParams.OutData = StartParams.Data >> 10;
			}
			else
			{	
				msCnt1++;
				if(msCnt1 > 19)
				{
//					IEC1bits.IC4IE = 1;
					IC4CONbits.ICM = 3;
					IFS1bits.IC4IF = 1;              //清零中断标志
					zeroCrossCnt1 = 0;
					msCnt1 = 0;
				}
				StartParams.Data = StartParams.Data - StartParams.LoopData;
				if(StartParams.Data < StartParams.Ugmin)
				{
					StartParams.Data = StartParams.Ugmin;
					StartParams.RunCount += 0x0001;
					if( StartParams.RunCount > 200)
					{
						if(StartState.PulseF)
						{StartState.TurnRunF = 1;}
					}
					if( StartParams.RunCount > 600)
					{Fault.Bits.StartTO = 1;}		
				}
					
				StartParams.OutData = StartParams.Data >> 10;
			}	
		}
	else if(StartParams.ControlMode == 1)
	{
		if( MainParams.Ia < StartParams.LimitCurrent)//Ib小于限流定值
			{
				if(MainParams.Ia < ProtectParams.FastCurrent)//Ib小于0.8倍的限流定制时
				{
						StartParams.Data = StartParams.Data - (StartParams.LoopData*StartParams.K1);
	
						
				}
				else if(MainParams.Ia < ProtectParams.FastCurrent1)//Ib小于0.9倍的限流定制时
				{
						StartParams.Data = StartParams.Data - (StartParams.LoopData*StartParams.K2);
				}

				else
				{
					StartParams.Data = StartParams.Data - (StartParams.LoopData/StartParams.K3);//Ib大于0.9倍限流定值时，步长除2
				}
	
											
		}
		else{
              if (StartParams.K4<=0)
              { 
               StartParams.K4=4;
              }
                
              else{
               StartParams.Data = StartParams.Data - (StartParams.LoopData/StartParams.K4);
                }
            }//Ib大于等于限流定值时，步长缩小4倍除4
		
                           
        if(StartParams.Data < StartParams.Ugmin)
		{
			StartParams.Data = StartParams.Ugmin;
		}
		if(!StartState.StartNormF)
		{
			StartParams.OverTime=StartParams.OverTime+10;
			if(StartParams.OverTime > StartParams.LCockedDelay)
			{
				Fault.Bits.UnnormalStart = 1;			
			}
		}
	
		if(StartParams.StartTimeCount > StartParams.StartTime)
		{
			Fault.Bits.StartTO = 1;
		}
		if(StartParams.StartTime1s > StartParams.SwitchTime)
		{
			if(StartState.PulseF)
			{StartState.TurnRunF = 1;}
		}
		StartParams.OutData = StartParams.Data >> 10;

	}
	else if(StartParams.ControlMode == 2)
		{
			
			StartParams.Data = StartParams.Data - StartParams.LoopData;
			if(StartParams.Data < StartParams.Ugmin)
			{
				StartParams.Data = StartParams.Ugmin;
				StartParams.RunCount += 10;
				if( StartParams.RunCount > 2000)
				{
					if(StartState.PulseF)
					{StartState.TurnRunF = 1;}
				}
				if( StartParams.RunCount > 6000)
				{Fault.Bits.StartTO = 1;}		
			}
          	if(StartParams.StartTimeCount > StartParams.StartTime)
		    {
			   StartState.TurnRunF = 1;
		    }
				
			StartParams.OutData = StartParams.Data >> 10;
		}
   else if(StartParams.ControlMode == 3)
    { 
         if(step3==0)
         { step3=1;
           StartParams.StartTimeCount=0;
         }
         if(step3==1 )
         {StartParams.Data = StartParams.Data - StartParams.LoopData1;
           if(StartParams.StartTimeCount>=StartParams.t1)
            {step3=2;}
         }
         if(step3==2)
         {StartParams.Data = StartParams.Data - StartParams.LoopData2;
           if(StartParams.StartTimeCount>=(StartParams.t1+StartParams.t2))
            {step3=3;}
         }
         if(step3==3)
         {StartParams.Data = StartParams.Data - StartParams.LoopData3;
           if(StartParams.StartTimeCount>=(StartParams.t1+StartParams.t2+StartParams.t3))
            {step3=4;}
         }
         if(step3==4)
         {StartParams.Data = StartParams.Data - StartParams.LoopData4;
           if(StartParams.StartTimeCount>=(StartParams.t1+StartParams.t2+StartParams.t3+StartParams.t4))
            {step3=10;}
         }
        
         if(step3==10)
        {
         if(Functionswitch.Customswitch==1)
            { StartState.TurnRunF= 1;}
         }
          
       	if(StartParams.Data < StartParams.Ugmin)
			{
			StartParams.Data = StartParams.Ugmin;
			}

       StartParams.OutData = StartParams.Data >> 10;


    }



	else
	{Nop();}	
   if(StartParams.OutData<AdcParams.delaylimit)
    {StartParams.OutData=AdcParams.delaylimit;}	
   	//AdcParams.pulse=8600 - MainParams.SaveParams[0]-StartParams.OutData;
}
/***********************************************************************************
*************************************************************************************/
/***********************************************************************************
*************************************************************************************/
void SoftStop1Ms()
{
	StartParams.StartTimeCount += 1;
	if(StartParams.StartTimeCount > 100)	
	{
		StartParams.StartTime1s = __builtin_divud(StartParams.StartTimeCount,100);
	}	
	if(StartParams.StartTimeCount > 5)
	{
		OUTPUT_RUN_ON = 0;
		OUTPUT_RUN_OFF = 1;	
      if(Functionswitch.Contactor==1)
         { Relay.Scc=1;}
	}
   
	if(StartParams.StartTimeCount > 300)
	{
		OUTPUT_RUN_OFF = 0;
      if(Functionswitch.Contactor==1)
         {Relay.Scc=0;}
		if((Input.RunCheck)&&(Functionswitch.Runcheckswitch==1)&&Protectswitch.runcheckswitch)
		{Fault.Bits.RunCheck = 1;}
	}
	
	StartParams.Data += StartParams.StopStep;
	StartParams.OutData = StartParams.Data >> 10;
	StartState.PulseF = 1;
    StartState.stopflag=1;
    SysStatus = STOP;
    if(StartParams.StartTimeCount > StartParams.StopTime)
	{
		stopFlag = 0;
		step11=13;
	}
}

void SoftStop2()
{
	StartParams.StartTimeCount += 1;
	if(StartParams.StartTimeCount > 100)	
	{
		StartParams.StartTime1s = __builtin_divud(StartParams.StartTimeCount,100);
	}	

   	
	StartParams.Data += StartParams.StopStep;
   if(StartParams.Data>StartParams.BeginVoltage)
    {StartParams.Data=StartParams.BeginVoltage;}
	StartParams.OutData = StartParams.Data >> 10;
	StartState.PulseF = 1;
    StartState.stopflag=1;
    SysStatus = STOP;
    if(StartParams.StartTimeCount > StartParams.Stoptimestore)
	{
		stopFlag = 0;
		step11=13;
	}
}
