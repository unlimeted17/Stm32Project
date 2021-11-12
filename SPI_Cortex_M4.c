

#define F_CPU 72000000 //Тактируется от STLINK - 8MHz, PLL:ON, PLLMUL:0111, input clock x9

//--------------------------------------------------------------------------------------------------------------------------
#define RCC_CR          (*(volatile unsigned long *) 0x40021000) //Clock control register(RCC_CR)
#define RCC_CFGR        (*(volatile unsigned long *) 0x40021004) //Clock configuration register(RCC_CFGR)
#define RCC_AHBENR      (*(volatile unsigned long *) 0x40021014) //AHB peripheral clock enable register (RCC_AHBENR)
#define RCC_APB2ENR     (*(volatile unsigned long *) 0x40021018) //APB2 peripheral clock enable register
#define RCC_APB1ENR     (*(volatile unsigned long *) 0x4002101C) //APB1 peripheral clock enable register

#define FLASH_ACR       (*(volatile unsigned long *) 0x40022000) // Flash access control register
//--------------------------------------------------------------------------------------------------------------------------
#define GPIOE_MODER (*(volatile unsigned long *) 0x48001000) //GPIO port mode register (GPIOx_MODER)
#define GPIOE_ODR   (*(volatile unsigned long *) 0x48001014) //GPIO port output data register (GPIOx_ODR) (x = A..H)


//--------------------------------------------------------------------------------------------------------------------------
#define GPIOA_MODER     (*(volatile unsigned long *) 0x48000000) //GPIO port mode register (GPIOA_MODER)
#define GPIOA_AFRL      (*(volatile unsigned long *) 0x48000024) //GPIO alternate function high register
#define GPIOA_OSPEEDR   (*(volatile unsigned long *) 0x48000008)
#define GPIOA_IDR       (*(volatile unsigned long *) 0x48000010)

//--------------------------------------------------------------------------------------------------------------------------
#define  GPIOB_MODER (*(volatile unsigned long*)  0x48000400) //GPIO port mode register (GPIOB_MODER)
#define  GPIOB_AFRH  (*(volatile unsigned long*)  0x48000424) //GPIO alternate function high register
#define GPIOB_ODR    (*(volatile unsigned long *) 0x48000414) //
#define GPIOB_BSRR   (*(volatile unsigned long *) 0x48000418) // GPIO port bit set/reset register
//--------------------------------------------------------------------------------------------------------------------------
#define USART_BRR  (*(volatile unsigned long *) 0x4001380C) // Baud rate register
#define USART_CR1  (*(volatile unsigned long *) 0x40013800) // Control register 1
#define USART_ISR  (*(volatile unsigned long *) 0x4001381C) // Interrupt and status register
#define USART1_TDR (*(volatile unsigned long *) 0x40013828) // Transmit data register
#define USART1_RDR (*(volatile unsigned long *) 0x40013824) // Receive data register
//--------------------------------------------------------------------------------------------------------------------------
#define SYSCFG_CFGR1    (*(volatile unsigned long *) 0x40010000) //SYSCFG configuration register 1 (SYSCFG_CFGR1)
#define SYSCFG_EXTICR3  (*(volatile unsigned long *) 0x40010010) //SYSCFG external interrupt configuration register 3

#define EXTI_IMR1   (*(volatile unsigned long *) 0x40010400) // Interrupt mask register
#define EXTI_FTSR1   (*(volatile unsigned long *) 0x4001040C) // Interrupt mask register
#define EXTI_PR1   (*(volatile unsigned long *) 0x40010414) // Pending register

#define NVIC_ISER1 (*(volatile unsigned long *) 0xE000E104) // Interrupt set-enable registers

//--------------------------------------------------------------------------------------------------------------------------
#define SPI2_CR1 (*(volatile unsigned long *) 0x40003800) // SPI control register 1
#define SPI2_CR2 (*(volatile unsigned long *) 0x40003804) // SPI control register 2
#define SPI2_SR (*(volatile unsigned long *) 0x40003808) // SPI status register

