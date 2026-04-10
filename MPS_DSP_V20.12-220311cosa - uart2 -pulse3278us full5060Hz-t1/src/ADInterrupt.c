//襄樊大力工业控制股份有限公司，高压固态起动装置，DSPIC30F5011系统
//AD转换中断程序文件，ADInterrupt.c，根据运行模式，进行转差矢量控制计算

//头文件
#include "UserParams.h"	//用户系统配置，常数定义及配置值
#include "Function.h"	//系统外部调用函数声明，内部调用函数在对应文件内声明
#include "variable.h"	//系统全局变量声明，局部变量在对应文件内声明
#include "math.h"
tADI ADI;
unsigned int adii=0;
#define AD_BASE  0

#define FILTER_N2 100

unsigned int   Ia_im;
unsigned int   Ic_im;

void InitAD(void)
{
// 1.配置端口引脚作为模拟输入，将相应位置0
    ADPCFG = 0xFFFF;        // Set all ports digital
    ADPCFGbits.PCFG8 = 0;   // AN8 analog
    ADPCFGbits.PCFG9 = 0;   // AN9 analog
    ADPCFGbits.PCFG10 = 0;   // AN10 analog
    ADPCFGbits.PCFG11 = 0;   // AN11 analog
    ADPCFGbits.PCFG12 = 0;   // AN12 analog
    ADPCFGbits.PCFG13 = 0;   // AN13 analog
    ADPCFGbits.PCFG14 = 0;   // AN14 analog
    ADPCFGbits.PCFG15 = 0;   // AN15 analog
    
    
// 2.选择参考电压源 ADCON2的13——15位
    ADCON2 = 0x0000;        
    ADCON2bits.VCFG = 3;    // 使用外部参考电源 VREF+,VREF-
    ADCON2bits.CSCNA = 1;   // 扫描输入
//    ADCON2bits.CHPS = 0;    // 转化CH0
    ADCON2bits.SMPI = 7;    // 每8次转化完产成中断
    ADCON2bits.BUFM = 0;	//0 = Buffer configured as one 16-word buffer
    ADCON2bits.ALTS = 0;    // 总是采用MUX A开关

// 3.选择模拟转换时钟ADCON3
    ADCON3 =  0x0000;        //使用内部时钟 
    ADCON3bits.SAMC = 11;    // 0~31Tad;
	ADCON3bits.ADRC = 0;	//0 = Clock derived from system clock
    ADCON3bits.ADCS = 31;    // 0.5Tcy~32Tcy ??????tad =1us 1+14Tad
                                //15us采集刷新一个通道，共8个通道，120us进一次中断
 
// 4.AD输入选择ADCHS
    ADCHS=0x0000;
    ADCHSbits.CH0SA = 8;     // 0000 = Channel 0 positive input  is AN0
    ADCHSbits.CH0NA = 0;    //0 = Channel 0 negative input is VREF           
    
   
// 5.配置ADCSSL扫描选择寄存器 ，不扫描选0
    ADCSSL = 0x0000;    
    ADCSSLbits.CSSL8 = 1;    //扫描输入通道 AN8-ADCBUF0-Uab
    ADCSSLbits.CSSL9 = 1;    //AN9-ADCBUF1 -Ubc
    ADCSSLbits.CSSL10 = 1;   //AN10-ADCBUF2
    ADCSSLbits.CSSL11 = 1;   //AN11-ADCBUF3-In
    ADCSSLbits.CSSL12 = 1;   //AN12-ADCBUF4-Ia
    ADCSSLbits.CSSL13 = 1;   //AN13-ADCBUF5-Ib
    ADCSSLbits.CSSL14 = 1;   //AN14-ADCBUF6-Ic
    ADCSSLbits.CSSL15 = 1;   //AN15-ADCBUF7-T
    
// 6.配置采样/转换过程ADCON1
    ADCON1 = 0;
	
    ADCON1bits.FORM = 1;    // 01 = Signed integer (DOUT = ssss sddd dddd dddd)
	ADCON1bits.SSRC = 7;    //  111 = (auto convert)
    ADCON1bits.ASAM = 1;    // 转换结束，自动采样开始为，置1使能
//    ADCON1bits.SIMSAM = 1;	// 同时采样
    
// 7.开启A/D采样通道
    ADCON1bits.ADON = 1;    // 开启 AD 模块  

// 8.配置AD中断
    IPC2bits.ADIP = 5;   
    IFS0bits.ADIF = 0;   //清AD中断标志位
    IEC0bits.ADIE = 1;   //使能AD中断


AdcParams.Count=0;
}

signed int filter_bufUab[FILTER_N2+1];
signed int filter_bufUbc[FILTER_N2+1];
signed int filter_bufUca[FILTER_N2+1];

signed int filter_bufIa[FILTER_N2+1];
signed int filter_bufIb[FILTER_N2+1];
signed int filter_bufIc[FILTER_N2+1];
signed int filter_bufIn[FILTER_N2+1];
signed long filter_bufPa[FILTER_N2+1];


