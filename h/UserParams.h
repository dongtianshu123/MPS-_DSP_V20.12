//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//用户系统配置，常数定义及配置值 UserParms.h


#ifndef UserParams_H
#define UserParams_H

//**************  Oscillator （晶振） **************
#define dFoscExt        4000000					// 外部晶振频率(Hz)
#define dPLL            8						// PLL倍数
#define dTimer1         0.001					// 定时器1中断时间(s)

//*************   Derived   *****************************
#define dFosc       	(dFoscExt*dPLL)     					// 内部时钟频率					
#define dFcy        	(dFosc/4)           					// 指令频率						
#define dTcy        	(1.0/dFcy)          					// 指令周期时间					
#define dTImer1Tcy  	(int)(dTimer1*dFcy) 					// 定时器1计数值
#define dTImer2Tcy  	(int)(dFcy*8) 							// 定时器2频率值


//****************** EEPROM 存储单元常数 ***********************
#define 				WORD    		1						//EEPROM读写参数
#define 				ROW     		16
#define 				ALL_EEPROM      0xFFFF
#define 				ERROREE 		-1						//EEPROM读写参数变量


//************** 系统参数定标系数 **************
#define		dFcyTime3		(dFcy/8)
#define		ActiveTime		0.004
#define		ActivePWM		(ActiveTime * dFcyTime3)

#define		UgMinTime		(0.02 * 60 / 360) 
#define		UgMin			(UgMinTime * dFcyTime3 * 375)

#define		MAXSTEP_TIME	2000  //2秒钟内
#define		MINANGLE_TIME	2000  //已到最小角度后的延时

//************** 系统IO控制 **************
#define		INPUT_START			PORTDbits.RD7	
#define		INPUT_STOP			PORTFbits.RF0
#define		INPUT_READY			PORTFbits.RF1
#define		INPUT_RUN_CHECK		PORTGbits.RG1
#define		INPUT_NO_USE		PORTGbits.RG0
#define		INPUT_ZA			PORTDbits.RD11



#define       INPUT_in6                  PORTCbits.RC13
#define       INPUT_in7                 PORTCbits.RC14
#define       INPUT_in8                  PORTBbits.RB2
#define       INPUT_in9                  PORTBbits.RB3
#define       INPUT_in10                  PORTBbits.RB4

#define		OUTPUT_READY			LATGbits.LATG14	
#define		OUTPUT_START			LATGbits.LATG12
#define		OUTPUT_TRIGGER			LATGbits.LATG13
#define		OUTPUT_RUN_ON			LATGbits.LATG15
#define		OUTPUT_RUN_OFF			LATCbits.LATC1
#define		OUTPUT_ALARM			LATCbits.LATC2
#define		OUTPUT_K8			LATGbits.LATG6
#define		OUTPUT_K5		    LATGbits.LATG7
#define		OUTPUT_K6		    LATGbits.LATG8
#define		OUTPUT_K7		    LATGbits.LATG9

#define		OUTPUT_PA			LATDbits.LATD3
#define		OUTPUT_PB			LATDbits.LATD2
#define		OUTPUT_PC			LATDbits.LATD1


#define		FEEDBACK_A			PORTDbits.RD6
#define		FEEDBACK_B			PORTDbits.RD5
#define		FEEDBACK_C			PORTDbits.RD4

//************** 版本信息 **************
#define Version  2012

#endif


