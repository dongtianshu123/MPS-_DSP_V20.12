#include "UserParams.h"	
#include "variable.h"

unsigned int inputStartCnt1;
unsigned int inputStartCnt2;
unsigned int inputStartCnt3;
unsigned int inputStartCnt4;
unsigned int inputStopCnt1;
unsigned int inputStopCnt2;
unsigned int inputStopCnt3;
unsigned int inputStopCnt4;
unsigned int inputReadyCnt1;
unsigned int inputReadyCnt2;
unsigned int inputRunCheckCnt1;
unsigned int inputRunCheckCnt2;
unsigned int inputNoUseCnt1;
unsigned int inputNoUseCnt2;

unsigned int input_in6Cnt1;// reset
unsigned int input_in6Cnt2;
unsigned int input_in7Cnt1;//local_remote
unsigned int input_in7Cnt2;
unsigned int input_in8Cnt1;//start_contact_feedback
unsigned int input_in8Cnt2;

unsigned int output_sccCnt1;
unsigned int output_scoCnt1;



unsigned int step1=0;
unsigned int step13=0;
unsigned int step14=0;

unsigned int inputStartCnt11;
unsigned int inputStopCnt11;
unsigned int inputReadyCnt11;
unsigned int inputRunCheckCnt11;
unsigned int inputNoUseCnt11;
unsigned int outputcnt;




unsigned int inputX1,inputX2,inputX3,inputX4,inputX5,inputX6,inputX7,inputX8,inputX9;
unsigned int outputY1,outputY2,outputY3,outputY4,outputY5,outputY6,outputY7,outputY8,outputY9;


unsigned int		OUTPUT_EXTERN_READY;
unsigned int		OUTPUT_EXTERN_ALARM;
unsigned int		OUTPUT_EXTERN_TRIP;
unsigned int        OUTPUT_NO_USE;
unsigned int        OUTPUT_EXTERN_TRIGGER;

