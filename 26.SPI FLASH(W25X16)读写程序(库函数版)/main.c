/******************** (C) COPYRIGHT 2013 STMicroelectronics ********************
* File Name          : main.c
* Author             : WWW.ARMJISHU.COM֮STM32�����Ŷ�  JESSE 
* Version            : V2.0.1
* Date               : 01/09/2013
* Description        : STM32����III�ſ�����    ����
*******************************************************************************/
#include "stm32f10x.h"
#include "flash.h" 
#include "delay.h"
#include <stdio.h>

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define RCC_KEY1                                    RCC_APB2Periph_GPIOD
#define GPIO_KEY1_PORT                              GPIOD    
#define GPIO_KEY1                                   GPIO_Pin_3

#define RCC_KEY2                                    RCC_APB2Periph_GPIOA
#define GPIO_KEY2_PORT                              GPIOA  
#define GPIO_KEY2                                   GPIO_Pin_8

#define RCC_KEY3                                    RCC_APB2Periph_GPIOC
#define GPIO_KEY3_PORT                              GPIOC    
#define GPIO_KEY3                                   GPIO_Pin_13 

#define RCC_KEY4                                    RCC_APB2Periph_GPIOA
#define GPIO_KEY4_PORT                              GPIOA    
#define GPIO_KEY4                                   GPIO_Pin_0 

#define GPIO_KEY_ANTI_TAMP                          GPIO_KEY3
#define GPIO_KEY_WEAK_UP                            GPIO_KEY4

/* Private variables ---------------------------------------------------------*/
/* Values magic to the Board keys */
#define  NOKEY  0
#define  KEY1   1
#define  KEY2   2
#define  KEY3   3
#define  KEY4   4


/*Ҫд�뵽W25X16���ַ�������*/
const u8 TEXT_Buffer[]={"����III�� SPI ��д���ʳ���"};
#define SIZE sizeof(TEXT_Buffer)


USART_InitTypeDef USART_InitStructure;

/* Private function prototypes -----------------------------------------------*/
u8 ReadKeyDown(void);
void GPIO_KEY_Config(void);
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
void STM32_Shenzhou_COMInit(USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable UART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(USART1, USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}

int main(void)
{
	u8 key;
	u8 i=0;
	u8 datatemp[SIZE];
			   
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     

  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOF, GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9);


   
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM32_Shenzhou_COMInit(&USART_InitStructure);
  printf("\n\r--------------------------------------------- ");
  printf("\n\r\n ");
  printf("\n\r����III�� \n");

  printf("\n\r ############ WWW.ARMJISHU.COM! ############ ("__DATE__ " - " __TIME__ ")");

  printf("\r\n");
  printf("  _____ _______ __  __ ____ ___  ______ __  ___\r\n");
  printf(" / ____|__   __|  \\/  |___ \\__ \\|  ____/_ |/ _ \\\r\n");
  printf("| (___    | |  | \\  / | __) | ) | |__   | | | | |_  __\r\n");
  printf(" \\___ \\   | |  | |\\/| ||__ < / /|  __|  | | | | \\ \\/ /\r\n");
  printf(" ____) |  | |  | |  | |___) / /_| |     | | |_| |>  <\r\n");
  printf("|_____/   |_|  |_|  |_|____/____|_|     |_|\\___//_/\\_\\\r\n");
  printf("\r\n");

  printf("\n\r WWW.ARMJISHU.COM use __STM32F10X_STDPERIPH_VERSION %d.%d.%d\n\r",
			__STM32F10X_STDPERIPH_VERSION_MAIN,
			__STM32F10X_STDPERIPH_VERSION_SUB1,
			__STM32F10X_STDPERIPH_VERSION_SUB2);
  printf("\n\r ��Ʒ�ڲ�Flash��СΪ��%dK�ֽڣ� \t www.armjishu.com\n\r",  *(__IO uint16_t*)(0x1FFFF7E0));
  printf("\n\r--DS1��˸: ������������");
  printf("\n\r--��USER2��:��SPI FLASH��W25X16��д��һ���ַ�");
  printf("\n\r--��USER1��:��SPI FLASH��W25X16������֮ǰд����ַ�");
  printf("\n\r--------------------------------------------- ");
  
  GPIO_KEY_Config();

  SPI_Flash_Init();   
  		 
    while(SPI_Flash_ReadID()!=FLASH_ID)//��ⲻ��W25X16
    {	   
		i=SPI_Flash_ReadID();
		printf("\n\r ID:%d",i);
		printf("\n\rû�ж�����ȷ��W25X16оƬID������Ӳ������");
		Delay(0xaFFFF);
		Delay(0xaFFFF);
		GPIO_ResetBits(GPIOF,  GPIO_Pin_7);
		GPIO_SetBits(GPIOF,  GPIO_Pin_7);

	}

	  
	while(1)
	{
		key=ReadKeyDown();
		if(key==KEY1)//KEY1����,д��SPI FLASH
		{
 			printf("\n\r��ʼд��W25X16 SPI FLASHоƬ....");
			SPI_Flash_Write((u8*)TEXT_Buffer,1000,SIZE);//��1000�ֽڴ���ʼ,д��SIZE���ȵ�����
			printf("\n\rд�����!");//��ʾ�������
		}
		if(key==KEY2)//KEY2����,��ȡд����ַ����ַ�������ʾ
		{
 			printf("\n\r��ʼ��W25X16 SPI FLASHоƬ��ȡ����.... ");
			SPI_Flash_Read(datatemp,1000,SIZE);//��1000��ַ����ʼ,����SIZE���ֽ�
			printf("\n\r��ȡ��ɣ�����������Ϊ: %s  ",datatemp);//��ʾ�������
		}

		i++;
		Delay(0xFFFF);
		if(i>0&& i<100)
		{
			GPIO_SetBits(GPIOF, GPIO_Pin_6);;//��ʾϵͳ��������	
		}
		else if(i >= 100 && i < 200)
		{
			GPIO_ResetBits(GPIOF, GPIO_Pin_6);;//��ʾϵͳ��������	
		}	
		i = i % 200;	   
	}

  
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}


