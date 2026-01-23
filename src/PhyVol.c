//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//电压同步信号处理，PhyVol.c

//头文件
#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明

#define t4_delay 1000

unsigned int feedBackACnt;
unsigned int feedBackBCnt;
unsigned int feedBackCCnt;


//#define PULSEWidth 3278

tFeedback Feedback;

extern unsigned int PULSEWidth;
extern unsigned int PULSEWidth1;


unsigned int zeroCrossCnt1;

unsigned char ICflag=0;
unsigned char ICcnt=0;
unsigned char actual_zero_flag;
unsigned char actual_zero_flag1;

unsigned int  step4=0;
unsigned int  step5=0;
unsigned int  step6=0;
unsigned int  step7=0;  //速断检测
unsigned int  phaseFcnt;
unsigned int  phaseRcnt;
unsigned int  phaseEcnt;

unsigned int compareACnt1;
unsigned int newcompareACnt1;
unsigned int oldcompareACnt1;

extern unsigned int phaseACnt;

unsigned int last_capture=0;
unsigned int current_capture;//当前捕捉数值
unsigned int interval;



/**********************************计算平均间隔***********************************************/
unsigned int calculate_average_interval(void)
{
  unsigned long sum=0;
  unsigned char i;
  
  for (i=0; i<4; i++)
   {
    sum+=zcd_mgr.interval_history[i];
   }
    
  return(unsigned int)(sum>>2);
}
/**********************************信号质量检测***********************************************/
unsigned char check_signal_quality(unsigned int current_interval)
{ static unsigned char error_count=0;
  //频率范围检查(46.5Hz-51.5Hz,对应19420us-21500us)
    if(current_interval<19420 || current_interval>21500)
     {error_count++;
      if (error_count >2)
       {return 0;}//信号质量差
     } 
      else
     {  
         error_count=0;
         //时间间隔变化率检查 超过25%为信号质量差
         unsigned int avg_interval=calculate_average_interval();
          if(abs((int)current_interval-(int)avg_interval)>(avg_interval>>2))
            {return 0;}//信号质量差

     }

     
     return 1;//信号质量好

}
/**********************************信号质量检测***********************************************/
 void reset_timer4(void)
{
   T4CONbits.TON = 0;             //关闭定时器
   TMR4=0;                        //重置计数器
   PR4=zcd_mgr.simulated_interval+t4_delay;//更新周期
   T4CONbits.TON = 1; 


}

/**********************************统一的过零处理子程序***********************************************/
void zero_cross_processing(void)
{

      if(StartState.PulseF)
	 {
		TMR5 = 0; 
      if(Functionswitch.Fre==0)
       {PR5 = 1667;}
      else
       {PR5 = 1389;}//装载1667us
	  	IFS1bits.T5IF = 0;       
	    IEC1bits.T5IE = 1;     //T5使能
		T5CONbits.TON = 1; 	   //T5使能

        step4=1;
        ICflag=1;
        //OUTPUT_TRIGGER = 1;
        OUTPUT_EXTERN_TRIGGER=1;
		IFS1bits.IC4IF = 0;    //清零中断标志
		IC4CONbits.ICM = 0;
		IEC1bits.IC4IE = 0; 

	}
}

void InitIC( void )
{
    step6=0;
	IC4CON=0;                       
    IC4CONbits.ICTMR = 0;            /*1 = TMR2 contents are captured on capture event
									0 = TMR3 contents are captured on capture event*/
    IC4CONbits.ICI = 0;              
    IC4CONbits.ICM = 3;              /*010 = Capture mode, every falling edge
									001 = Capture mode, every edge (rising and falling) 
									011 = Capture mode, every rising edge*/
   
    IPC7bits.IC4IP = 7;              //设置中断优先级为7
    IFS1bits.IC4IF = 0;              //清零中断标志
    IEC1bits.IC4IE = 1;              // 1 允许IC4中断
}

