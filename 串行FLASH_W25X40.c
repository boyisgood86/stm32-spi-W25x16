W25X40  51单片机读写程序  

华邦串行FLASH W25X40 读写程序范例

适用于华邦W25X系列，单通道模式；

/****************************************************************************/
/*     深圳恒森微电子有限公司  （华邦单片机香港及中国区代理商）          */
/*     香港恒森科技有限公司    （华邦单片机香港及中国区代理商）          */
/*     陈文    email：chenwen@hengsen.cn (有 bug 请提交）                   */
/****************************************************************************/
/*  文件名: spidrv.c              */
/* 版本     1.0            */
/*  创建日期:    2007-10-29 10:58        */
/*  概述: 本文件为 w79e823   读写 SPI flash程序       */
/*--------------------------------------------------------------------------*/
/*  历史修改记录：               */
/*  名字                   日期                   注释      */
/*  陈文                   2007.10.28           ver 1.0      */
/****************************************************************************/
#include <w79e825.h>
#include <intrins.h>
#define uint8 unsigned char
#define uint16 unsigned int
#define uchar unsigned char
#define uint unsigned int
#define uint32 unsigned long

sbit _cs= P1^2;
sbit _do= P1^3;
sbit _wp= P1^4;
//sbit _hold= P1^5;
sbit _clk= P1^6;
sbit _di= P1^7;

#define W25P_WriteEnable 0x06
#define W25P_WriteDisable 0x04
#define W25P_ReadStatusReg 0x05
#define W25P_WriteStatusReg 0x01
#define W25P_ReadData  0x03
#define W25P_FastReadData 0x0B
#define W25P_PageProgram 0x02
#define W25P_SectorErase 0xD8 // 64KB
#define W25P_ChipErase  0xC7
#define W25P_PowerDown  0xB9
#define W25P_ReleasePowerDown 0xAB
#define W25P_DeviceID  0xAB
#define W25P_ManufactDeviceID 0x90
 
void init_cpu(void);
void IO_Send_Byte(uchar out);
uchar IO_Get_Byte();
void delay(uchar tt);
void IO_Wait_Busy();
void IO_init();
uchar IO_Read_StatusReg();
void IO_Write_StatusReg(byte);
void IO_Write_Enable();
void IO_PowerDown();
void IO_ReleasePowerDown();
uchar IO_Read_ID1();
uint IO_Read_ID2(uchar ID_Addr);
uchar IO_Read_Byte(uint32 Dst_Addr);
void IO_Read_nBytes(uint32 Dst_Addr, uchar nBytes_128);
uchar IO_FastRead_Byte(uint32 Dst_Addr);
void IO_FastRead_nBytes(uint32 Dst_Addr, uchar nBytes_128);
void IO_Write_Byte(uint32 Dst_Addr, uchar byte);
void IO_Write_nBytes(uint32 Dst_Addr, uchar nBytes_128);
void IO_Erase_Chip();
void IO_Erase_Sector(uint32 Dst_Addr);
void Verify(uchar byte, uchar cor_byte);
void IO_Write_Disable();
void trace(uchar *str,uchar len);
uint IO_Read_ID3();

uint8 Rxtemp;
bit MYTI;
uint8 tx_buff[16];
uint8 upper_128[16];
bit rx_ok;
#define nop() _nop_()

