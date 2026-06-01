//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//系统外部调用函数声明，内部调用函数在对应文件内声明  Function.h

#ifndef Function_H
#define Function_H   
   
//读EEP，配置系统参数
extern  void SysConfig(void);
//初始化各端口1
extern  void SetupPorts(void);
//初始化定时器
extern  void InitTimer(void);
//初始化AD
extern void  InitAD(void);
//初始化捕获单元
extern void InitIC( void );
// UART1 串行通讯初始化
extern void InitUART1(void);
// UART2 串行通讯初始化
extern void InitUART2(void);
//输出比较初始化
extern void InitOC(void);
//读EEPROM并设置程序初始参数值，启动初始功能单元
extern void StateInit(void);
//读EEPROM
extern int ReadEE(int Page, int Offset, int* DataOut, int Size);
//擦除EEPROM
extern int EraseEE(int Page, int Offset, int Size);
//写EEPROM
extern int WriteEE(int* DataIn, int Page, int Offset, int Size);
 
extern void __attribute__((__interrupt__)) _PWMInterrupt(void);
extern void __attribute__((__interrupt__)) _T1Interrupt(void);
extern void __attribute__((__interrupt__)) _FLTAInterrupt(void);
extern void __attribute__((__interrupt__)) _CNInterrupt(void);
extern void __attribute__((__interrupt__)) _ADCInterrupt(void);
extern void __attribute__((__interrupt__)) _IC1Interrupt (void);
extern void __attribute__((__interrupt__)) _IC2Interrupt (void);
extern void __attribute__((__interrupt__)) _IC3Interrupt (void);
extern void __attribute__((__interrupt__)) _IC4Interrupt (void);
extern void __attribute__((__interrupt__)) _IC6Interrupt (void);
extern void __attribute__((__interrupt__)) _IC4Interrupt (void);
extern void __attribute__((__interrupt__)) _SI2CInterrupt (void);
extern void __attribute__((__interrupt__)) _MI2CInterrupt (void);
extern void __attribute__((__interrupt__)) _U2RXInterrupt (void);
extern void __attribute__((__interrupt__)) _U2TXInterrupt (void);
extern void __attribute__((__interrupt__)) _U1RXInterrupt (void);
extern void __attribute__((__interrupt__)) _U1TXInterrupt (void);


//读EEP，设置PWM调制频率
extern void readEEP(void);
//读IO口状态
extern void IORead(void);		
//IO口输出
extern void IOWrite(void);
//保护处理程序		
extern void Protect(void);
//通信处理程序
extern void Uart1Tx(void);
extern void Uart2Tx(void);
extern void UartCRC(unsigned int DataCRC);
extern void CommunicateOrderUart2(void);
extern void CommunicateOrderUart1(void);
//主控制程序
extern void MainCtrl(void);
extern void StartOutput(void);
extern void StopOutput(void);
extern void SoftStart(void);
extern void StartClose(void);
extern void StartOpen(void);
extern void BeiTuo(void);
extern void RunClose(void);
extern void RunOpen(void);
extern void StartErrorStop(void);
extern void RunErrorStop(void);

extern void AdcDescend(void);
extern void TurnRun(void);
extern void Delay10ms(void);
extern void Delay60ms(void);
#endif 

