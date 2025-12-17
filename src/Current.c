#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明
#include "math.h"

#define COF_U 780
#define COF_I 77// KZX 3.3 
#define COF_IN 165
#define COF_TEMPRATURE 2000
#define	I_DECREASE		100
#define	I_INCREASE		100


unsigned int Delay2sCnt1;
unsigned int Delay2sCnt2;
unsigned int samcnt1;
unsigned int reducecnt1;
unsigned int reducecnt2;

unsigned long ulCofTemp1;
unsigned int TemporaryAdcDescend;
unsigned int pwturnflag;
unsigned int PULSEWidth1;//脉宽更新缓存
extern unsigned int PULSEWidth;
extern unsigned int pwturnflag;


void AdcDescend(void)
{   
   
    AdcParams.Uab_value=sqrt(AdcParams.siAdTemp4/AdcParams.FILTER_N1);
    AdcParams.Ubc_value=sqrt(AdcParams.siAdTemp12/AdcParams.FILTER_N1);
    AdcParams.Uca_value=sqrt(AdcParams.siAdTemp13/AdcParams.FILTER_N1);
    

    AdcParams.Ia_value=sqrt(AdcParams.siAdTemp5/AdcParams.FILTER_N1);
    AdcParams.Ic_value=sqrt(AdcParams.siAdTemp7/AdcParams.FILTER_N1);
    AdcParams.Ib_value=sqrt(AdcParams.siAdTemp6/AdcParams.FILTER_N1);
    AdcParams.In_value=sqrt(AdcParams.siAdTemp8/AdcParams.FILTER_N1);

    AdcParams.Ps_value=__builtin_mulss(AdcParams.Uab_value, AdcParams.Ia_value);//视在功率
    AdcParams.cos_value=(double)AdcParams.siAdTemp10/(AdcParams.FILTER_N1*AdcParams.Ps_value);
   //AdcParams.acos_int=(signed int)(acos(AdcParams.cos_value)*100.0); 
   //limittemp=__builtin_mulss(AdcParams.acos_int,3185);
   //AdcParams.delaylimit=__builtin_divsd(limittemp,100)+100;
   AdcParams.pf_int=(signed int)(AdcParams.cos_value*100.0);
    
  if((MainParams.Ia> ProtectParams.RatingCurrent
			|| MainParams.Ib> ProtectParams.RatingCurrent
			|| MainParams.Ic> ProtectParams.RatingCurrent)&&Protectswitch.sampletest==1)            //判断功率因数，如果在起动电流大于额定电流后，为负值持续1s判断起动故障
  {
   if(AdcParams.pf<0)
   {samcnt1++;
    if(samcnt1>100)
     {Fault.Bits.UnnormalStart = 1;}
   }
   else
   {samcnt1=0;}
  }


   
    if(AdcParams.pf_int>1 && step11==3&& StartState.TurnRunF==0)
       {
        if(StartParams.StartTime1s==1)
          {AdcParams.acos_min=AdcParams.pf_int;}
        if(AdcParams.pf_int<AdcParams.acos_min) 
          {AdcParams.acos_min=AdcParams.pf_int;}
        if(AdcParams.pf_int>AdcParams.acos_max) 
         {AdcParams.acos_max=AdcParams.pf_int;}
        if(MainParams.Ia>AdcParams.Imax)
         {AdcParams.Imax=MainParams.Ia;
          AdcParams.Umin=MainParams.Ua;
          AdcParams.Ud=AdcParams.Umax-AdcParams.Umin;
          AdcParams.Ud= __builtin_muluu(AdcParams.Ud,100);
          AdcParams.Ud= __builtin_divud(AdcParams.Ud,AdcParams.Umax);

         }
        if(MainParams.Ic>AdcParams.Imax)
         {AdcParams.Imax=MainParams.Ic;} 
          
        if(StartParams.StartTime1s!=AdcParams.Laststarttime)
         {AdcParams.Laststarttime=StartParams.StartTime1s;}
           

        AdcParams.pf=AdcParams.pf_int;  
       }
    
        
	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, AdcParams.Ia_value);  //A相电流
	MainParams.Ia = __builtin_divud(ulCofTemp1,COF_I);
    
	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, AdcParams.Ib_value);  //B相电流
	MainParams.Ib = __builtin_divud(ulCofTemp1,COF_I);

	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, AdcParams.Ic_value);  //C相电流
	MainParams.Ic = __builtin_divud(ulCofTemp1,COF_I);

	ulCofTemp1 = __builtin_muluu(AdcParams.CofI_zero, AdcParams.In_value);  //零序电流
	MainParams.In = __builtin_divud(ulCofTemp1,COF_I);

//	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, ADI.IcOri);
//	ADI.Ic = __builtin_divud(ulCofTemp1,COF_IN);
 //   MainParams.Ic=ADI.Ic;

