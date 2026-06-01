#include "UserParams.h"	
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明
#include "Function.h"
#include <p30fxxxx.h>
#define DLY_CNT_1MS		(dFcy/1000)
unsigned int dlyCnt1;
unsigned int dlyCnt2;
unsigned int dlyCnt3;

void Delay10ms(void)
{
	dlyCnt1 = 0;
	for(; dlyCnt1<DLY_CNT_1MS; dlyCnt1++)
	{	
		Nop();
		Nop();
	}
}

void Delay3s(void)
{
	dlyCnt3 = 0;
	for(; dlyCnt3<300 ; dlyCnt3++)
	{	
		Delay10ms();
		IORead();
	}
}

void Delay60ms(void)
{
	dlyCnt2 = 0;
	for(; dlyCnt2<6; dlyCnt2++)
	{	
		Delay10ms();
            if(SysTime.Ms10)
		{
			SysTime.Ms10 = 0;
                      
			Uart2App();	
			//Uart1App();				
		}

	}
}