void InitTmr2(void)
{
	T2CON = 0;                   
    TMR2 = 0;                    
    T2CONbits.TSIDL = 0;        
    T2CONbits.TGATE = 0;         /*1 = Gated time accumulation enabled
								0 = Gated time accumulation disabled*/
    T2CONbits.TCS = 0;           /*1 = External clock from pin TxCK
								0 = Internal clock (FOSC/4)*/
    T2CONbits.TCKPS = 1;         /*11 = 1:256 prescale value
								10 = 1:64 prescale value
								01 = 1:8 prescale value
								00 = 1:1 prescale value*/
    T2CONbits.T32 = 0;			/*1 = TMRx and TMRy form a 32-bit timer
								0 = TMRx and TMRy form separate 16-bit timer*/
	T2CONbits.TON = 0;  
    PR2 = 0xFFFF;
    IPC1bits.T2IP = 6;
    IFS0bits.T2IF = 0;           
    IEC0bits.T2IE = 0;    

	T3CON = 0;
    TMR3 = 0;
    T3CONbits.TSIDL = 0;        
    T3CONbits.TGATE = 0;         /*1 = Gated time accumulation enabled
								0 = Gated time accumulation disabled*/
    T3CONbits.TCS = 0;           /*1 = External clock from pin TxCK
								0 = Internal clock (FOSC/4)*/
    T3CONbits.TCKPS = 1;         /*11 = 1:256 prescale value
								10 = 1:64 prescale value
								01 = 1:8 prescale value
								00 = 1:1 prescale value*/

    T3CONbits.TON = 1;           
    PR3 = 0xFFFF;                  
    IPC1bits.T3IP = 6;
    IFS0bits.T3IF = 0;           
    IEC0bits.T3IE = 0;  


	T4CON= 0;                    // 关闭定时器3
    TMR4 = 0;                    // 复位定时器计3数器
    PR4=20000;
    T4CONbits.TSIDL = 1;         // 空闲模式下停止工作
    T4CONbits.TGATE = 0;         // 禁止门控定时器累计
    T4CONbits.TCS = 0;           // 使用Tcy 作为源时钟
    T4CONbits.TCKPS = 1;         // Tcy / 8 作为输入时钟
    T4CONbits.TON = 0;           // 开启timer3
	T4CONbits.T32 = 0;
   

	IPC5bits.T4IP = 6;
    IFS1bits.T4IF = 0;               
    IEC1bits.T4IE = 1;           // 使能timer4 中断     

	T5CON= 0;                    // 关闭定时器3
    TMR5 = 0;                    // 复位定时器计3数器
    T5CONbits.TSIDL = 1;         // 空闲模式下停止工作
    T5CONbits.TGATE = 0;         // 禁止门控定时器累计
    T5CONbits.TCS = 0;           // 使用Tcy 作为源时钟
    T5CONbits.TCKPS = 1;         // Tcy / 8 作为输入时钟
    T5CONbits.TON = 0;           // 开启timer3
    PR5 = PULSEWidth; 

	IPC5bits.T5IP = 6;
    IFS1bits.T5IF = 0;               
    IEC1bits.T5IE = 0;           // 禁止timer3 中断                           
}
	
  
void __attribute__((__interrupt__)) _IC4Interrupt (void)  //过零捕捉中断
{    
    StartState.AZeroflag=1;
	IFS1bits.IC4IF = 0;              //清零中断标志
	IC4CONbits.ICM = 3;
	zeroCrossCnt1 = 0;

   /*********************************************************/
    current_capture=IC4BUF;
    //计算时间间隔
    if(current_capture>=last_capture)
      {interval=current_capture -last_capture;}
    else
      {interval=(0xFFFF-last_capture)+current_capture;}

    //保存间隔到历史记录
    if (interval>19420&& interval<21500)
    {
      zcd_mgr.interval_history[zcd_mgr.history_index]=interval;
      zcd_mgr.history_index=(zcd_mgr.history_index+1)%4;
    }

    //计算平均间隔
     zcd_mgr.actual_interval=calculate_average_interval();

    //过零信号质量检测
     zcd_mgr.signal_quality=check_signal_quality(interval);

    if(zcd_mgr.signal_quality)
      { //
        zcd_mgr.simulated_interval=zcd_mgr.actual_interval;
        reset_timer4(); //重置T4
        actual_zero_flag=1;
      }
    last_capture=current_capture; //更新上一次时间记录值
     
    if(zcd_mgr.signal_quality)
     {
       zero_cross_processing (); 
     }

   /*********************************************************/


    

      
	return;
}