void main(void)
{ 
 uint i;
 
 init_cpu(); 
 IO_init();
 for(;;)
 {   
  if(rx_ok == 1)
  {
   rx_ok = 0;
   switch(Rxtemp)
   {
    case 0x01:
     Rxtemp = 0;
     tx_buff[0] = IO_Read_ID1();
     trace(tx_buff,1);
     break;
    case 0x02:
    i = IO_Read_ID2(0x00);
    tx_buff[1] = (uchar)i;
    tx_buff[0] = (uchar)(i>>8);
    trace(tx_buff,2);
    
    break;
    case 0x03:
    i = IO_Read_ID3();
    tx_buff[1] = (uchar)i;
    tx_buff[0] = (uchar)(i>>8);
    trace(tx_buff,3);
    break;
    case 0x04:
    tx_buff[0] = IO_Read_Byte(0x00000000);
    trace(tx_buff,1);
    break;
    case 0x05:
    tx_buff[0] = 0x55;
    IO_Write_Byte(0x00000000,0xa5);
    trace(tx_buff,1);
    break;
    case 0x06:
    tx_buff[0] = IO_Read_StatusReg();
    trace(tx_buff,1);
    break;
    case 0x07:
    IO_Write_Enable(); 
    break;
    case 0x08:
    upper_128[0]=0x01;upper_128[1]=0x02;upper_128[2]=0x03;upper_128[3]=0x04;
    IO_Write_nBytes(0x00000000,4);
    
    break;
    case 0x09:
    IO_Erase_Chip();
    break;
    case 0x0a:
    IO_Erase_Sector(0x00000000);
    break;
    default:
    break;
   }
   
  }
  
   
 }
}

void init_cpu(void)
{  
 TMOD = 0x21;   
 PCON = PCON | 0x80;  //波特率加倍
 //CKCON = 0x10;           //定时器1采用4分频
 SCON  = 0x50;   //异步、10位、波特率可变，无校验位
 TH1   = 0xf8;            //在15M晶振下，波特率是9600，
    TL1   = 0xf8;   
    TR1   = 1;     //T1 timer run
    ES    = 1;    //uart interrupt enable 
 EA = 1;     //all interrupt enable
 
}


//串口中断程序
void UART_isr(void) interrupt 4
{ 
 if(RI)
    {
        RI = 0;
        Rxtemp = SBUF;   //接收
        //SBUF = Rxtemp;   //发送
        rx_ok = 1;
  return;
 }
 if(TI)
 {
  TI = 0;
  MYTI = 1;  
  }
}

void IO_Send_Byte(uchar out)
{
 uchar i = 0; 
 _cs = 0;
 for (i = 0; i < 8; i++)
 {
  if ((out & 0x80) == 0x80) /* check if MSB is high */
   _di = 1;
  else
   _di = 0;  /* if not, set to low */
  _clk = 1;   /* toggle clock high */
  out = (out << 1);  /* shift 1 place for next bit */
  nop();nop();nop();nop();
  _clk = 0;   /* toggle clock low */
 }
}

uchar IO_Get_Byte()
{
 uchar i = 0, in = 0, temp = 0; 
 _cs = 0;
 for (i = 0; i < 8; i++)
 {
  in = (in << 1);  /* shift 1 place to the left or shift in 0 */
  temp = _do;  /* save input */
  _clk = 1;  /* toggle clock high */
  if (temp == 1)  /* check to see if bit is high */
   in |= 0x01; /* if high, make bit high */
  _clk = 0;  /* toggle clock low */
 } 
 return in;
}

void delay(uchar tt)
{
 while(tt--);
}

void IO_Wait_Busy()
{
/*  waste time until not busy WEL & Busy bit all be 1 (0x03). */
 while (IO_Read_StatusReg() == 0x03)
  IO_Read_StatusReg();
}

void IO_init()
{
 _clk = 0; /* set clock to low initial state for SPI operation mode 0 */
// _clk = 1; /* set clock to low initial state for SPI operation mode 3 */
// _hold = 1;
 _wp = 1;
 _cs = 1;
 
 IO_Write_Disable();
 
}

uchar IO_Read_StatusReg()
{
 uchar byte = 0;
 _cs = 0;   /* enable device */
 IO_Send_Byte(W25P_ReadStatusReg); /* send Read Status Register command */
 byte = IO_Get_Byte();   /* receive byte */
 _cs = 1;    /* disable device */
 
 return byte;
}

void IO_Write_StatusReg(byte)
{
 _cs = 0;    /* enable device */
 IO_Send_Byte(W25P_WriteStatusReg); /* select write to status register */
 IO_Send_Byte(byte); /* data that will change the status(only bits 2,3,7 can be written) */
 _cs = 1;    /* disable the device */
}