tInput  Input; 
void IORead(void) //10ms
{   
 
   if(Functionswitch.singlestart==0)
   {
 	if(!INPUT_START)
	{
		inputStartCnt2 = 0;
		inputStartCnt1++;
		if(inputStartCnt1 > 10)
		{	
			inputStartCnt1 = 0;
			Input.Start = 1;
            Input.Stop = 0;
		}		
	}
    else
   {
     inputStartCnt1=0;
     
    }

    
     
/*******************************************/
	if(!INPUT_STOP)
	{
		inputStopCnt2 = 0;
		inputStopCnt1++;
		if(inputStopCnt1 > 10)
		{	
			inputStopCnt1 = 0;
			Input.Stop = 1;
            Input.Start = 0;
		}		
	}
    else
   {
    inputStopCnt1 = 0;
   }
	
 }

else
{


   if ((!INPUT_START)==1 && step11==1)
   {	
		inputStartCnt3++;
     	if(inputStartCnt3 > 5)
		{	
			inputStartCnt3 = 0;
			Input.Start = 1;
            Input.Stop = 0;
            
		}	
    }
   else
   {inputStartCnt3 = 0;}

   if ((!INPUT_START)==0 && step11>=3&&step11<=10)
   {	
		inputStopCnt3++;
     	if(inputStopCnt3 > 5)
		{	
			inputStopCnt3 = 0;
			Input.Stop = 1;
            Input.Start = 0;
            step1=0;
		}	
    }
   else
   {inputStopCnt3 = 0;}

    	if(Input.Start == 1)
	{
		inputStartCnt3 = 0;
		inputStartCnt4++;
		if(inputStartCnt4 > 5)
		{	
			inputStartCnt4 = 0;
			Input.Start = 0;
            
		}		
	 }
    	if(Input.Stop == 1)
	{
		inputStopCnt3 = 0;
		inputStopCnt4++;
		if(inputStopCnt4 > 5)
		{	
			inputStopCnt4 = 0;
			Input.Stop = 0;
		}		
	}

  
}


	if(Input.Start == 1)
	{
		inputStartCnt1 = 0;
		inputStartCnt2++;
		if(inputStartCnt2 > 5)
		{	
			inputStartCnt2 = 0;
			Input.Start = 0;
            
		}		
	 }
   if(Input.Stop == 1)
	{
		inputStopCnt1 = 0;
		inputStopCnt2++;
		if(inputStopCnt2 > 10)
		{	
			inputStopCnt2 = 0;
			Input.Stop = 0;
		}		
	}
/*******************************************/
	if(!INPUT_READY)
	{
		inputReadyCnt2 = 0;
		inputReadyCnt1++;
		if(inputReadyCnt1 > 5)
		{	
			inputReadyCnt1 = 6;
			Input.Ready = 1;
		}		
	}
	else
	{
		inputReadyCnt1 = 0;
		inputReadyCnt2++;
		if(inputReadyCnt2 > 5)
		{	
			inputReadyCnt2 = 6;
			Input.Ready = 0;
		}		
	}
/*******************************************/
	if(!INPUT_RUN_CHECK	)
	{
		inputRunCheckCnt2 = 0;
		inputRunCheckCnt1++;
		if(inputRunCheckCnt1 > 10)
		{	
			inputRunCheckCnt1 = 11;
			Input.RunCheck = 1;
		}		
	}
	else
	{
		inputRunCheckCnt1 = 0;
		inputRunCheckCnt2++;
		if(inputRunCheckCnt2 > 10)
		{	
			inputRunCheckCnt2 = 11;
			Input.RunCheck = 0;
		}		
	}
/*******************************************/
	if(!INPUT_NO_USE)
	{
		inputNoUseCnt2 = 0;
		inputNoUseCnt1++;
		if(inputNoUseCnt1 > 5)
		{	
			inputNoUseCnt1 = 6;
			Input.NoUse = 1;
		}		
	}
	else
	{
		inputNoUseCnt1 = 0;
		inputNoUseCnt2++;
		if(inputNoUseCnt2 > 5)
		{	
			inputNoUseCnt2 = 6;
			Input.NoUse = 0;
		}		
	}
/******************复位*************************/
	if((!INPUT_in6) && (Functionswitch.X6reset==1))
	{
		input_in6Cnt2 = 0;
		input_in6Cnt1++;
		if(input_in6Cnt1 > 5)
		{	
			input_in6Cnt1 = 6;
			Uart2.commandreset = 1;
		}		
	}
    else if ((!INPUT_in6==0) && (Functionswitch.X6reset==1))
	{
		input_in6Cnt1 = 0;
		input_in6Cnt2++;
		if(input_in6Cnt2 > 5)
		{	
			input_in6Cnt2 = 6;
			Uart2.commandreset = 0;
		}		
	}
     else
      {}



/****************本地远程***************************/
if (!INPUT_in7) 
	{
		input_in7Cnt2 = 0;
		input_in7Cnt1++;
		if(input_in7Cnt1 > 5)
		{	
		   input_in7Cnt1 = 6;
		   Input.Local_Remote = 1;
		}		
	}
    else
	{
		input_in7Cnt1 = 0;
		input_in7Cnt2++;
		if(input_in7Cnt2 > 5)
		{	
			input_in7Cnt2 = 6;
			Input.Local_Remote = 0;
		}		
	}


/*******************************************/

/****************起动接触器反馈***************************/
if (!INPUT_in8) 
	{
		input_in8Cnt2 = 0;
		input_in8Cnt1++;
		if(input_in8Cnt1 > 2)
		{	
		   input_in8Cnt1 = 3;
		   Input.Startcontact = 1;
		}		
	}
    else
	{
		input_in8Cnt1 = 0;
		input_in8Cnt2++;
		if(input_in8Cnt2 > 2)
		{	
			input_in8Cnt2 = 3;
			Input.Startcontact = 0;
		}		
	}


/*******************************************/
	if(Input.Stop)
	{Input.Start = 0;}
/***************OUTPUT K5 ready****************************/
    if(StartParams.K5_relay==0)
       {OUTPUT_K5=OUTPUT_EXTERN_READY;}

else if(StartParams.K5_relay==1)
       {OUTPUT_K5=OUTPUT_START;}

else if(StartParams.K5_relay==2)
       {OUTPUT_K5=Input.RunCheck;}

else if(StartParams.K5_relay==3)
       {OUTPUT_K5=OUTPUT_ALARM;}

else if(StartParams.K5_relay==4)
       {OUTPUT_K5=OUTPUT_EXTERN_TRIP;}

else if(StartParams.K5_relay==5)
       {OUTPUT_K5=Relay.Scc;}   //起动接触器合闸输出

else if(StartParams.K5_relay==6)
       {OUTPUT_K5=Relay.Sco;}   //起动接触器分闸输出
    else 
        {}

/***************OUTPUT K6 trip****************************/
    if(StartParams.K6_relay==0)
       {OUTPUT_K6=OUTPUT_ALARM;}

else if(StartParams.K6_relay==1)
       {OUTPUT_K6=OUTPUT_EXTERN_READY;}

else if(StartParams.K6_relay==2)
       {OUTPUT_K6=OUTPUT_START;}

else if(StartParams.K6_relay==3)
       {OUTPUT_K6=Input.RunCheck;}

else if(StartParams.K6_relay==4)
       {OUTPUT_K6=OUTPUT_EXTERN_TRIP;}

else if(StartParams.K6_relay==5)
       {OUTPUT_K6=Relay.Scc;}

else if(StartParams.K6_relay==6)
       {OUTPUT_K6=Relay.Sco;}
    else 
        {}

/***************OUTPUT K7 scc****************************/
      if(StartParams.K7_relay==0)
       {OUTPUT_K7=OUTPUT_EXTERN_TRIP;}

else if(StartParams.K7_relay==1)
       {OUTPUT_K7=OUTPUT_EXTERN_READY;}

else if(StartParams.K7_relay==2)
       {OUTPUT_K7=OUTPUT_START;}

else if(StartParams.K7_relay==3)
       {OUTPUT_K7=Input.RunCheck;}

else if(StartParams.K7_relay==4)
       {OUTPUT_K7=OUTPUT_ALARM;}

else if(StartParams.K7_relay==5)
       {OUTPUT_K7=Relay.Scc;}

else if(StartParams.K7_relay==6)
       {OUTPUT_K7=Relay.Sco;}
    else 
        {}

/***************OUTPUT K5 sco****************************/
    if(StartParams.K8_relay==0)
       {OUTPUT_K8=Input.RunCheck;}

else if(StartParams.K8_relay==1)
       {OUTPUT_K8=OUTPUT_EXTERN_READY;}

else if(StartParams.K8_relay==2)
       {OUTPUT_K8=OUTPUT_START;}

else if(StartParams.K8_relay==3)
       {OUTPUT_K8=OUTPUT_ALARM;}

else if(StartParams.K8_relay==4)
       {OUTPUT_K8=OUTPUT_EXTERN_TRIP;}

else if(StartParams.K8_relay==5)
       {OUTPUT_K8=Relay.Scc;}

else if(StartParams.K8_relay==6)
       {OUTPUT_K8=Relay.Sco;}
    else 
        {}




if (Relay.Scc==1)    //起动接触器合闸2s返回
	{
	
		output_sccCnt1++;
		if(output_sccCnt1 > 20)
		{	
		   output_sccCnt1 = 0;
		   Relay.Scc = 0;
		}		
	}

if (Relay.Sco==1) //起动接触器分闸2s返回
	{
		
		output_scoCnt1++;
		if(output_scoCnt1 > 20)
		{	
		   output_scoCnt1 = 0;
		   Relay.Sco = 0;
		}		
	}

}

