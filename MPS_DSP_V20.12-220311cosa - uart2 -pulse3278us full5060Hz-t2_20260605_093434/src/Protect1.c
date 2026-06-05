#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//
#include <math.h>
#include <stdio.h>

unsigned int LT=3333;
unsigned int buf;
unsigned int poweroffcnt;

extern unsigned int feedBackACnt;
extern unsigned int feedBackBCnt;
extern unsigned int feedBackCCnt;
extern unsigned int  step6;

extern signed int filter_bufUab[];
extern signed int filter_bufUbc[];
extern signed int filter_bufUca[];

unsigned int phaseErrCnt1;
unsigned int phaseErrCnt2;
unsigned int phaseErrCnt3;

unsigned int uaCnt1;

unsigned int ubCnt1;

unsigned int ucCnt1;


unsigned int uaCnt3;

unsigned int ubCnt3;

unsigned int ucCnt3;

unsigned int overIaCnt1;

unsigned int overIcCnt1;
unsigned int overTCnt1;

unsigned int LowerIaCnt1;
unsigned int LowerIcCnt1;

unsigned int RoverIaCnt1;
unsigned int RoverIaCnt2;


unsigned int RoverIcCnt1;
//unsigned int RoverIcCnt2;

unsigned int overInCnt1;
//unsigned int overInCnt2;

unsigned int unbalanceICnt1;

unsigned int noPowerCnt1;
//unsigned int noPowerCnt2;
unsigned int ATSTcnt1;  
unsigned int StartOFTOK;


unsigned int phaseACnt;
unsigned int newPhaseACnt;
unsigned int oldPhaseACnt;

unsigned int Acnt;

unsigned int newState;
unsigned int oldState;

unsigned int step2=0;

unsigned int compareACnt;
unsigned int newcompareACnt;
unsigned int oldcompareACnt;
unsigned int phasecompareOKcnt;
unsigned int phasecompareErrorcnt;
unsigned int phasecompareError;
  
   float wt1;
   double Dlta_t;
   double Dltat;
/*
void abc_dq(void)
{   
    int i;
    	for(i = 0; i < 4; i ++)
    {
    AdcParams.abc0[i]=filter_bufUbc[i]+filter_bufUab[i]+filter_bufUab[i];// 3*Ua
    AdcParams.abc1[i]=filter_bufUbc[i];//

    AdcParams.alf[i]=__builtin_mulss(500,AdcParams.abc0[i]);//U_alf=(3/2)*Ua
    AdcParams.blt[i]=__builtin_mulss(866,AdcParams.abc1[i]);//U_beta=0.866*Ubc

wt1=(float)AdcParams.blt[i]/AdcParams.alf[i];
wt1=atanf(wt1);
Dlta_t=wt1*100.0;
AdcParams.wt[i]=(signed int)Dlta_t;
}
for(i = 0; i < 4; i ++)
{
 AdcParams.Dlta_t[i]=AdcParams.wt[i+1]-AdcParams.wt[i];
if(AdcParams.Dlta_t[i]>157)
{AdcParams.Dlta_t[i]=AdcParams.Dlta_t[i]-314;}
else if(AdcParams.Dlta_t[i]<-157)
{AdcParams.Dlta_t[i]=AdcParams.Dlta_t[i]+314;}
else
{}

}
if(AdcParams.Dlta_t[0]>0 &&AdcParams.Dlta_t[0]>0 && AdcParams.Dlta_t[0]>0)
{
Functionswitch.FWD_REV=0;
}
else if(AdcParams.Dlta_t[0]<0 &&AdcParams.Dlta_t[0]<0 && AdcParams.Dlta_t[0]<0)
{
Functionswitch.FWD_REV=1;
}
else
{}

}
*/
void phasecompare()
{
if (step2==0&&FEEDBACK_A==0)
 {step2=1;}
if (step2==1&&FEEDBACK_A==1)
  {
    step2=2;
   oldcompareACnt = TMR3;
  }
if (step2==2&&INPUT_ZA==0)
  {
   step2=3; 
  }


if (step2==3&&INPUT_ZA==1)
   {
          newcompareACnt=TMR3;
        if(newcompareACnt > oldcompareACnt)
		  {compareACnt = newcompareACnt - oldcompareACnt;}
        else
		  {
			compareACnt = 65535 - oldcompareACnt;
			compareACnt = compareACnt + newcompareACnt;	
		  }
   step2=4;
  }
  


if(step2==4)  //如果和光纤反馈Fa上升沿和过零Za上升沿相差小于1.2ms大于6600us小于8000us则，比对成功（仅为6kV 10kV范围）
{ if(compareACnt>StartParams.D2&&compareACnt<StartParams.D1)
   {phasecompareOKcnt++;
     if(phasecompareOKcnt>3)
      {phasecompareError=0;
       phasecompareOKcnt=0;
      } 
    step2=0;
   }
  else
   {
    phasecompareOKcnt=0;
    phasecompareError=1;
    
    step2=0;
   }
}






}