void IO_Write_Enable()
{
 _cs = 0;   /* enable device */
 IO_Send_Byte(W25P_WriteEnable); /* send W25P_Write_Enable command */
 _cs = 1;   /* disable device */
}

void IO_PowerDown()
{
 _cs = 0;;    /* enable device */
 IO_Send_Byte(W25P_PowerDown);  /* send W25P_PowerDown command 0xB9 */
 _cs = 1;;    /* disable device */
 delay(6);    /* remain CS high for tPD = 3uS */
}

void IO_ReleasePowerDown()
{
 _cs = 0;   /* enable device */
 IO_Send_Byte(W25P_ReleasePowerDown); /* send W25P_PowerDown command 0xAB */
 _cs = 1;    /* disable device */
 delay(6);    /* remain CS high for tRES1 = 3uS */
}

uchar IO_Read_ID1()
{
 uchar byte;
 _cs = 0;  /* enable device */
 IO_Send_Byte(W25P_DeviceID); /* send read device ID command (ABh) */
 IO_Send_Byte(0);  /* send address */
 IO_Send_Byte(0);  /* send address */
 IO_Send_Byte(0);  /* send 3_Dummy address */
 byte = IO_Get_Byte();  /* receive Device ID byte */
 
 _cs  = 1;   /* disable device */
 delay(4);   /* remain CS high for tRES2 = 1.8uS */

 return byte;
}

uint IO_Read_ID2(uchar ID_Addr)
{
 uint IData16;
 _cs = 0;   /* enable device */
 IO_Send_Byte(W25P_ManufactDeviceID); /* send read ID command (90h) */
    IO_Send_Byte(0x00);   /* send address */
 IO_Send_Byte(0x00);   /* send address */
 IO_Send_Byte(ID_Addr);   /* send W25Pxx selectable ID address 00H or 01H */
 IData16 = IO_Get_Byte()<<8;  /* receive Manufature or Device ID byte */
 IData16 |= IO_Get_Byte();  /* receive Device or Manufacture ID byte */
 _cs = 1;    /* disable device */
 
 return IData16;
}

uint IO_Read_ID3()
{
 uint IData16;
 _cs = 0;   /* enable device */
 IO_Send_Byte(0x9f); /* send read ID command (90h) */
    
 IData16 = IO_Get_Byte()<<8;  /* receive Manufature or Device ID byte */
 IData16 |= IO_Get_Byte();  /* receive Device or Manufacture ID byte */
 tx_buff[2] = IO_Get_Byte(); 
 _cs = 1;    /* disable device */
 
 return IData16;
}

uchar IO_Read_Byte(uint32 Dst_Addr) 
{
 uchar byte = 0; 

 _cs = 0;     /* enable device */
 IO_Send_Byte(W25P_ReadData);    /* read command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16)); /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 byte = IO_Get_Byte();
 _cs = 1;   /* disable device */
 
 return byte;   /* return one byte read */
}

void IO_Read_nBytes(uint32 Dst_Addr, uchar nBytes_128)
{
 uint32 i = 0;
 
 _cs = 0;     /* enable device */
 IO_Send_Byte(W25P_ReadData);    /* read command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16));  /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 for (i = 0; i < nBytes_128; i++)  /* read until no_bytes is reached */
 {
  upper_128[i] = IO_Get_Byte(); /* receive byte and store at address 80H - FFH */
 }
 
 _cs = 1;    /* disable device */
 
}

uchar IO_FastRead_Byte(uint32 Dst_Addr) 
{
 uchar byte = 0; 

 _cs = 0;     /* enable device */
 IO_Send_Byte(W25P_FastReadData);   /* fast read command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16)); /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 IO_Send_Byte(0xFF);  /*dummy byte*/
 byte = IO_Get_Byte();
 _cs = 1;   /* disable device */
 
 return byte;   /* return one byte read */
}

void IO_FastRead_nBytes(uint32 Dst_Addr, uchar nBytes_128)
{
 uchar i = 0;
 
 _cs = 0;     /* enable device */
 IO_Send_Byte(W25P_FastReadData);   /* read command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16));  /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 IO_Send_Byte(0xFF);   /*dummy byte*/
 for (i = 0; i < nBytes_128; i++) /* read until no_bytes is reached */
 {
  upper_128[i] = IO_Get_Byte(); /* receive byte and store at address 80H - FFH */
 }
 _cs = 1;    /* disable device */

}

