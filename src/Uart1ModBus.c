/*
*********************************************************************************************************
* MPS SOURCE

*********************************************************************************************************
*/

/* 寄存器地址定义 - 映射到软起动器内部变量 */
#define REG_SYS_STATUS    0x0000  // 系统状态 (只读)
#define REG_FAULT_CODE    0x0001  // 故障代码 (只读)
#define REG_CURRENT_A     0x0002  // A相电流 (只读)
#define REG_VOLTAGE_A     0x0003  // A相电压 (只读)
#define REG_START_CMD     0x0064  // 0x64: 远程起停控制 (读写)
#define REG_STOP_CMD      0x0065  // 0x65: 远程停止控制

#define MB_REG_MAX        128     // 寄存器池大小
unsigned int Modbus_Regs[MB_REG_MAX]; 
/*
*********************************************************************************************************
* Uart1 OPERATIONS  
* Filename : PWMTest.c
* Version : V1.0
* Programmer(s) : JOE
* FGK
*********************************************************************************************************
* Note(s) : (1)OC4 is used , it is used to be pulse phase A for MPS;20110216
* (2) 
*********************************************************************************************************
/*
*********************************************************************************************************
* INCLUDE FILES
*********************************************************************************************************
*/
#include <p30fxxxx.h>
#include "UniversalDef.h"
#include "UserParams.h"	
#include "Function.h"	
#include "Variable.h"
/*
*********************************************************************************************************
* LOCAL DEFINES
*********************************************************************************************************
*/
#define BAUD_RATE    	((dFcy/16/StartParams.modbusbaud)-1)	/*Low speed = Fcy/16;else Fcy/4/*/
#define RX_INTERVER_TIME    	10			/*unit (10ms)*/
/*
*********************************************************************************************************
* LOCAL VARIABLES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* GLOBAL VARIABLES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*********************************************************************************************************
* LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
void __attribute__((__interrupt__)) _U1RXInterrupt (void);
void __attribute__((__interrupt__)) _U1TXInterrupt (void);
//void Uart1RxApp(void);
//void Uart1RxErrApp(void);
void Modbus_Slave_App(void);
//void Uart1App(void);
//void Uart1TxModbus(void);
//void Uart1TxCRCerr(void);
void InitUart1(void);
unsigned int CRC16_Check(unsigned char *Pushdata,unsigned char length);
void Modbus_Start_Transmit(unsigned char len);

void Update_Modbus_Registers(void);
/*
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
*/
//tU1TxRx	U1TxRx;
/*
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* 函数名称: Modbus_Slave_App
* 功能描述: 在主循环中调用，解析并响应功能码 03, 06, 16
*********************************************************************************************************
*/
void Modbus_Slave_App(void)
{
    unsigned int startAddr, regNum, i;
    unsigned int crcCalc, crcRecv;
    
    if (MB.FrameReady == 0) return; // 无新帧则返回

    // 1. 基本校验：站号匹配及最小长度校验
    if (MB.RxBuf[0] != StartParams.modbusaddr || MB.RxCnt < 4) goto RESET_MB;

    // 2. CRC 校验
    crcCalc = CRC16_Check(MB.RxBuf, MB.RxCnt - 2);
    crcRecv = (unsigned int)MB.RxBuf[MB.RxCnt - 2] << 8 | MB.RxBuf[MB.RxCnt - 1];
    if (crcCalc != crcRecv) goto RESET_MB;

    // 3. 功能码处理
    switch (MB.RxBuf[1]) 
    {
        case 0x03: // 读多个保持寄存器
            startAddr = (unsigned int)MB.RxBuf[2] << 8 | MB.RxBuf[3];
            regNum    = (unsigned int)MB.RxBuf[4] << 8 | MB.RxBuf[5];
            
            if ((startAddr + regNum) > MB_REG_MAX) break; // 超限不做响应或发异常码

            MB.TxBuf[0] = StartParams.modbusaddr;
            MB.TxBuf[1] = 0x03;
            MB.TxBuf[2] = (unsigned char)(regNum * 2); // 字节数
            for (i = 0; i < regNum; i++) {
                MB.TxBuf[3 + i * 2] = (unsigned char)(Modbus_Regs[startAddr + i] >> 8);
                MB.TxBuf[4 + i * 2] = (unsigned char)(Modbus_Regs[startAddr + i] & 0xFF);
            }
            Modbus_Start_Transmit(3 + regNum * 2);
            break;

        case 0x06: // 写单个寄存器
            startAddr = (unsigned int)MB.RxBuf[2] << 8 | MB.RxBuf[3];
            if (startAddr < MB_REG_MAX) {
                Modbus_Regs[startAddr] = (unsigned int)MB.RxBuf[4] << 8 | MB.RxBuf[5];
                // 写入后的特殊逻辑处理：起停控制
                if (startAddr == REG_START_CMD && Modbus_Regs[startAddr] == 0x00AA) Input.Start = 1;
                if (startAddr == REG_STOP_CMD  && Modbus_Regs[startAddr] == 0x00BB) Input.Stop = 1;
                
                // 原样返回应答帧
                for(i=0; i<8; i++) MB.TxBuf[i] = MB.RxBuf[i];
                Modbus_Start_Transmit(8);
            }
            break;

        case 0x10: // 写多个寄存器 (16进制 10H)
            startAddr = (unsigned int)MB.RxBuf[2] << 8 | MB.RxBuf[3];
            regNum    = (unsigned int)MB.RxBuf[4] << 8 | MB.RxBuf[5];
            if ((startAddr + regNum) <= MB_REG_MAX) {
                for (i = 0; i < regNum; i++) {
                    Modbus_Regs[startAddr + i] = (unsigned int)MB.RxBuf[7 + i * 2] << 8 | MB.RxBuf[8 + i * 2];
                }
                // 返回确认帧 (站号+功能码+地址+数量+CRC)
                for(i=0; i<6; i++) MB.TxBuf[i] = MB.RxBuf[i];
                Modbus_Start_Transmit(6);
            }
            break;

        default:
            // 未定义功能码异常处理
            break;
    }

RESET_MB:
    MB.RxCnt = 0;
    MB.FrameReady = 0;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*/
void InitUart1(void)
{
    // 1. 设置引脚方向 (48引脚 5011 专用)
    TRISFbits.TRISF2 = 1;     // RF2 作为 U1RX 输入
    TRISFbits.TRISF3 = 0;     // RF3 作为 U1TX 输出
    
    // 2. 彻底关闭冲突外设
    SPI1STATbits.SPIEN = 0;   // 确保 SPI1 没有抢占 RF2/RF3

    // 3. 配置波特率 (假设 Fcy=7.3728M, 9600bps 则为 11, 若 Fcy=29.49M 则为 191)
    U1BRG = BAUD_RATE; 

    // 4. 配置模式
    U1MODE = 0;
    U1MODEbits.UARTEN = 1;    // 开启 UART
    U1MODEbits.ALTIO = 0;     // 48脚封装: 0 使用 RF2/RF3; 1 使用备用引脚(如有)
    
    // 5. 配置状态与中断
    U1STA = 0;
    U1STAbits.UTXEN = 1;      // 发送使能
    
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;      // 接收中断使能
    
    // 6. RS485 控制 (高压软起动板通常 0 为收，1 为发)
    TRISFbits.TRISF6 = 0;
    LATFbits.LATF6 = 1;       // 默认进入监听模式
}

/*
*********************************************************************************************************
*********************************************************************************************************
*/
/* UART 接收中断 */
void __attribute__((__interrupt__)) _U1RXInterrupt (void)
{
    unsigned char temp_byte;

    // --- 1. 硬件错误处理 (最高优先级) ---
    if (U1STAbits.OERR == 1) 
    {
        // 发生溢出错误：通常是因为 CPU 处理其他高优先级中断太久，没来得及读 FIFO
        U1STAbits.OERR = 0;   // 硬件规范：必须清零 OERR 才能继续接收
        MB.ErrorCount++;      // 记录错误，用于现场通讯稳定性评估
    }
    
    if (U1STAbits.FERR == 1)
    {
        // 发生帧错误：通常是电磁干扰（dv/dt）或波特率严重偏离
        U1STAbits.FERR = 0;   // 清除错误，丢弃当前受损字节
    }

    // --- 2. 接收 FIFO 处理 ---
    // 使用 while 循环确保读完硬件缓冲区里的所有字节
    while (U1STAbits.URXDA) 
    {
        temp_byte = U1RXREG;  // 读取硬件寄存器

        // --- 3. 状态机与保护逻辑 ---
        if (MB.FrameReady == 0) 
        {
            // 正常状态：缓冲区空闲，允许存入数据
            if (MB.RxCnt < 128) 
            {
                MB.RxBuf[MB.RxCnt] = temp_byte;
                MB.RxCnt++;
                
                // 收到合法字节，重置 3.5T 定时器
                // 注意：这里仅负责重置，由专门的 Timer 中断负责累加和判断超时
                MB.Timer35T = 0; 
            }
            else 
            {
                // 异常：单帧报文超过了定义的缓冲区长度
                // 策略：通常选择重置计数器，等待下一帧，或者记录错误
                MB.RxCnt = 0; 
            }
        }
        else 
        {
            // 异常：主循环尚未处理完上一帧，新字节就到达了
            // 策略：在高压 MPS 环境下，为了保证当前正在处理指令的完整性，
            // 我们选择丢弃这个字节，不覆盖原有缓冲区（防止 CRC 校验在计算中被篡改）
            MB.ErrorCount++; 
        }
    }

    // --- 4. 清除中断标志 ---
    IFS0bits.U1RXIF = 0;
}



/* 启动发送函数 */
void Modbus_Start_Transmit(unsigned char len)
{
    unsigned int crc;
    crc = CRC16_Check(MB.TxBuf, len);
    MB.TxBuf[len] = (unsigned char)(crc >> 8);
    MB.TxBuf[len + 1] = (unsigned char)(crc & 0xFF);
    
    MB.TxLen = len + 2;
    MB.TxPtr = 0;
    
    LATFbits.LATF6 = 0; // RS485 切换至发送模式 (DE 引脚有效)
    U1TXREG = MB.TxBuf[MB.TxPtr++]; // 触发首个字符发送
}

/* UART 发送中断 */
void __attribute__((__interrupt__)) _U1TXInterrupt (void)
{
    IFS0bits.U1TXIF = 0;
    if (MB.TxPtr < MB.TxLen) {
        U1TXREG = MB.TxBuf[MB.TxPtr++];
    } else {
        // 等待硬件移位寄存器发完后再拉高 DE，防止末尾字节截断
        while(!U1STAbits.TRMT); 
        LATFbits.LATF6 = 1; // 切换回接收模式
    }
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
void Update_Modbus_Registers(void)
{
    // 将关键运行数据同步到映射表
    Modbus_Regs[REG_SYS_STATUS] = SysStatus;
    Modbus_Regs[REG_FAULT_CODE] = Fault.Byte;
    
    // 针对你的 Ia <= 5A 的逻辑：
    if(SysStatus != START && MainParams.Ia <= 5) {
        Modbus_Regs[REG_CURRENT_A] = 0;
    } else {
        Modbus_Regs[REG_CURRENT_A] = MainParams.Ia;
    }
    
    Modbus_Regs[REG_VOLTAGE_A] = MainParams.Ua;
    // 温度等其他参数...
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
//高字节地位输出，低字节高位输出
unsigned int CRC16_Check(unsigned char *Pushdata,unsigned char length)  
{  
  unsigned int  Reg_CRC = 0xffff;  
  unsigned char Temp_reg = 0x00;  
  unsigned char i,j;   
 	
  for( i = 0; i<length ;i++)
   {  
   Reg_CRC^= *Pushdata++;  
   for (j = 0; j<8; j++)  
   {       
   	if (Reg_CRC & 0x0001)  
   	Reg_CRC=Reg_CRC>>1^0xA001;  
	else  
	Reg_CRC >>=1;  
   }    
   }  
   Temp_reg=Reg_CRC>>8;  
   return (Reg_CRC<<8|Temp_reg);  
} 