void Frequencydetect1()
{
  if(StartState.AZeroflag)
	{
		newPhaseACnt = IC4BUF;

        if(newPhaseACnt > oldPhaseACnt)
		{phaseACnt = newPhaseACnt - oldPhaseACnt;}
        else
		{
			phaseACnt = 65535 - oldPhaseACnt;
			phaseACnt = phaseACnt + newPhaseACnt;	
		}
        oldPhaseACnt = newPhaseACnt;
		phaseErrCnt3 = 0;
        StartState.AZeroflag=0;
	}
	else
	{phaseErrCnt3++;}

	if(phaseACnt>16000 && phaseACnt<22000)
	{	
		phaseErrCnt1 = 0;
		phaseErrCnt2++;
	}
	
	else
	{
		phaseErrCnt1++;
		phaseErrCnt2 = 0;
	}

	if(phaseErrCnt2>3)
	{StartState.ZeroOk = 1;}
	
	if(phaseErrCnt1 > 3)
	{StartState.ZeroOk = 0;}

	if(phaseACnt != 0 && phaseACnt>16000 && phaseACnt<25000)
	{MainParams.frequency = __builtin_divud(10000000,phaseACnt);}
	buf=phaseACnt/6;
	if((buf>2700)&&(buf<3400)&&(Functionswitch.autotrigger==1))
	{LT=buf;}
	else
	{
     if(Functionswitch.Fre==0)
      {LT=3333;}
     else
      {LT=2777;}
    }

}
/*{
   if(INPUT_ZA)
     {T3CONbits.TON = 1;  }
else
     {
   Acnt=TMR3;
    TMR3=0;
T3CONbits.TON =0;
		}
if(Acnt != 0)
	{MainParams.frequency = __builtin_divud(5000000,Acnt);}
}*/



void SR_overload()   //双电子过载曲线 
{
  /*
   float T_SR;          //过载时间
   float x_current;     //电流倍数
   float class_overload; //过载曲线
   float k;
  
  x_current= (float)MainParams.Ia/ProtectParams.RatingCurrent; //电流倍数
  k=         (float)8*StartParams.start_class; //电子曲线对应的常数k
if (step11==3) //起动状态时
{
  class_overload =class_overload+0.1*(x_current*x_current-1);  //过载曲线累加

  if(class_overload > k)
   {
    Fault.Bits.Overload=1;
    }
}
else
{class_overload=0;}

 */


 

if (step11==3&& Protectswitch.S_overload==1) //起动状态时
{ 
   ProtectParams.temp1= __builtin_muluu(MainParams.Ia,MainParams.Ia);

  if(ProtectParams.class_overload>=0)
   {
  ProtectParams.class_overload =ProtectParams.class_overload+ProtectParams.temp1-ProtectParams.temp2;  //过载曲线累加
   }
  else
   {ProtectParams.class_overload =0;}

  if(ProtectParams.class_overload > (signed long)ProtectParams.temp3)
    {
    Fault.Bits.Overload=1;
    ProtectParams.class_overload=0;
    }
}

if (step11==4&& Protectswitch.R_overload==1)
{ ProtectParams.temp1= __builtin_muluu(MainParams.Ia,MainParams.Ia);
  if(ProtectParams.runclass_overload>=0 )
   {
  ProtectParams.runclass_overload =ProtectParams.runclass_overload+ProtectParams.temp1-ProtectParams.temp2;  //过载曲线累加
   }
   else
   {ProtectParams.runclass_overload=0; }
  if(ProtectParams.runclass_overload > (signed long)ProtectParams.temp4)
    {
    Fault.Bits.Overload=1;
    ProtectParams.runclass_overload=0;
    }


}

}