void IO_Write_Byte(uint32 Dst_Addr, uchar byte)
{
 _cs = 0;     /* enable device */
 IO_Write_Enable();    /* set WEL */
 IO_Wait_Busy();
 
 _cs = 0; 
 IO_Send_Byte(W25P_PageProgram);   /* send Byte Program command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16)); /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 IO_Send_Byte(byte);   /* send byte to be programmed */
 _cs = 1;    /* disable device */
}

void IO_Write_nBytes(uint32 Dst_Addr, uchar nBytes_128)
{ 
 uchar i, byte; 
 _cs = 0;     /* enable device */
 IO_Write_Enable();    /* set WEL */
 _cs = 0;
 IO_Send_Byte(W25P_PageProgram);   /* send Byte Program command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16)); /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 
 for (i = 0; i < nBytes_128; i++)
 {
  byte = upper_128[i];
  IO_Send_Byte(byte);  /* send byte to be programmed */
 } 
 _cs = 1;    /* disable device */
 
 //printf("\nPage program (%d nBytes)! please waiting....\n");
}

void IO_Erase_Chip()
{
 _cs = 0;    /* enable device */
 IO_Write_Enable();    /* set WEL */
 _cs = 0;
 IO_Wait_Busy();
 _cs = 0;
 IO_Send_Byte(W25P_ChipErase);  /* send Chip Erase command */
 _cs = 1;    /* disable device */
}

void IO_Erase_Sector(uint32 Dst_Addr)
{
 _cs = 0;     /* enable device */
 IO_Write_Enable();    /* set WEL */
 _cs = 0;
 IO_Send_Byte(W25P_SectorErase);   /* send Sector Erase command */
 IO_Send_Byte(((Dst_Addr & 0xFFFFFF) >> 16));  /* send 3 address bytes */
 IO_Send_Byte(((Dst_Addr & 0xFFFF) >> 8));
 IO_Send_Byte(Dst_Addr & 0xFF);
 _cs = 1;     /* disable device */
}

void Verify(uchar byte, uchar cor_byte)
{
 if (byte != cor_byte)
 {
  while(1);
   //LED_Error = 0; /* display to view error on LED. */   
 }
}

void IO_Write_Disable()
{
 _cs = 0;    /* enable device */
 IO_Send_Byte(W25P_WriteDisable); /* send W25P_Write_Disable command */
 _cs = 1;    /* disable device */
}

void myputchar(uchar c)
{ 
 ES = 0;
 SBUF = c;
 while (TI == 0);
 TI = 0;
 ES = 1;
}

void trace(uchar *str,uchar len)
{
 uint i;
 for(i=0;i<len;i++)
 {
  myputchar(*str);
  str++;
 }
}

 

//IO_Read_StatusReg  Reads the status register of the serial flash
//IO_Write_StatusReg  Performs a write to the status register
//IO_Write_Enable   Write enables the serial flash
//IO_Write_Disable  Write disables the serial flash
//IO_Read_ID1   Reads the device ID using the instruction 0xAB
//IO_Read_ID2   Reads the manufacturer ID and device ID with 0x90
//IO_Read_Byte   Reads one byte from the serial flash and returns byte(max of 20 MHz CLK frequency)
//IO_Read_nBytes   Reads multiple bytes(max of 20 MHz CLK frequency)
//IO_FastRead_Byte  Reads one byte from the serial flash and returns byte(max of 33 MHz CLK frequency)
//IO_FastRead_nBytes  Reads multiple bytes(max of 33 MHz CLK frequency)
//IO_Write_Byte   Program one byte to the serial flash
//IO_Write_nBytes   Program n bytes to the serial flash, n<=256
//IO_Erase_Chip   Erases entire serial flash
//IO_Erase_Sector   Erases one sector (64 KB) of the serial flash
//IO_Wait_Busy   Polls status register until busy bit is low