//    MainParams.Ib = (MainParams.Ia+MainParams.Ic)/2;
 //   ADI.Ib=(ADI.Ia+ADI.Ic)/2;
 //  MainParams.Ib=ADI.Ib;

     ADI.Ia=MainParams.Ia;
     ADI.Ib=MainParams.Ib;
     ADI.Ic=MainParams.Ic;

	ulCofTemp1 = __builtin_muluu(AdcParams.CofU, AdcParams.Uab_value);
	MainParams.Ua = __builtin_divud(ulCofTemp1,COF_U);
    //ulCofTemp1 = __builtin_muluu(AdcParams.CofU, AdcParams.Ubc_value);
	MainParams.Ub = MainParams.Ua;
    //ulCofTemp1 = __builtin_muluu(AdcParams.CofU, AdcParams.Uca_value);
	MainParams.Uc = MainParams.Ua;

//	ulCofTemp1 = __builtin_muluu(AdcParams.CofU, AdcParams.UbOrigin);
//	MainParams.Ub = __builtin_divud(ulCofTemp1,COF_U);
//
//	ulCofTemp1 = __builtin_muluu(AdcParams.CofU, AdcParams.UcOrigin);
//	MainParams.Uc = __builtin_divud(ulCofTemp1,COF_U);

//	ulCofTemp1 = __builtin_muluu(AdcParams.CofU, AdcParams.UoutOrigin);
//	MainParams.Uo = __builtin_divud(ulCofTemp1,COF_U);
	MainParams.Uo = 0;

	ulCofTemp1 = __builtin_muluu(314, AdcParams.TemperatureOrigin);
	MainParams.Temperature = __builtin_divud(ulCofTemp1,9985);	