//***************** AD转换中断 ***********************************//
void __attribute__((__interrupt__)) _ADCInterrupt(void)
{   
       int old_sam1;
       int old_sam2;
       int this_sam2;
       int this_sam3;
       int last;
       int diff;
       long qUab1;
       long qUab1_old;
       long qUbc1;
       long qUbc1_old;
       long qUca1;
       long qUca1_old;

       long qIa1;
       long qIa1_old;
       long qIb1;
       long qIb1_old;
       long qIc1;
       long qIc1_old;
       long qIn1;
       long qIn1_old;
       long qPa;
       long qPa_old;
      signed int ad_zero=20;
      
   if(AdcParams.Count<=AdcParams.FILTER_N)
     {AdcParams.Count++;
      if(ADCBUF4>0 &&ADCBUF4<ad_zero)
      {
      ad_zero=ADCBUF4;
      AdcParams.ad_z=ad_zero;
      }
     }
      

         AdcParams.Uab_Iv  = ADCBUF0; //电压
         AdcParams.Ubc_Iv  = ADCBUF1; 
        
         
         
        AdcParams.Uca_Iv = -AdcParams.Uab_Iv-AdcParams.Ubc_Iv;

        AdcParams.In_Iv= ADCBUF3; //零序电流
        
        AdcParams.Ia_Iv=ADCBUF4;  //A相电流

        if(AdcParams.Ia_Iv>0)
        {AdcParams.Ia_Iv=AdcParams.Ia_Iv-AdcParams.ad_z;}
        else
        {AdcParams.Ia_Iv=AdcParams.Ia_Iv-AdcParams.ad_z;}

       	//AdcParams.siAdTemp1 = ADCBUF4;
        //AdcParams.siAdTemp2 = AdcParams.siAdTemp1;

//        if(AdcParams.siAdTemp1<12&&AdcParams.siAdTemp1>-12)
//        {AdcParams.siAdTemp1=0;}

        
       
         AdcParams.Ic_Iv= ADCBUF6;

         if(AdcParams.Ic_Iv>0)
        {AdcParams.Ic_Iv=AdcParams.Ic_Iv-AdcParams.ad_z;}
        else
        {AdcParams.Ic_Iv=AdcParams.Ic_Iv-AdcParams.ad_z;}




       	AdcParams.Ib_Iv = -AdcParams.Ia_Iv-AdcParams.Ic_Iv;



		AdcParams.siAdTemp9 = ADCBUF7;
		AdcParams.siAdTemp9 += 2048;
		AdcParams.TemperatureAccumulate += AdcParams.siAdTemp9;	     

     old_sam1= (AdcParams.this_sam1-AdcParams.FILTER_N1+AdcParams.FILTER_N) %AdcParams.FILTER_N;
     old_sam2= (old_sam1-4+AdcParams.FILTER_N) %AdcParams.FILTER_N;
     this_sam2= (AdcParams.this_sam1-4+AdcParams.FILTER_N) %AdcParams.FILTER_N;
     //计算先入的采样点
     last=(AdcParams.this_sam1-1+AdcParams.FILTER_N) %AdcParams.FILTER_N;

     filter_bufUab[AdcParams.this_sam1]= AdcParams.Uab_Iv;// 后入的采样点 -- Uab采样
     qUab1=__builtin_mulss(filter_bufUab[AdcParams.this_sam1], filter_bufUab[AdcParams.this_sam1]);
     qUab1_old=__builtin_mulss(filter_bufUab[old_sam1],filter_bufUab[old_sam1]);
     AdcParams.siAdTemp4= AdcParams.siAdTemp4+qUab1-qUab1_old; //平方和，并先入先出
   if(AdcParams.Count >=AdcParams.FILTER_N1)
     {
   //  AdcParams.Uab_value=sqrt(AdcParams.siAdTemp4/FILTER_N1);//求平均后开根号
     }  

     filter_bufUbc[AdcParams.this_sam1]= AdcParams.Ubc_Iv;// 后入的采样点 -- Ubc采样
     qUbc1=__builtin_mulss(filter_bufUbc[AdcParams.this_sam1], filter_bufUbc[AdcParams.this_sam1]);
     qUbc1_old=__builtin_mulss(filter_bufUbc[old_sam1],filter_bufUbc[old_sam1]);
     AdcParams.siAdTemp12= AdcParams.siAdTemp12+qUbc1-qUbc1_old; //平方和，并先入先出


     filter_bufUca[AdcParams.this_sam1]= AdcParams.Uca_Iv;//后入的采样点--Uca采样
     qUca1=__builtin_mulss(filter_bufUca[AdcParams.this_sam1], filter_bufUca[AdcParams.this_sam1]);
     qUca1_old=__builtin_mulss(filter_bufUca[old_sam1],filter_bufUca[old_sam1]);
     AdcParams.siAdTemp13= AdcParams.siAdTemp13+qUca1-qUca1_old;  


     filter_bufIa[AdcParams.this_sam1]= AdcParams.Ia_Iv;//后入的采样点--Ia采样
     qIa1=__builtin_mulss(filter_bufIa[AdcParams.this_sam1], filter_bufIa[AdcParams.this_sam1]);
     qIa1_old=__builtin_mulss(filter_bufIa[old_sam1],filter_bufIa[old_sam1]);
     AdcParams.siAdTemp5= AdcParams.siAdTemp5+qIa1-qIa1_old; 
   if(AdcParams.Count >=AdcParams.FILTER_N1)
     {
   //  AdcParams.Ia_value=sqrt(AdcParams.siAdTemp5/FILTER_N1);
     }  

     filter_bufIc[AdcParams.this_sam1]= AdcParams.Ic_Iv;//后入的采样点--Ic采样
     qIc1=__builtin_mulss(filter_bufIc[AdcParams.this_sam1], filter_bufIc[AdcParams.this_sam1]);
     qIc1_old=__builtin_mulss(filter_bufIc[old_sam1],filter_bufIc[old_sam1]);
     AdcParams.siAdTemp7= AdcParams.siAdTemp7+qIc1-qIc1_old;
   if(AdcParams.Count >=AdcParams.FILTER_N1)
     {
   //  AdcParams.Ic_value=sqrt(AdcParams.siAdTemp7/FILTER_N1);
     }  


     filter_bufIb[AdcParams.this_sam1]= AdcParams.Ib_Iv;//后入的采样点--Ib采样
     qIb1=__builtin_mulss(filter_bufIb[AdcParams.this_sam1], filter_bufIb[AdcParams.this_sam1]);
     qIb1_old=__builtin_mulss(filter_bufIb[old_sam1],filter_bufIb[old_sam1]);
     AdcParams.siAdTemp6= AdcParams.siAdTemp6+qIb1-qIb1_old;
   if(AdcParams.Count >=AdcParams.FILTER_N1)
     {
   //  AdcParams.Ib_value=sqrt(AdcParams.siAdTemp6/FILTER_N1);
     }  



if (Protectswitch.R_ZeroI==1 || Protectswitch.S_ZeroI==1)
 {
     filter_bufIn[AdcParams.this_sam1]= AdcParams.In_Iv;//后入的采样点--In采样
     qIn1=__builtin_mulss(filter_bufIn[AdcParams.this_sam1], filter_bufIn[AdcParams.this_sam1]);
     qIn1_old=__builtin_mulss(filter_bufIn[old_sam1],filter_bufIn[old_sam1]);
     AdcParams.siAdTemp8= AdcParams.siAdTemp8+qIn1-qIn1_old;
   if(AdcParams.Count >=AdcParams.FILTER_N1)
     {
   //  AdcParams.In_value=sqrt(AdcParams.siAdTemp8/FILTER_N1);
     }
  }  


   if(Functionswitch.FWD_REV==0)
   {
     qPa=__builtin_mulss(filter_bufUab[this_sam2], filter_bufIa[AdcParams.this_sam1]);
      filter_bufPa[AdcParams.this_sam1]= qPa;// 后入的采样点 -- Pa
     qPa_old=__builtin_mulss(filter_bufUab[old_sam2],filter_bufIa[old_sam1]);
     AdcParams.siAdTemp10= AdcParams.siAdTemp10+qPa-qPa_old; //平方和，并先入先出
   }
   else
   {
     qPa=__builtin_mulss(filter_bufUab[AdcParams.this_sam1], filter_bufIa[this_sam2]);
      filter_bufPa[AdcParams.this_sam1]= qPa;// 后入的采样点 -- Pa
     qPa_old=__builtin_mulss(filter_bufUab[old_sam1],filter_bufIa[old_sam2]);
     AdcParams.siAdTemp10= AdcParams.siAdTemp10+qPa-qPa_old; //平方和，并先入先出

   } 
  


  AdcParams.this_sam1++;
if(AdcParams.this_sam1>=AdcParams.FILTER_N)
  {

    /*
    AdcParams.abc10=filter_bufUbc[AdcParams.this_sam1]+filter_bufUab[AdcParams.this_sam1]+filter_bufUab[AdcParams.this_sam1];
    AdcParams.abc11=filter_bufUbc[AdcParams.this_sam1];
   // AdcParams.abc12=filter_bufUca[AdcParams.this_sam1];
    AdcParams.abc20=filter_bufUbc[last]+filter_bufUab[last]+filter_bufUab[last];
    AdcParams.abc21=filter_bufUbc[last];
    //AdcParams.abc22=filter_bufUca[this_sam2];
    AdcParams.alf1=__builtin_mulss(500,AdcParams.abc10);
    AdcParams.blt1=__builtin_mulss(866,AdcParams.abc11);

    AdcParams.alf2=__builtin_mulss(500,AdcParams.abc20);
    AdcParams.blt2=__builtin_mulss(866,AdcParams.abc21);
    */   
   	AdcParams.TemperatureOrigin = __builtin_divud(AdcParams.TemperatureAccumulate,AdcParams.FILTER_N);
	AdcParams.TemperatureAccumulate = 0;


   AdcParams.this_sam1=0;
  }




  


	IFS0bits.ADIF = 0;   //清AD中断标志位
    return;    
	
}



