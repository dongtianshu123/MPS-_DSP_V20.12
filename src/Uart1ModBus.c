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
#define MB_BUF_SIZE       128
#define MB_MAX_READ_REGS  ((MB_BUF_SIZE - 5) / 2)
#define MB_MAX_WRITE_REGS ((MB_BUF_SIZE - 9) / 2)
unsigned int Modbus_Regs[MB_REG_MAX]; 
extern unsigned int StartOFTOK;
extern unsigned int inputBits;
extern unsigned int outputBits;
    unsigned long ulTempCalc;  // 32位中间变量，防止乘法溢出
    unsigned long  uSpan;       // 量程区间宽度

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
void Write_Modbus_Command(void);
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
/*
*********************************************************************************************************
* 函数名称: Modbus_Slave_App
* 功能描述: Modbus从站协议解析引擎 - 处理读(03)、写(06)、多写(16)
* 优化重点: 增加 0x64/65/66/67 遥控遥调逻辑，强化写入安全性校验
*********************************************************************************************************
*/
void Modbus_Slave_App(void)
{
    unsigned int startAddr, regNum, i;
    unsigned int crcCalc, crcRecv;
    unsigned int writeVal;
    unsigned int byteCnt;
    
    if (MB.FrameReady == 0) return; // 无完整帧则退出

    // 1. 站号与长度基础校验
    if (MB.RxBuf[0] != StartParams.modbusaddr || MB.RxCnt < 4) goto RESET_MB;

    // 2. CRC 校验 (调用你提供的 CRC16_Check)
    crcCalc = CRC16_Check((unsigned char *)MB.RxBuf, MB.RxCnt - 2);
    crcRecv = (unsigned int)MB.RxBuf[MB.RxCnt - 2] << 8 | MB.RxBuf[MB.RxCnt - 1];
    if (crcCalc != crcRecv) goto RESET_MB;

    // 3. 功能码解析
    switch (MB.RxBuf[1]) 
    {
        case 0x03: // --- 读保持寄存器 ---
            if (MB.RxCnt != 8) break;
            startAddr = (unsigned int)MB.RxBuf[2] << 8 | MB.RxBuf[3];
            regNum    = (unsigned int)MB.RxBuf[4] << 8 | MB.RxBuf[5];
            
            if (regNum == 0 || regNum > MB_MAX_READ_REGS) break;
            if (startAddr >= MB_REG_MAX || regNum > (MB_REG_MAX - startAddr)) break;

            MB.TxBuf[0] = StartParams.modbusaddr;
            MB.TxBuf[1] = 0x03;
            MB.TxBuf[2] = (unsigned char)(regNum * 2);
            for (i = 0; i < regNum; i++) {
                MB.TxBuf[3 + i * 2] = (unsigned char)(Modbus_Regs[startAddr + i] >> 8);
                MB.TxBuf[4 + i * 2] = (unsigned char)(Modbus_Regs[startAddr + i] & 0xFF);
            }
            Modbus_Start_Transmit(3 + regNum * 2);
            break;

        case 0x06: // --- 写单个寄存器 (遥控/遥调) ---
            if (MB.RxCnt != 8) break;
            startAddr = (unsigned int)MB.RxBuf[2] << 8 | MB.RxBuf[3];
            writeVal  = (unsigned int)MB.RxBuf[4] << 8 | MB.RxBuf[5];
            
            if (startAddr < MB_REG_MAX) {
                // 将数据存入映射表
                Modbus_Regs[startAddr] = writeVal;

                // 具体的遥控执行逻辑
                switch (startAddr)
                {
                    case 0x0064: // 遥控起动
                        if (writeVal == 0x00AA) {
                            Input.Start = 1; // 触发起动逻辑
                        }
                        break;

                    case 0x0065: // 遥控停止
                        if (writeVal == 0x00BB) {
                            Input.Stop = 1;  // 触发停止逻辑
                        }
                        break;

                    case 0x0066: // 遥控故障复位
                        if (writeVal == 0x00CC) {
                            // 执行故障清除逻辑
                            Fault.Byte = 0;
                            Input.reset = 1; 
                        }
                        break;

                    case 0x0067: // 远程遥调写指令
                        // 直接写入调整参数，范围 0-0xFFFF
                        StartParams.AdjustData = writeVal; 
                        break;
                    case 0x0068: // 调试指令，临时将模式调整到4
                        // 
                        StartParams.ControlMode=4; 
                        break;
                       

                    default:
                        // 其他可写寄存器逻辑
                        break;
                }
                
                // 按照 Modbus 标准：06 功能码响应原报文
                for(i = 0; i < 6; i++) MB.TxBuf[i] = MB.RxBuf[i];
                Modbus_Start_Transmit(6);
            }
            break;

        case 0x10: // --- 写多个寄存器 ---
            if (MB.RxCnt < 9) break;
            startAddr = (unsigned int)MB.RxBuf[2] << 8 | MB.RxBuf[3];
            regNum    = (unsigned int)MB.RxBuf[4] << 8 | MB.RxBuf[5];
            byteCnt   = MB.RxBuf[6];
            if (regNum == 0 || regNum > MB_MAX_WRITE_REGS) break;
            if (byteCnt != regNum * 2) break;
            if (MB.RxCnt != byteCnt + 9) break;
            if (startAddr >= MB_REG_MAX || regNum > (MB_REG_MAX - startAddr)) break;
            for (i = 0; i < regNum; i++) {
                Modbus_Regs[startAddr + i] = (unsigned int)MB.RxBuf[7 + i * 2] << 8 | MB.RxBuf[8 + i * 2];
            }
            for(i = 0; i < 6; i++) MB.TxBuf[i] = MB.RxBuf[i];
            Modbus_Start_Transmit(6);
            break;

        default:
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
            if (MB.RxCnt < MB_BUF_SIZE)
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
    unsigned int i;
    
    if (len > (MB_BUF_SIZE - 2)) {
        MB.ErrorCount++;
        return;
    }

    crc = CRC16_Check((unsigned char *)MB.TxBuf, len);
    MB.TxBuf[len] = (unsigned char)(crc >> 8);
    MB.TxBuf[len + 1] = (unsigned char)(crc & 0xFF);
    
    MB.TxLen = len + 2;
    MB.TxPtr = 0;
    
    // 切换至发送模式 (用户硬件逻辑：0 为发送)
    LATFbits.LATF6 = 0; 
    
    // 采用循环发送，防止中断嵌套导致逻辑混乱
    for(i = 0; i < MB.TxLen; i++)
    {
        while(U1STAbits.UTXBF); // 等待硬件 FIFO 空间
        U1TXREG = MB.TxBuf[i];
    }
    
    // *** 关键：等待移位寄存器清空，确保最后一个 Stop Bit 离开引脚 ***
    while(!U1STAbits.TRMT); 
    
    // --- 影子数据清理 (针对 RS485 自发自收) ---
    {
        unsigned char dummy;
        while(U1STAbits.URXDA) dummy = U1RXREG; // 读空发送产生的回流数据
        U1STAbits.OERR = 0;                     // 强制复位溢出错误
        IFS0bits.U1RXIF = 0;                    // 清除自收导致的中断标志
    }
    
    // 切换回接收模式 (用户硬件逻辑：1 为接收)
    LATFbits.LATF6 = 1;
}

/* UART 发送中断 */
void __attribute__((__interrupt__)) _U1TXInterrupt (void)
{
    IFS0bits.U1TXIF = 0;
    if (MB.TxPtr < MB.TxLen) 
      {
        U1TXREG = MB.TxBuf[MB.TxPtr++];
      } 
    else {
        // 等待硬件移位寄存器发完后再拉高 DE，防止末尾字节截断
        while(!U1STAbits.TRMT); 
        LATFbits.LATF6 = 1; // 切换回接收模式
    }
}
/*
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* 函数名称: Update_Modbus_Registers
* 功能描述: 将系统变量同步至 Modbus 寄存器映射表 (Modbus_Regs[])
* 备注: 严格对应串口 Uart2 的报文逻辑，实现数据跨协议同步
*********************************************************************************************************
*/
void Update_Modbus_Registers(void)
{
    unsigned int inputBits = 0;
    unsigned int outputBits = 0;
    unsigned int SystemStatusBits = 0;
    unsigned int tempStatus = 0;

    // --- 1. 三相电流 (起动/运行状态下显示真实值，否则清零) ---
    if ((SysStatus == START) || (SysStatus == RUN))
    {
        Modbus_Regs[0] = ADI.Ia; 
        Modbus_Regs[1] = ADI.Ib; 
        Modbus_Regs[2] = ADI.Ic; 
    }
    else
    {
        Modbus_Regs[0] = 0;
        Modbus_Regs[1] = 0;
        Modbus_Regs[2] = 0;
    }

    // --- 2. 三相电压 ---
    Modbus_Regs[3] = MainParams.Ua;
    Modbus_Regs[4] = MainParams.Ub;
    Modbus_Regs[5] = MainParams.Uc;

    // --- 3. IO输入点状态封包 (逻辑取反: 0为有效) ---
    if(INPUT_START == 0)    inputBits |= 0x0001;
    if(INPUT_STOP == 0)     inputBits |= 0x0002;
    if(INPUT_READY == 0)    inputBits |= 0x0004;
    if(INPUT_RUN_CHECK == 0)inputBits |= 0x0008;
    if(INPUT_NO_USE == 0)   inputBits |= 0x0010;
    if(INPUT_in6 == 0)      inputBits |= 0x0020;
    if(INPUT_in7 == 0)      inputBits |= 0x0040;
    if(INPUT_in8 == 0)      inputBits |= 0x0080;
    if(INPUT_in9 == 0)      inputBits |= 0x0100;
    if(INPUT_in10 == 0)     inputBits |= 0x0200;
    if(Functionswitch.FWD_REV == 1) inputBits |= 0x8000; // 1为反转
    Modbus_Regs[6] = inputBits;

    // --- 4. IO输出点状态封包 (逻辑: 1为有效) ---
    if(OUTPUT_READY == 1)   outputBits |= 0x0001;
    if(OUTPUT_START == 1)   outputBits |= 0x0002;
    if(OUTPUT_TRIGGER == 1) outputBits |= 0x0004;
    if(OUTPUT_RUN_ON == 1)  outputBits |= 0x0008;
    if(OUTPUT_RUN_OFF == 1) outputBits |= 0x0010;
    if(OUTPUT_ALARM == 1)   outputBits |= 0x0020;
    if(OUTPUT_K5 == 1)      outputBits |= 0x0040;
    if(OUTPUT_K6 == 1)      outputBits |= 0x0080;
    if(OUTPUT_K7 == 1)      outputBits |= 0x0100;
    if(OUTPUT_K8 == 1)      outputBits |= 0x0200;
    Modbus_Regs[7] = outputBits;

    // --- 5. 系统运行状态映射 ---
    tempStatus = SysStatus;
    if (tempStatus == 0 || tempStatus == 1) 
    {
        if (lowVoltageTest == 0xaaaa) Modbus_Regs[8] = 4; // 低压测试状态
        else Modbus_Regs[8] = tempStatus;
    }
    else if (tempStatus == 2 || tempStatus == 3) Modbus_Regs[8] = 2; // 运行/过载运行
    else if (tempStatus == 4) Modbus_Regs[8] = 3; // 软停
    else if (tempStatus == 5) Modbus_Regs[8] = 5; // 故障
    else Modbus_Regs[8] = 0;

    // --- 6. 系统实时参数 ---
    Modbus_Regs[9]  = MainParams.Temperature;
    Modbus_Regs[10] = Fault.Byte;
    Modbus_Regs[11] = StartParams.StartTime1s;
    Modbus_Regs[12] = MainParams.frequency;
    Modbus_Regs[13] = Version;
    Modbus_Regs[14] = compareACnt;
    Modbus_Regs[15] = ProtectParams.intevalmin;

    // --- 7. 系统状态位 SystemStatusBits ---
    if(phasecompareError == 1) SystemStatusBits |= 0x0001; 
    if(Input.Ready == 1)       SystemStatusBits |= 0x0002; 
    if(StartOFTOK == 1)        SystemStatusBits |= 0x0004; 
    if(MainParams.Temperature < ProtectParams.Temperature || Functionswitch.TDetection == 0)
                               SystemStatusBits |= 0x0008; 
    Modbus_Regs[16] = SystemStatusBits;

    // --- 8. 扩展 ADC 参数 ---
    if (Functionswitch.Delay_Zero == 0) Modbus_Regs[17] = LT;
    else Modbus_Regs[17] = MainParams.In;

    Modbus_Regs[18] = StartParams.OutData;
    Modbus_Regs[19] = step11;
    Modbus_Regs[20] = AdcParams.pf;
    Modbus_Regs[21] = AdcParams.acos_max;
    Modbus_Regs[22] = AdcParams.Imax;
    Modbus_Regs[23] = AdcParams.Laststarttime;
    Modbus_Regs[24] = AdcParams.Ud;

  }

void Write_Modbus_Command(void)
 {

    unsigned int begin;
    unsigned int ugmin;

    // 1. 获取上位机下发的原始数据 (来自 Modbus 寄存器 0x0067)
    // 假设 Modbus_Slave_App 已经将数据写入 StartParams.AdjustData
    
    // 2. 计算目标区间的跨度 (Span)
    // 增加防御性编程：确保上限大于下限，防止出现负数或反向映射
    if (StartParams.Ugmin < StartParams.BeginVoltage)
    {
        ugmin=StartParams.Ugmin>>10;
        begin=StartParams.BeginVoltage>>10;
        uSpan = begin-ugmin;

        // 3. 线性缩放计算: Y = A + (X * Span) / 32767
        // 强制转换为 unsigned long 确保 32 位乘法执行
        ulTempCalc = (unsigned long)StartParams.AdjustData * uSpan;
        
        // 执行除法并取整归一化
        // 加上 16383 (即 32767/2) 可实现四舍五入效果，提高控制精度
        StartParams.Data_remote = StartParams.Ugmin + (unsigned long)(((ulTempCalc + 16383) / 32767)<<10);
    }

    else
    {
        // 上下限相等的情况
        StartParams.Data_remote = StartParams.BeginVoltage;
    }

     
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