void __attribute__((__interrupt__)) _T5Interrupt(void)
{

   
   
	if((step4==1)&&(!INPUT_ZA)&&zcd_mgr.signal_quality)      //防抖判断  
	{
	
			                               //如果是抖动
        step4=0;
		T5CONbits.TON = 0; 	

        IFS1bits.T5IF = 0;               
        IEC1bits.T5IE = 0;

        IFS1bits.IC4IF = 0;              //重新打开IC检测
		IEC1bits.IC4IE = 1; 
		IC4CONbits.ICM = 3;
     //   T4CONbits.TON = 0; 	             //关掉虚拟过零延时
     //   IFS1bits.T4IF = 0;                
     //   IEC1bits.T4IE = 0;
  
	}
	else if ((step4==1)&&(INPUT_ZA)&&zcd_mgr.signal_quality) 
	{   step4=2;
        IFS1bits.IC4IF = 0;              //如果不是抖动，禁用IC中断
		IEC1bits.IC4IE = 0; 
		IC4CONbits.ICM = 0;

     }   
   else if((step4==1)&&(!INPUT_ZA)&&(!zcd_mgr.signal_quality))
    {   step4=2;
    }
   
  else
  {}
  
  if (step4==2)
   {
        TMR5 = 0;                               //如果不是抖动
		PR5 = StartParams.OutData;       	     //装载触发角延时
        if(actual_zero_flag1==1)
          {
           actual_zero_flag1=0;
           PR5=PR5-t4_delay;
          }
        PULSEWidth=PULSEWidth1;
        IFS1bits.T5IF = 0;               
        IEC1bits.T5IE = 1;
        T5CONbits.TON = 1; 
     if(Functionswitch.FWD_REV==0)               //正反转标志，如果是正转从第3步开始，如果是反转从第11步开始
	    {step4=3;}
     else 
        {step4=11;}
   
	}

 else if(step4 == 3)
	{   step4=4;

		TMR5 = 0;
        PR5=PULSEWidth;	
        IFS1bits.T5IF = 0;               
        IEC1bits.T5IE = 1;
        T5CONbits.TON = 1; 

		OUTPUT_PA = 1;
		OUTPUT_PB = 1;
     }  
 else if(step4==4)
    {
        step4=5;
        step5=3;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	      //LT为脉冲之间延时，默认为3333us，每次触发输出第一个延时为脉宽，第二个延时为LT-脉宽
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;

         IFS1bits.T5IF = 0;               
        IEC1bits.T5IE = 0;
 


    }
else if(step4 == 11)
	{   step4=12;

		TMR5 = 0;
        PR5=PULSEWidth;	
        IFS1bits.T5IF = 0;               
        IEC1bits.T5IE = 1;
        T5CONbits.TON = 1; 

		OUTPUT_PA = 1;
		OUTPUT_PB = 1;
     }  
 else if(step4==12)
    {
        step4=13;
        step5=21;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;

         IFS1bits.T5IF = 0;               
        IEC1bits.T5IE = 0;
 


    }

else {
	        


     }	



    



    return;
}

void __attribute__((__interrupt__)) _T2Interrupt(void)
{
	IFS0bits.T2IF=0;				//clear interrupt flag//
  

 if(step5 == 3)               //正常输出-正转
	{   step5=4;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 1;
		OUTPUT_PC = 1;
     }    
 else if(step5==4)
    {
        step5=5;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }
else if(step5 == 5)
	{   step5=6;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PB = 1;
		OUTPUT_PC = 1;
     }    
 else if(step5==6)
    {
        step5=7;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }

else if(step5 == 7)
	{   step5=8;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PB = 1;
		OUTPUT_PA = 1;
     }    
 else if(step5==8)
    {
        step5=9;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }      	
else if(step5 == 9)
	{   step5=10;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PC = 1;
		OUTPUT_PA = 1;
     }    
 else if(step5==10)
    {
        step5=11;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }

else if(step5 == 11)
	{   step5=12;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PC = 1;
		OUTPUT_PB = 1;
     }    
 else if(step5==12)
    {

         step5=13;
		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 0;           
		T2CONbits.TON = 0; 

    }


if(step5 == 21)               //反转
	{   step5=22;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PB = 1;
		OUTPUT_PC = 1;
     }    
 else if(step5==22)
    {
        step5=23;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }
else if(step5 == 23)
	{   step5=24;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 1;
		OUTPUT_PC = 1;
     }    
 else if(step5==24)
    {
        step5=25;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }

else if(step5 == 25)
	{   step5=26;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PB = 1;
		OUTPUT_PA = 1;
     }    
 else if(step5==26)
    {
        step5=27;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }      	
else if(step5 == 27)
	{   step5=28;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PC = 1;
		OUTPUT_PB = 1;
     }    
 else if(step5==28)
    {
        step5=29;
    	TMR2 = 0;
        PR2=LT-PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;


    }

else if(step5 == 29)
	{   step5=30;

		TMR2 = 0;
        PR2=PULSEWidth;	
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 1;           
		T2CONbits.TON = 1; 

		OUTPUT_PC = 1;
		OUTPUT_PA = 1;
     }    
 else if(step5==30)
    {

         step5=31;
		OUTPUT_PA = 0;
		OUTPUT_PB = 0;
        OUTPUT_PC = 0;
	    IFS0bits.T2IF = 0;               
	    IEC0bits.T2IE = 0;           
		T2CONbits.TON = 0; 

    }	
	


	else
	{

	}






    return;
}   