void StartOFTProtect()   //起动过频保护 10ms
{

  if(SysStatus ==WAIT && StartOFTOK==0)//当系统状态不为起动，且倒计时没有到0时，开始计时
  {    
       ATSTcnt1++;
      if(ATSTcnt1>10)
         {   ATSTcnt1=0;

            if(ProtectParams.intevalsec<=0&&ProtectParams.intevalmin<=0)
            {
                StartOFTOK=1;
                ProtectParams.intevalsec=0;
                ProtectParams.intevalmin=0;
            }
            else if(ProtectParams.intevalsec<=0&&ProtectParams.intevalmin>=1)
            {ProtectParams.intevalsec=59;
             ProtectParams.intevalmin--;           
            }
             
            else if(ProtectParams.intevalsec>0&&ProtectParams.intevalmin>=0)
            {
             ProtectParams.intevalsec--;
            }
            else
             {}
		}
				
         
         } 

}
    




void UProtect1()
{ 	
	if(MainParams.Ua>=StartParams.VoltageUpperLimit  )  //过压保护
	{
		uaCnt1++;

    }
	else 
	{
		uaCnt1 = 0;
	}

	if(MainParams.Ub>=StartParams.VoltageUpperLimit  )
	{
		ubCnt1++;
	
	}
	else 
	{
		
		ubCnt1 = 0;
	}

	if(MainParams.Uc>=StartParams.VoltageUpperLimit  )
	{
		ucCnt1++;
		
	}
	else 
	{
	
		ucCnt1 = 0;
	}

		if(uaCnt1>StartParams.Overvoltagedelay || ubCnt1>StartParams.Overvoltagedelay || ucCnt1>StartParams.Overvoltagedelay)
    	{
        Fault.Bits.OverVoltage=1;
       uaCnt1 = 0;
       ubCnt1 = 0;
	   ucCnt1 = 0;
        }

}

 void UProtect2()
{
	


	if( MainParams.Ua<=StartParams.VoltageLowerLimit )     //欠压保护
	{
		uaCnt3++;
    	

	}
	else 
	{

		uaCnt3 = 0;
	}


	if(MainParams.Ub<=StartParams.VoltageLowerLimit  )
	{
		ubCnt3++;
		
	}
	else 
	{
		
		ubCnt3 = 0;
	}

	if(MainParams.Uc<=StartParams.VoltageLowerLimit  )
	{
		ucCnt3++;
		
	}
	else 
	{
		
		ucCnt3= 0;
	}

      if(uaCnt3>StartParams.Lowervoltagedelay || ubCnt3>StartParams.Lowervoltagedelay || ucCnt3>StartParams.Lowervoltagedelay)
	   {
        Fault.Bits.LowerVoltage=1;
        uaCnt3 = 0;
        ubCnt3 = 0;
    	   ucCnt3= 0;
        }


//	zeroCrossCnt1++;
	if(zeroCrossCnt1 > 10)
	{Fault.Bits.PowerOFF = 1;}

}

void ZeroIprotect()
{
	if(MainParams.In >= StartParams.ZeroILimit)
	{
		overInCnt1++;
    if(overInCnt1>StartParams.ZeroIdelay)  //零序过流
	{Fault.Bits.ZeroI = 1;}
	}
	else
	{
		overInCnt1 = 0;
	}


}


void OverIProtect()                              //过流保护
{	
	if(MainParams.Ia >= ProtectParams.CurrentUpperLimit)
	{
		overIaCnt1++;
	   	if(overIaCnt1>StartParams.OverIdelay)  //A相过流
	    {Fault.Bits.OverIa = 1;}
	}
	else
	{		
		overIaCnt1 = 0;
	}
	
	if(MainParams.Ic >= ProtectParams.CurrentUpperLimit)
	{
		overIcCnt1++;
	if(overIcCnt1>StartParams.OverIdelay)  //C相过流
	{Fault.Bits.OverIc = 1;}
	}
	else
	{	
		overIcCnt1 = 0;
	}
	
}

void LowerIProtect()                              //欠电流保护
{	
	if(MainParams.Ia <= StartParams.Lowercurrentset)
	{
		LowerIaCnt1++;
	   	if(LowerIaCnt1>StartParams.Lowercurrentdelay)  //A相欠电流
	    {Fault.Bits.UnnormalStart = 1;}
	}
	else
	{		
		LowerIaCnt1 = 0;
	}
	
	if(MainParams.Ic <= StartParams.Lowercurrentset)
	{
		LowerIcCnt1++;
	   	if(LowerIcCnt1>StartParams.Lowercurrentdelay)  //C相欠电流
	    {Fault.Bits.UnnormalStart = 1;}
	}
	else
	{		
		LowerIcCnt1 = 0;
	}
	
	
}

