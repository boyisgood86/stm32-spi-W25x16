//---------------------------------------------------------------------------------
//
//								  W25X80驱动程序（.C）
//									
//																
//						 	  作者：赌神    2011年2月24日
//---------------------------------------------------------------------------------

#include <C8051F320.H>

//=================================================================================
//                                   W25X80预定义
//=================================================================================
// W25X80A命令宏定义
#define FLASH_CMD_WREN              0x06    //写允许
#define FLASH_CMD_RDSR              0x05    //读状态:指令+1~N数据
#define FLASH_CMD_READ              0x03    //读数据:指令+3字节地址+1~N数据
#define FLASH_CMD_PP                0x02    //页编程:指令+3字节地址,1~256页号
#define FLASH_CMD_BE                0xC7    //芯片擦除

/*

#define FLASH_CMD_WRDI              0x04    //写禁止
#define FLASH_CMD_WRSR              0x01    //写状态:指令+1
#define FLASH_CMD_FAST_READ         0x0B    //快速读数据:指令+3字节地址+1字节任意数据+1~N数据
#define FLASH_CMD_SE                0xD8    //块擦除:指令+3字节地址
#define FLASH_CMD_DP                0xB9    //深度节电
#define FLASH_CMD_RES               0xAB    //退出深度节电:指令+3字节任意数据+1~N数据

*/

//W25X80A的控制接口定义
sbit W25X_CS    =   P1^4;		//W25X80A CS
sbit W25X_DO    =   P1^5;		//W25X80A DO
sbit W25X_CLK   =   P1^3;		//W25X80A CLK
sbit W25X_DIO   =   P1^2;		//W25X80A DIO

//常量预定义
#define W25Xdelay     1         //定义延时时间

//------------------------------------------------------------------------------------------
//=================================================================================
//                                   内部函数定义
//=================================================================================
//---------------------------------------------------
//函数功能：延时函数
//---------------------------------------------------
static void delay(int count)
{
	while(count--);
}
//---------------------------------------------------
//函数功能：向W25X80写入一个字节数据
//---------------------------------------------------
void WriteW25X(unsigned char x)
{
    unsigned char i;
		delay(W25Xdelay);
    W25X_CLK = 1;
		delay(W25Xdelay);
    for (i=0; i<8; i++)
    {
        W25X_CLK=0;
        delay(W25Xdelay);
        W25X_DIO =(bit) (x & 0x80);
        delay(W25Xdelay);
        W25X_CLK=1;
        delay(W25Xdelay);
        x <<=1;
    }
}
//---------------------------------------------------
//函数功能：从W25X80读出一个字节数据
//---------------------------------------------------
unsigned char ReadW25X(void)
{
    unsigned char i;
    unsigned char x;
    W25X_CLK = 1 ;
    for (i = 0,x=0 ; i < 8 ; i++)
    {
        W25X_CLK=0;
        delay(W25Xdelay);
        x<<=1;
        if (W25X_DO)
        x |= 0x01;
        W25X_CLK=1;
        delay(W25Xdelay);
    }
	return x;
}
//---------------------------------------------------
//函数功能：测试擦除是否完成
//---------------------------------------------------
void WaitFlash(void)
{  
	unsigned char x=1;
	while(x&0x01)
	{
		W25X_CS=0;
		WriteW25X(FLASH_CMD_RDSR);
		x=ReadW25X();
		W25X_CS=1;   
    }			
}
//---------------------------------------------------
//函数功能：打开写使能位
//---------------------------------------------------
void WriteEnable(void)
{
	W25X_CS=0;
	WriteW25X(FLASH_CMD_WREN);   //写使能，命令字为 06h
	W25X_CS=1;	     
}
//------------------------------------------------------------------------------------------
//=================================================================================
//                                   接口函数定义
//=================================================================================
//---------------------------------------------------
//函数功能：配置交叉开关，设置输入输出方式
//---------------------------------------------------    
void W25X80_Port_Init()
{                   
	P1MDOUT |= 0x3E;                   
  XBR0	= 0x00;						    
	XBR1	= 0x40;
	P1 &= 0xFD;					 
}
//---------------------------------------------------
//函数功能：从W25X80的某地址addr读count个字节
//---------------------------------------------------
void  ReadFlash(unsigned long addr,unsigned char *rdata,unsigned char count)
{	  
	/*此处容易出错，如果定义为unsigned char xtada *rdata 读取可能出错！*/
	W25X_CS=0;
	WriteW25X(FLASH_CMD_READ);  //写入命令字03h
	(void)WriteW25X((unsigned char)(addr>>16));
	(void)WriteW25X((unsigned char)(addr>>8));
	(void)WriteW25X((unsigned char)(addr>>0));	//写入地址addr
	while(count-->0)
	{ 
		delay(W25Xdelay);
		*rdata++=ReadW25X();
    }
	W25X_CS=1;
}
//---------------------------------------------------
//函数功能：向W25X80的某地址addr写入count个字节
//要求：每次不超过128个字节
//---------------------------------------------------
void ProgramFlash(unsigned long addr, unsigned char * wdata, int len)
{	  
		int nblock;  
		while(len > 0)
    {
		nblock = (int)(256 - (addr & 0xFF));
		if(len < nblock)
			nblock = len;
		WriteEnable();
		W25X_CS=0;
		(void)WriteW25X(FLASH_CMD_PP);     //page program
		(void)WriteW25X((unsigned char)(addr>>16));
		(void)WriteW25X((unsigned char)(addr>>8));
		(void)WriteW25X((unsigned char)(addr>>0));
		len  -= nblock;
		while(nblock-- > 0)
        {
					delay(W25Xdelay);
					WriteW25X(*wdata++);
        }
						W25X_CS=1;
						WaitFlash();
     }	
}
//---------------------------------------------------
//函数功能：整体擦除芯片
//---------------------------------------------------
void ChipErase(void)
{
   WriteEnable();
   W25X_CS=0;
   WriteW25X(FLASH_CMD_BE);   //整体擦除芯片,命令字为 C7h或者60h
   W25X_CS=1;		 
   WaitFlash();	      //测试擦除是否完成
}
//------------------------------------------------------------------------------------------