void __attribute__((__interrupt__)) _T4Interrupt(void)
{
	           
   /*
		TMR5 = 0; 
		PR5 = 1667;            
	  	IFS1bits.T5IF = 0;       
	    IEC1bits.T5IE = 1;
		T5CONbits.TON = 1; 	
	    ICdelay=1;  //过零检测延时标志置位1.667ms

		TMR4 = 0; 
		//PR4 = phaseACnt;    //虚拟过零    
        PR4 = 20000;    //虚拟过零    
	  	IFS1bits.T4IF = 0;       
	    IEC1bits.T4IE = 1;
		T4CONbits.TON = 1; 


		IFS1bits.IC4IF = 0;              //清零中断标志
		IC4CONbits.ICM = 0;
		IEC1bits.IC4IE = 0;
        step4=2; 
        */

     PR4=zcd_mgr.simulated_interval;
     zcd_mgr.signal_quality=0;
     if(!zcd_mgr.signal_quality)
       {
         zero_cross_processing();
       }
     if(actual_zero_flag)
       {
        actual_zero_flag=0;
        actual_zero_flag1=1;
        
        PR4=zcd_mgr.simulated_interval-t4_delay;
       }
      
      TMR4 = 0; 
	  IFS1bits.T4IF = 0;       
	  IEC1bits.T4IE = 1;
      T4CONbits.TON = 1;   


      

    return;
} 
void __attribute__((__interrupt__)) _CNInterrupt (void)
{
		IFS0bits.CNIF = 0;
	if(FEEDBACK_A != Feedback.A)
	{
		Feedback.A = FEEDBACK_A;
		feedBackACnt = 0;
	}

	if(FEEDBACK_B != Feedback.B)
	{
		Feedback.B = FEEDBACK_B;
		feedBackBCnt = 0;
	}

	if(FEEDBACK_C != Feedback.C)
	{
		Feedback.C = FEEDBACK_C;
		feedBackCCnt = 0;
	}
 if(SysStatus != START && SysStatus != RUN && MainParams.Ua>=28 )
 {  
   if(Feedback.A==0&&step6==0)
   {step6=1;}
   else if(Feedback.A==1&&step6==1)
   {step6=2;
    oldcompareACnt1 = TMR3;
   }
   else if (Feedback.B==1&&step6==2)
   {   step6=0;
        newcompareACnt1=TMR3;
        if(newcompareACnt1 > oldcompareACnt1)
		  {compareACnt1 = newcompareACnt1 - oldcompareACnt1;}
        else
		  {
			compareACnt1 = 65535 - oldcompareACnt1;
			compareACnt1 = compareACnt1 + newcompareACnt1;	
		  }
    
       if(compareACnt1<9000)
      { phaseFcnt++;
        phaseRcnt=0;
        phaseEcnt=0;
        if(phaseFcnt>6)
       {    
        StartState.phase =0;
        step6=5;
        phaseFcnt=0;
        if(Functionswitch.Auto_turn==1)
          {Functionswitch.FWD_REV=0;}
        
       }
      }
        else
         {
        phaseRcnt++;
        phaseFcnt=0;
        phaseEcnt=0;
         if(phaseRcnt>6)
           {
            StartState.phase=1;       
            step6=5;
            phaseRcnt=0;
        if(Functionswitch.Auto_turn==1)
          {Functionswitch.FWD_REV=1;}
           }



        } 
   
       
    
       }
   
   
   else
    {}
   

  }
}
    	

	






