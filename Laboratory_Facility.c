#include <string.h>

#define F_CPU 72000000 //Тактируется от STLINK - 8MHz, PLL:ON, PLLMUL:0111, input clock x9
//--------------------------------------------------------------------------------------------------------------------------
#define RCC_AHBENR   (*(volatile unsigned long *) 0x40021014) //AHB peripheral clock enable register (RCC_AHBENR)
#define RCC_APB2ENR  (*(volatile unsigned long *) 0x40021018) //APB2 peripheral clock enable register

//--------------------------------------------------------------------------------------------------------------------------
#define GPIOE_MODER (*(volatile unsigned long *) 0x48001000) //GPIO port mode register (GPIOx_MODER)
#define GPIOE_ODR   (*(volatile unsigned long *) 0x48001014) //GPIO port output data register (GPIOx_ODR) (x = A..H)

//--------------------------------------------------------------------------------------------------------------------------
#define GPIOA_MODER (*(volatile unsigned long *) 0x48000000) //GPIO port mode register (GPIOx_MODER)
#define GPIOA_AFRL  (*(volatile unsigned long *) 0x48000024) //GPIO alternate function high register


//--------------------------------------------------------------------------------------------------------------------------
#define USART_BRR  (*(volatile unsigned long *) 0x4001380C) // Baud rate register
#define USART_CR1  (*(volatile unsigned long *) 0x40013800) // Control register 1
#define USART_ISR  (*(volatile unsigned long *) 0x4001381C) // Interrupt and status register
#define USART1_TDR (*(volatile unsigned long *) 0x40013828) // Transmit data register
#define USART1_RDR (*(volatile unsigned long *) 0x40013824) // Receive data register
//--------------------------------------------------------------------------------------------------------------------------

#define NVIC_ISER1 (*(volatile unsigned long *) 0xE000E104) // Interrupt set-enable registers

void _delay(unsigned long int index);
unsigned int str_len(const unsigned char *str); //Длина строки
void sendByteUSART1(unsigned char txData); // Передаем байт по USART
void sendStringUSART1(unsigned const char *txData); // Передаем строку по USART

int flag = 0;



int main(void)
{

	RCC_AHBENR |= 1<<21; //port E clock enable
	RCC_AHBENR |= 1<<17; //port A clock enable


	GPIOE_MODER |= (0<<31)|(1<<30);  //General purpose output mode
	GPIOE_MODER |= (0<<29)|(1<<28);  //General purpose output mode
	GPIOE_MODER |= (0<<11)|(1<<10);


	//USART1_TX.PA9.AF7
	GPIOA_MODER |= (1<<19)|(0<<18);  //PA9 AF mode
	//PA9 AF mode
	GPIOA_AFRL |= (0<<7)|(1<<6|1<<5)|(1<<4);  //PA9 in AF7 mode

	//USART1_RX.PA10.AF7
	GPIOA_MODER |= (1<<21)|(0<<20);
	GPIOA_AFRL |= (0<<11)|(1<<10|1<<9)|(1<<8);  //PA10 in AF7 mode
	//USART1 init
	RCC_APB2ENR |= 1<<14; //USART1 clock enable

	USART_BRR = 0x1D4C; //Budrate 9600

	USART_CR1  |= 1<<0; //USART Enabled
	USART_CR1  |= 1<<3;//Transmitter enable
    USART_CR1  |= 1<<2;//Reciver enable

    USART_CR1  |= 1<<5; //RXNE interupt enabled
        NVIC_ISER1 |= 1<<5; //Разрешить прерывание 37, USART1
        asm volatile ("CPSIE I"); //Глобальное разрешение прерываний

//	unsigned char *txData = "Start Programm\n";
    sendStringUSART1("Start Programm\n");

	//Конец программы
    while(1)
    {
    	//if (USART_ISR & (1<<7)) //Передаем байт
    		//USART1_TDR = 0x41;
    	//sendByteUSART1(0x41);

    	GPIOE_ODR |= (1<<15); //PE15 is HIGH
    	_delay(2000000);
    	GPIOE_ODR &= ~(1<<15); //PE15 is LOW
    	_delay(2000000);
    }
}

void _delay(unsigned long int index) //Задержка
{
	while(index--);
}
//Возращает длину строки

unsigned int str_len(const unsigned char *str)
{
	unsigned int lenght = 0;
	while(*str++) ++lenght;
	return lenght;
}

// Передаем байт по USART
void sendByteUSART1(unsigned char txData)
{
	while(!(USART_ISR & (1<<7))); // Ждем пока осовободиться TDR
			USART1_TDR = txData; //Передаем байт
}
// Передаем строку по USART
void sendStringUSART1(unsigned const char *txData)
{
	int lenght = str_len(txData);
	for (int i=0; i<lenght; ++i)
		{
			while(!(USART_ISR & (1<<7))); // Ждем пока осовободиться TDR
			USART1_TDR = *txData++;

		}
}

void USART1_IRQHandler()
{
  unsigned char data = USART1_RDR;
 // GPIOE_ODR |= (1<<14); //PE15 is HIGH
  if (flag == 0)
  {
	  GPIOE_ODR |= (1<<14);
	  GPIOE_ODR |= (1<<5);
	  flag =1;
  }
  else if (flag == 1)
		{
	  GPIOE_ODR &= ~(1<<5);
	  GPIOE_ODR  &= ~(1<<14);
	  flag = 0;
		}

 // if (strcmp(data, "f"))
  sendStringUSART1("Answer!\n");
 // GPIOE_ODR |= (1<<14); //PE15 is HIGH
  //else
	//  sendStringUSART1(data);
}

/*unsigned int strlen(unsigned char *str)
{
	unsigned int lenght = 0;
	unsigned int index = 0;
	while(str[index]!=0)
	{
		++lenght;
		++index;
	}

	return lenght;
}
unsigned int str_len(const unsigned char *str)
{
	return (*str) ? str_len(++str) + 1 : 0;
}
*/

