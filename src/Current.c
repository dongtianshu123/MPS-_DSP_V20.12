#include "UserParams.h"	//ï¿½Ã»ï¿½ÏµÍ³ï¿½ï¿½ï¿½Ã£ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½å¼°ï¿½ï¿½ï¿½ï¿½Öµ
#include "Function.h"	//ÏµÍ³ï¿½â²¿ï¿½ï¿½ï¿½Ãºï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú²ï¿½ï¿½ï¿½ï¿½Ãºï¿½ï¿½ï¿½ï¿½Ú¶ï¿½Ó¦ï¿½Ä¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#include "variable.h"	//ÏµÍ³È«ï¿½Ö±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö²ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú¶ï¿½Ó¦ï¿½Ä¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#include "math.h"

#define COF_U 780
#define COF_I 77// KZX 3.0/3.1 -161  KZX 3.3/3.4 -77
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
unsigned int PULSEWidth1;//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Â»ï¿½ï¿½ï¿½
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

    AdcParams.Ps_value=__builtin_mulss(AdcParams.Uab_value, AdcParams.Ia_value);//ï¿½ï¿½ï¿½Ú¹ï¿½ï¿½ï¿½
    AdcParams.cos_value=(double)AdcParams.siAdTemp10/(AdcParams.FILTER_N1*AdcParams.Ps_value);
   //AdcParams.acos_int=(signed int)(acos(AdcParams.cos_value)*100.0); 
   //limittemp=__builtin_mulss(AdcParams.acos_int,3185);
   //AdcParams.delaylimit=__builtin_divsd(limittemp,100)+100;
   AdcParams.pf_int=(signed int)(AdcParams.cos_value*100.0);
    
  if((MainParams.Ia> ProtectParams.RatingCurrent
			|| MainParams.Ib> ProtectParams.RatingCurrent
			|| MainParams.Ic> ProtectParams.RatingCurrent)&&Protectswitch.sampletest==1)            //ï¿½Ð¶Ï¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ð¶¯µï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ú¶î¶¨ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Îªï¿½ï¿½Öµï¿½ï¿½ï¿½ï¿?sï¿½Ð¶ï¿½ï¿½ð¶¯¹ï¿½ï¿½ï¿½
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
    
        
	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, AdcParams.Ia_value);  //Aï¿½ï¿½ï¿½ï¿½ï¿?
	MainParams.Ia = __builtin_divud(ulCofTemp1,COF_I);
    
	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, AdcParams.Ib_value);  //Bï¿½ï¿½ï¿½ï¿½ï¿?
	MainParams.Ib = __builtin_divud(ulCofTemp1,COF_I);

	ulCofTemp1 = __builtin_muluu(AdcParams.CofI, AdcParams.Ic_value);  //Cï¿½ï¿½ï¿½ï¿½ï¿?
	MainParams.Ic = __builtin_divud(ulCofTemp1,COF_I);

	ulCofTemp1 = __builtin_muluu(AdcParams.CofI_zero, AdcParams.In_value);  //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?
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
	
	if((step11==3)&&(StartState.stopflag==0))  //ï¿½ï¿½ï¿½ï¿½ï¿½Ð»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä£Ê½ï¿½ÂºÍ·ï¿½ï¿½ï¿½Í£Ä£Ê½ï¿½Â²ï¿½ï¿½Ð¶ï¿½
	{
		if(MainParams.Ia> ProtectParams.RatingCurrent
			|| MainParams.Ib> ProtectParams.RatingCurrent
			|| MainParams.Ic> ProtectParams.RatingCurrent)
		{StartState.StartNormF = 1;}
			
		StartParams.StartCurrent = MainParams.Ia;
		StartParams.StartUpCurrent = StartParams.StartCurrent - StartParams.StartCurrentOld;
		StartParams.StartDownCurrent = StartParams.StartCurrentOld - StartParams.StartCurrent;
		StartParams.StartCurrentOld = StartParams.StartCurrent;
			
        if((MainParams.Ia > (ProtectParams.RatingCurrent )) && (StartState.UpCarIaF == 0) )
		  {
			
			if(AdcParams.CarUpTimesIa < StartParams.I_increase) 
			{
               AdcParams.CarUpTimesIa++;
			}
            else
            {StartState.UpCarIaF = 1;}
		  }

        if(StartState.UpCarIaF)// ¸üÐÂÀúÊ·×î´óµçÁ÷
          {
		    if(MainParams.Ia > AdcParams.MaxIa)
		      {
		        AdcParams.MaxIa = MainParams.Ia;
		      }
                
		    else if (MainParams.Ia < AdcParams.MaxIa )
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
                                  StartParams.Data = StartParams.Ugmin;//ï¿½ï¿½Í¨ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
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
            }


		


			
/***************************************************************************************************/	
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/			
			
      if((MainParams.Ic > (ProtectParams.RatingCurrent )) && (StartState.UpCarIcF == 0) )
	     {
			
			if(AdcParams.CarUpTimesIc < StartParams.I_increase) 
			{
               AdcParams.CarUpTimesIc++;
			}
            else
            {StartState.UpCarIcF = 1;}


	     }
      if(StartState.UpCarIcF)// ¸üÐÂÀúÊ·×î´óµçÁ÷
         {
		        if(MainParams.Ic > AdcParams.MaxIc)
		          {
		             AdcParams.MaxIc = MainParams.Ic;
		          }
                
		        else if (MainParams.Ic < AdcParams.MaxIc )
		          {
           
			        if(StartState.UpCarIcF && MainParams.Uc>50 )
			           {
				        TemporaryAdcDescend = AdcParams.MaxIc >> 1;

				        if((MainParams.Ic < TemporaryAdcDescend)&&(Functionswitch.Ieswitch==0))
				            {
                             reducecnt2++;
                             if(reducecnt2>2)
                                {
                                  PULSEWidth1=2777;
                                  StartParams.Data = StartParams.Ugmin;//ï¿½ï¿½Í¨ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
                                  Delay2sCnt2++;
				                  if(Delay2sCnt2 > StartParams.t5&& Protectswitch.sampletest==0)
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
				               {
                                 Delay2sCnt2=0;
                                 reducecnt2=0;
                               }
			             }
		           }
            }
	

	}
	else
	{
          AdcParams.CarUpTimesIa=0;
          AdcParams.CarUpTimesIc=0;
          StartState.UpCarIaF=0;
          StartState.UpCarIcF=0;
          reducecnt1=0;
          reducecnt2=0;
          AdcParams.MaxIa=0;
          AdcParams.MaxIc=0;

    }
	
}