void OverTProtect()                              //过温保护
{	
	if(MainParams.Temperature > ProtectParams.Temperature)
	{
		overTCnt1++;
	   	if(overTCnt1>StartParams.OverTdelay)  //过温相过流
	    {Fault.Bits.OverTem = 1;}
	}
	else
	{		
		overTCnt1 = 0;
	}
		
}






void UnbalanceIProtect()
{   float a;
    unsigned int aveI;
    unsigned long sum;
    unsigned int realUnblance;
	MainParams.IaIb = MainParams.Ia - MainParams.Ib;
	MainParams.IbIc = MainParams.Ib - MainParams.Ic;
	MainParams.IcIa = MainParams.Ic - MainParams.Ia;

	if(MainParams.IaIb < 0)
	{MainParams.IaIb = -MainParams.IaIb;}
	if(MainParams.IbIc < 0)
	{MainParams.IbIc = -MainParams.IbIc;}
	if(MainParams.IcIa < 0)
	{MainParams.IcIa = -MainParams.IcIa;}
    
	if(MainParams.IaIb < MainParams.IbIc)
	{MainParams.IaIb = MainParams.IbIc;}
	if(MainParams.IaIb < MainParams.IcIa)
	{MainParams.IaIb = MainParams.IcIa;}
    sum=MainParams.Ia+MainParams.Ib+MainParams.Ic;
    aveI=__builtin_divud(sum,3);

    a=(float)MainParams.IaIb/aveI;
 
    realUnblance=a*100;
  
      

	if((realUnblance >= ProtectParams.ImbalanceDegree) && (AdcParams.Ia_value>10))
	{
		unbalanceICnt1++;
    	if(unbalanceICnt1 > StartParams.UnbalanceIdelay)
	    {Fault.Bits.UnbalanceI = 1;}
		
	}
	else
	{
		
		unbalanceICnt1 = 0;
	}


   
}

void poweroffprotect()
{
  
	if(MainParams.Ua < 20)
	{
		poweroffcnt++;
       if (poweroffcnt>10)
           {
             Fault.Bits.PowerOFF = 1;
             poweroffcnt=0;
           }                
    }
    else
    {poweroffcnt=0;}

}
void SCRProtect()
{
	feedBackACnt++;
	feedBackBCnt++;
	feedBackCCnt++;



		if(feedBackACnt>50 || feedBackBCnt>50 || feedBackCCnt>50 )	
			{
				Fault.Bits.SCR = 1;	
       

			}
	
	if(SysStatus ==RUN)
	{
		feedBackACnt = 0;
		feedBackBCnt = 0;
		feedBackCCnt = 0;
	}

}
void NoPowerCheck(void)        //输入掉电
{
	if(MainParams.Ua<10)
	    {		
            noPowerCnt1++;
			if(noPowerCnt1 >10)
			{Relay.NoPowerF = 1;
             step6=0;
            }

		}
	
	
	else
		{
			noPowerCnt1 = 0;
			Relay.NoPowerF = 0;
		}

}
void Protect()
{
    

     if (step11==2 && Protectswitch.S_SCR==1 && lowVoltageTest!=0xaaaa )   //晶闸管保护-起动
      {SCRProtect();}	

     if(step11>=2&& step11<=3 && Protectswitch.S_PowerOFF==1 && lowVoltageTest!=0xaaaa)
     {poweroffprotect();}
    
    if(step11==2 ||step11==3)
     {
      if(Protectswitch.SLV)                               //欠压保护-起动
      {UProtect2();}
      }

     if(step11==3)
    {
      if(Protectswitch.SOV)                               //过压保护-起动
      {UProtect1();}


  
      if(Protectswitch.SOC)                             //过流保护-起动
      {OverIProtect();}

     if(Protectswitch.S_ZeroI)                             //零序保护-起动
      {ZeroIprotect();}

      if(Protectswitch1.overtem)                             //过温保护-起动
      {OverTProtect();}

    }  
  
    
     if(step11==4)
       { if(Protectswitch.ROV)                               //过压保护-运行
         {UProtect1();}

         if(Protectswitch.RLV)                               //欠压保护-运行
          {UProtect2();}
  
         if(Protectswitch.ROC)                             //过流保护-运行
         {OverIProtect();}
    

         if(Protectswitch.R_ZeroI)                             //零序保护-运行
           {ZeroIprotect();}

         if(Protectswitch1.RLI)                             //低电流保护
           {LowerIProtect();}

       }  
   } 
   





	
	