#define GPIO_BSRR_BR_10            ((unsigned int)0x04000000)
#define GPIO_BSRR_BS_10            ((unsigned int)0x00000400)

#define		 CS_LOW 	GPIOB_BSRR = GPIO_BSRR_BR_10;
#define 	 CS_HIGH 	GPIOB_BSRR = GPIO_BSRR_BS_10;



void _delay(unsigned long int index);
void sendByteUSART1(unsigned char txData);  // Передаем байт по USART
void sendStringUSART1(unsigned char *txData);  // Передаем строку по USART



void RCC_init()
{
	RCC_CFGR |= (0 << 1 | 1 << 20 | 1 << 19 | 1 << 18);  // PLL multiplication factor
	   //0111: PLL input clock x 9
	RCC_CFGR |= (1 << 16 | 0 << 15);  //PLL entry clock source
	 //10: HSE used as PREDIV1 entry
	    RCC_CR |= (1 << 16);  //HSE clock enable
	 while(!(RCC_CR & (1 << 17)));
	//  sendStringUSART1("HSE oscillator is stable!...\n");
	  FLASH_ACR |= (0 << 0 | 1 << 1 | 0 << 2);
	RCC_CR |= (1 << 24);  //PLL enable
	while(!(RCC_CR & (1 << 17)));  //PLL clock ready flag
	//USART1_Send_String((u8*)"PLL is locked!...\n");
	     RCC_CFGR |= (1 << 1 | 0 << 0);  //System clock switch
	  //  RCC_CFGR = 0x001D040A;
	    while(!(RCC_CFGR & (1 << 3)));  //System clock switch

     
	   // USART_BRR = 72000000/9600;  //Baudrate 9600, 72MHz
	   //sendStringUSART1("PLL selected as system clock!...\n");
	   //USART1_Send_String((u8*)"PLL selected as system clock!...\n");

	   //10: PLL selected as system clock
}
     