/*******************************************************************************
* Function Name  : ReadKey
* Description    : Reads key from demoboard.
* Input          : None
* Output         : None
* Return         : Return RIGHT, LEFT, SEL, UP, DOWN or NOKEY
*******************************************************************************/
u8 ReadKeyDown(void)
{
  /* 1 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY1_PORT, GPIO_KEY1))
  {
    while(!GPIO_ReadInputDataBit(GPIO_KEY1_PORT, GPIO_KEY1))
	{
		;//reserved
	}
    return KEY1; 
  }	
  /* 2 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY2_PORT, GPIO_KEY2))
  {
    while(!GPIO_ReadInputDataBit(GPIO_KEY2_PORT, GPIO_KEY2))
	{
		;//reserved
	}
    return KEY2; 
  }
  /* 3 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY3_PORT, GPIO_KEY3))
  {
    while(!GPIO_ReadInputDataBit(GPIO_KEY3_PORT, GPIO_KEY3))
	{
		;//reserved
	}
    return KEY3; 
  }
  /* 4 key is pressed */
  if(!GPIO_ReadInputDataBit(GPIO_KEY4_PORT, GPIO_KEY4))
  {
    while(!GPIO_ReadInputDataBit(GPIO_KEY4_PORT, GPIO_KEY4))
	{
		;//reserved
	}    
    return KEY4; 
  }
  /* No key is pressed */
  else 
  {
    return NOKEY;
  }
}
void GPIO_KEY_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure KEY1 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY1, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY1;
  GPIO_Init(GPIO_KEY1_PORT, &GPIO_InitStructure);

  /* Configure KEY2 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY2, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY2;
  GPIO_Init(GPIO_KEY2_PORT, &GPIO_InitStructure);

  /* Configure KEY3 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY3, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY3;
  GPIO_Init(GPIO_KEY3_PORT, &GPIO_InitStructure);  

  /* Configure KEY4 Button */
  RCC_APB2PeriphClockCmd(RCC_KEY4, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY4;
  GPIO_Init(GPIO_KEY4_PORT, &GPIO_InitStructure);

}
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