//	MainParams.Temperature = 25;

}
void CurrentCheck(void)
  {
	
	if((step11==3)&&(StartState.stopflag==0))  //电流切换在正常起动模式下和非软停模式下才判断
	{
		if(MainParams.Ia> ProtectParams.RatingCurrent
			|| MainParams.Ib> ProtectParams.RatingCurrent
			|| MainParams.Ic> ProtectParams.RatingCurrent)
		{StartState.StartNormF = 1;}
			
		StartParams.StartCurrent = MainParams.Ia;
		StartParams.StartUpCurrent = StartParams.StartCurrent - StartParams.StartCurrentOld;
		StartParams.StartDownCurrent = StartParams.StartCurrentOld - StartParams.StartCurrent;
		StartParams.StartCurrentOld = StartParams.StartCurrent;
			
		if( (MainParams.Ia < AdcParams.AgoIa) && (StartState.UpCarIaF == 1) )
		{
			AdcParams.CarDescendTimesIa++;
			if(AdcParams.CarDescendTimesIa > I_DECREASE)  
			{
				AdcParams.CarDescendTimesIa = 0;
			}
					
		}
		else if( (MainParams.Ia > AdcParams.AgoIa) && (StartState.UpCarIaF == 0) )
		{
			AdcParams.CarUpTimesIa++;
			if(AdcParams.CarUpTimesIa > (StartParams.I_increase)) 
			{
				if(MainParams.Ia > ProtectParams.RatingCurrent)
				{
					StartState.UpCarIaF = 1;
					AdcParams.CarDescendTimesIa = 0;
				}
			}
          	if(MainParams.Ia>ProtectParams.FastCurrent)
			{
					StartState.UpCarIaF = 1;
					AdcParams.CarDescendTimesIa  = 0;
			}
		}
		else
		{Nop();}	
		AdcParams.AgoIa = MainParams.Ia;
		

		if(MainParams.Ia > AdcParams.MaxIa)
		{
			AdcParams.MaxIa = MainParams.Ia;
		}

		else if(MainParams.Ia < AdcParams.MaxIa )
		{
           
			if(StartState.UpCarIaF && MainParams.Ua>50 )
			{
				TemporaryAdcDescend = AdcParams.MaxIa >> 1;

				if((MainParams.Ia < TemporaryAdcDescend)&&(Functionswitch.Ieswitch==0))
				{
                   reducecnt1++;
                   if(reducecnt1>2)
                     {
                       PULSEWidth1=2777;
                       StartParams.Data = StartParams.Ugmin;//导通角增大抑制振荡
                       Delay2sCnt1++;
				       if(Delay2sCnt1 > StartParams.t5&& Protectswitch.sampletest==0)
				         {StartState.TurnRunF= 1;}
                     } 

				}
				else if(MainParams.Ia < ProtectParams.RatingCurrent &&(Functionswitch.Ieswitch==1)&& MainParams.Ua>20)
				{
					Delay2sCnt1++;
				if(Delay2sCnt1 > 2&& Protectswitch.sampletest==0)
				{StartState.TurnRunF= 1;}
                if(AdcParams.pf>=StartParams.cosa && Protectswitch.sampletest==1)
                {StartState.TurnRunF= 1;}
				}

                else if(AdcParams.pf>=StartParams.cosa && Protectswitch.sampletest==1)
                {StartState.TurnRunF= 1;}
				else
				{
                 Delay2sCnt1=0;
                 reducecnt1=0;
                }
			}
		}
		else
		{Nop();}
/***************************************************************************************************/	
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/		
// 		if( (MainParams.Ib < AdcParams.AgoIb) && (StartState.UpCarIbF == 1) )
//		{
//			AdcParams.CarDescendTimesIb++;
//			if(AdcParams.CarDescendTimesIb > I_DECREASE)  
//			{
//				StartState.TurnRunF= 1;
//				AdcParams.CarDescendTimesIb = 0;
//			}
//					
//		}
//		else if( (MainParams.Ib > AdcParams.AgoIb) && (StartState.UpCarIbF == 0) )
//		{
//			AdcParams.CarUpTimesIb++;
//			if(AdcParams.CarUpTimesIb > I_INCREASE) 
//			{
//				if(MainParams.Ib > ProtectParams.RatingCurrent)
//				{
//					StartState.UpCarIbF = 1;
//					AdcParams.CarDescendTimesIb = 0;
//				}
//			}
//		}
//			
//		AdcParams.AgoIb = MainParams.Ib;
//
//		if(MainParams.Ib > AdcParams.MaxIb)
//		{
//			AdcParams.MaxIb = MainParams.Ib;
//		}
//		else if(MainParams.Ib < AdcParams.MaxIb)
//		{
//			if(StartState.UpCarIbF)
//			{
//				TemporaryAdcDescend = AdcParams.MaxIb >> 1;
//				if(MainParams.Ib < TemporaryAdcDescend)
//				{
//					StartState.TurnRunF= 1;
//				}
//				else if(MainParams.Ib < ProtectParams.RatingCurrent)
//				{
//					StartState.TurnRunF= 1;
//				}
//			}
//		}
			
/***************************************************************************************************/	
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/			
			
 		if( (MainParams.Ic < AdcParams.AgoIc) && (StartState.UpCarIcF == 1) )
		{
			AdcParams.CarDescendTimesIc++;
			if(AdcParams.CarDescendTimesIc > I_DECREASE)  
			{
				//StartState.TurnRunF= 1;
				AdcParams.CarDescendTimesIc = 0;
			}
					
		}
		else if( (MainParams.Ic > AdcParams.AgoIc) && (StartState.UpCarIcF == 0) )
		{
			AdcParams.CarUpTimesIc++;
			if(AdcParams.CarUpTimesIc > (StartParams.I_increase)) 
			{
				if(MainParams.Ic > ProtectParams.RatingCurrent)
				{
					StartState.UpCarIcF = 1;
					AdcParams.CarDescendTimesIc = 0;
				}
			}
          	if(MainParams.Ic >ProtectParams.FastCurrent)
			{
					StartState.UpCarIcF = 1;
					AdcParams.CarDescendTimesIc  = 0;
			}

		}
		else
		{Nop();}
			
		AdcParams.AgoIc = MainParams.Ic;

		if(MainParams.Ic > AdcParams.MaxIc)
		{
			AdcParams.MaxIc = MainParams.Ic;
		}
		else if(MainParams.Ic < AdcParams.MaxIc)
		{
			if(StartState.UpCarIcF && MainParams.Ua>50)
			{
				TemporaryAdcDescend = AdcParams.MaxIc >> 1;

				if((MainParams.Ic < TemporaryAdcDescend)&&(Functionswitch.Ieswitch==0))
				{
                   reducecnt2++;
                   if(reducecnt2>2)
                     {
                      PULSEWidth1=2777;
                      StartParams.Data = StartParams.Ugmin;
			          Delay2sCnt2++;
				      if(Delay2sCnt2 > StartParams.t5 && Protectswitch.sampletest==0)
				      {StartState.TurnRunF= 1;}
                     }
              	    	
				}
				else if(MainParams.Ic < ProtectParams.RatingCurrent &&(Functionswitch.Ieswitch==1)&& MainParams.Ua>20)
				{
                Delay2sCnt2++;
				if(Delay2sCnt2 > 2&& Protectswitch.sampletest==0)
				{StartState.TurnRunF= 1;}
                if(AdcParams.pf>=StartParams.cosa && Protectswitch.sampletest==1)
                {StartState.TurnRunF= 1;}
		    	}
                else if(AdcParams.pf>=StartParams.cosa && Protectswitch.sampletest==1)
                {StartState.TurnRunF= 1;}
                else
				{Delay2sCnt2=0;
                 reducecnt2=0;
                }
			
				
			}
		}	
		else
		{Nop();}
	}
}