int main(void)
{

	RCC_AHBENR |= 1 << 21;  //port E clock enable
	RCC_AHBENR |= 1 << 17;  //port A clock enable
	RCC_AHBENR |= 1 << 18; //port B clock enable
	RCC_APB1ENR |= 1 << 14; // SPI2 clock enabled
	RCC_APB2ENR |= 1 << 0; //  SYSCFG clock enabled
	
	
	
	
	
  RCC_init();
	
	
	GPIOB_MODER &= ~(1 << 21 | 1 << 20);  // Линия INT (PB10) - Input mode
	GPIOB_MODER |= (0 << 13) | (1 << 12);
	EXTI_IMR1 |=  1 << 10;
	EXTI_FTSR1 |=  1 << 10;
	SYSCFG_EXTICR3 &= ~(1 << 9 | 1 << 10 | 1 << 11);
	SYSCFG_EXTICR3 |= 1 << 8;
	NVIC_ISER1 |= 1 << 8; //Разрешить прерывание 40, EXTI15_10_IRQn
	//Линия SCK(PB13, AF5)
	GPIOB_MODER |= (1 << 27 | 0 << 26); //PB13 - Alternate function mode
	GPIOB_AFRH |= (1 << 22 | 1 << 20); // Alternate function AF5
	GPIOB_AFRH &= ~(1 << 23 | 1 << 21); 
	//Линия MISO(PB14, AF5)
	GPIOB_MODER |= (1 << 29 | 0 << 28);  //PB14 - Alternate function mode
	GPIOB_AFRH |= (1 << 26 | 1 << 24);  // Alternate function AF5
	GPIOB_AFRH &= ~(1 << 27 | 1 << 25); 
	//Линия MOSI(PB15, AF5)
	GPIOB_MODER |= (1 << 31 | 0 << 30);   //PB15 - Alternate function mode
	GPIOB_AFRH |= (1 << 30 | 1 << 28);   // Alternate function AF5
	GPIOB_AFRH &= ~(1 << 31 | 1 << 29); 
	
	SPI2_CR1 &= ~(1 << 0); // Bit CPHA
	SPI2_CR1 &= ~(1 << 1);  // Bit CPOL
	SPI2_CR1 |= 1 << 2;  // Master configuration
	SPI2_CR1 |= (1 << 5 | 1 << 4 | 1 << 3 ); // Baud rate contro -  fPCLK/256 = 281,25kHz
	
	SPI2_CR2 |= 1 << 2; // Вывод NSS управляет аппаратным обеспечением
	SPI2_CR2 |= (0 << 11 | 1 << 10 | 1 << 9 | 1<< 8 ); //Формат передаваемых данных
	SPI2_CR2 |= 1 << 12; // Размер кадра 8 бит 
	
	SPI2_CR1 |= 1 << 6; // SPI2 enable
	
	while (SPI2_SR & (1 << 7)) ; // Ждем пока SPI2 занят
	


	GPIOE_MODER |= (0 << 31) | (1 << 30);   //General purpose output mode
    GPIOE_MODER |= (0 << 29) | (1 << 28);   //General purpose output mode
	GPIOE_MODER |= (0 << 27) | (1 << 26);    //General purpose output mode

	//USART1_TX.PA9.AF7
	GPIOA_MODER |= (1 << 19) | (0 << 18);   //PA9 AF mode
	//PA9 AF mode
	GPIOA_AFRL |= (0 << 7) | (1 << 6 | 1 << 5) | (1 << 4);   //PA9 in AF7 mode

	//USART1_RX.PA10.AF7
	GPIOA_MODER |= (1 << 21) | (0 << 20);
	GPIOA_AFRL |= (0 << 11) | (1 << 10 | 1 << 9) | (1 << 8);   //PA10 in AF7 mode

	
	//USART1 init
	RCC_APB2ENR |= 1 << 14;  //USART1 clock enable
	GPIOB_ODR |= (1 << 6);
        
    //   USART_BRR = 8000000/9600;

	USART_BRR = 0x1D4C;  //Budrate 9600

	USART_CR1  |= 1 << 0;  //USART Enabled
	USART_CR1  |= 1 << 3; //Transmitter enable
	USART_CR1  |= 1 << 2; //Reciver enable

	USART_CR1  |= 1 << 5;  //RXNE interupt enabled
 
  //  STK_LOAD = 0x0001193f;
    NVIC_ISER1 |= 1 << 5;  //Разрешить прерывание 37, USART1
    asm volatile("CPSIE I");  //Глобальное разрешение прерываний

//	unsigned char *txData = "Start Programm\n";
    sendStringUSART1("Start Programm\n");



	//Конец программы
    while(1)
	{
		//if (USART_ISR & (1<<7)) //Передаем байт
		//USART1_TDR = 0x41;
		//sendByteUSART1(0x41);

		GPIOE_ODR |= (1 << 15);  //PE15 is HIGH
		sendStringUSART1("An!\n");
		_delay(2000000);
		GPIOE_ODR &= ~(1 << 15);  //PE15 is LOW
		_delay(2000000);
	}

}

void _delay(unsigned long int index) //Задержка
{
	while (index--) ;
}
//Возращает длину строки



// Передаем байт по USART
void sendByteUSART1(unsigned char txData)
{
	while (!(USART_ISR & (1 << 7))) ; // Ждем пока осовободиться TDR
			USART1_TDR = txData;  //Передаем байт
}
// Передаем строку по USART
void sendStringUSART1(unsigned char *txData)
{
	while (*txData)
	{
		USART1_TDR = *txData++;
		while (!(USART_ISR & (1 << 7))) ; //Ждём пока освободится TDR
	}
}

void USART1_IRQHandler(void)
{
	GPIOE_ODR |= (1 << 14);  //PE15 is HIGH
	_delay(2000000);
	unsigned char data = USART1_RDR;
	sendStringUSART1("Answer!\n");
	GPIOE_ODR &= ~(1 << 14);  //PE15 is LOW
	_delay(2000000);

}

void EXTI15_10_IRQHandler(void)
{
	GPIOE_ODR |= (1 << 13);   //PE15 is HIGH
	_delay(2000000);
	GPIOE_ODR &= ~(1 << 13);
	EXTI_PR1 |= 1 << 10;	
}

