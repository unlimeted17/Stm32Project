 
#define PWR_CR      (*(volatile unsigned long *) 0x40007000) //PWR power control register
#define PWR_CSR     (*(volatile unsigned long *) 0x40007004) //PWR power control/status register
#define RCC_CR      (*(volatile unsigned long *) 0x40023800) //RCC clock control register
#define RCC_PLLCFGR (*(volatile unsigned long *) 0x40023804) //RCC PLL configuration register
#define RCC_AHB1ENR (*(volatile unsigned long *) 0x40023830) //RCC AHB1 peripheral clock register
#define RCC_APB2ENR (*(volatile unsigned long *) 0x40023844) //RCC APB2 peripheral clock enable register
#define RCC_APB1ENR (*(volatile unsigned long *) 0x40023840) //RCC APB1 peripheral clock enable register
#define RCC_BDCR    (*(volatile unsigned long *) 0x40023870) //RCC backup domain control register
#define RCC_CFGR    (*(volatile unsigned long *) 0x40023808) //RCC clock configuration register
#define FLASH_ACR   (*(volatile unsigned long *) 0x40023C00) //Flash access control register
//--------------------------------------------------------------------------------------------------------------------------
#define GPIOA_MODER (*(volatile unsigned long *) 0x40020000) //GPIO port mode register
#define GPIOA_ODR   (*(volatile unsigned long *) 0x40020014) //GPIO port mode register
#define GPIOA_AFRL  (*(volatile unsigned long *) 0x40020024) //GPIO alternate function high register

//--------------------------------------------------------------------------------------------------------------------------
#define USART_CR1 (*(volatile unsigned long *) 0x40011000) // Control register 1
#define USART_BRR (*(volatile unsigned long *) 0x4001100C) // Baud rate register
#define USART_ISR (*(volatile unsigned long *) 0x4001101C) // Interrupt and status registerфв
#define USART_TDR (*(volatile unsigned long *) 0x40011028) // Transmit data register
#define USART_RDR (*(volatile unsigned long *) 0x40011024) // Receive data register

//--------------------------------------------------------------------------------------------------------------------------
#define NVIC_ISER1 (*(volatile unsigned long *) 0xE000E104) //Interrupt set-enabled register

void _delay(volatile unsigned long int index);

void RCC_Init()
{
  RCC_CR |= 1<<16;
while(!(RCC_CR & (1<<17))); //HSE clock ready flag

//PLLSRC: Main PLL(PLL) and audio PLL (PLLI2S) entry clock source
RCC_PLLCFGR |= 1<<22;

// PLLM[5:0]: Division factor for the main PLLs (PLL, PLLI2S and PLLSAI) input clock
RCC_PLLCFGR &= ~(1<<4);
RCC_PLLCFGR |= (0<<5)|(0<<4)|(0<<3)|(1<<2)|(0<<1)|(0<<0); // PLLM 4

 //PLLN[8:0]:  Main PLL (PLL) multiplication factor for VCO // 216 Mhz
RCC_PLLCFGR |= (0<<14)|(1<<13)|(1<<12)|(0<<11)|(1<<10)|(1<<9)|(0<<8)|(0<<7)|(0<<6);

//PLLQ[3:0]: Main PLL (PLL) division factor for USB OTG FS, SDMMC1/2 and random number generator clocks
RCC_PLLCFGR &= ~(1<<26);
RCC_PLLCFGR |= (0<<27)|(0<<26)|(1<<25)|(0<<24); //  PLLQ = 2

// PLLR[2:0]: PLL division factor for DSI clock
RCC_PLLCFGR &= ~(1<<30);
RCC_PLLCFGR &= ~(1<<29);
RCC_PLLCFGR &= ~(1<<28); // 000: PLLR = 0, wrong configuration


PWR_CSR |= 1<<14; // VOSRDY: Regulator voltage scaling output selection ready bit
RCC_CR |= 1<<24; // Set and cleared by software to enable PLL
while(!(RCC_CR & (1<<25))); // Set by hardware to indicate that PLL is locked

PWR_CR |= 1<<16; //  Bit 16 ODEN: Over-drive enable
while(!(PWR_CSR & (1<<16))); //Bit 16 ODRDY: Over-drive mode ready
PWR_CR |= 1<<17; // Bit 17 ODSWEN: Over-drive switching enabled
while(!(PWR_CSR & (1<<17))); // Bit 17 ODSWRDY: Over-drive mode switching ready

//VOS[1:0]: Regulator voltage scaling output selection
PWR_CR |=  (1<<15)|(1<<14); // 11: Scale 1 mode (reset value)

//LATENCY[3:0]: Latency These bits represent the ratio of the CPU clock period to the Flash memory access time.
FLASH_ACR |= (1<<0)|(1<<1)|(1<<2)|(0<<3);  //Seven wait state

//PPRE2: APB high-speed prescaler (APB2)
RCC_CFGR |= (1<<15)|(0<<14)|(0<<13); // AHB clock divided by 2
//PPRE1: APB Low-speed prescaler (APB1)
RCC_CFGR |= (1<<12)|(0<<11)|(1<<10); // 101: AHB clock divided by 4
//SW: System clock switch
RCC_CFGR |= (1<<1) |(0<<0); //10: PLL selected as system clock
//SWS: System clock switch status
RCC_CFGR |= (1<<3) |(0<<2); //10: PLL used as the system clock(Я так понимаю тут тоже проверка должна быть)
}


int main(void)
{

RCC_APB1ENR |= 1<<28;
RCC_APB2ENR |= 1<<14;
RCC_Init();



RCC_AHB1ENR |= 1<<0; //IO port A clock enable
RCC_APB2ENR |= 1<<4; //USART1 clock enabled

GPIOA_MODER |= (1<<19)|(0<<18); //PA9.TX alternate function mode
GPIOA_AFRL |= (0<<7)|(1<<6)|(1<<5)|(1<<4); //PA9 in AF7 mode

GPIOA_MODER |= (1<<21)|(0<<20);
GPIOA_AFRL |= (0<<11)|(1<<10|1<<9)|(1<<8);  //PA10 in AF7 mode

/**Character transmission procedure**/
//1. Program the M bits in USART_CR1 to define the word length.
//M[1:0] = 00: 1 Start bit, 8 data bits, n stop bits

//2. Select the desired baud rate using the USART_BRR register.
//USART_BRR = 16000000/9600; //Budrate 9600

//USART_BRR = 216000000/9600;
USART_BRR = 216000000/19200;
//3. Program the number of stop bits in USART_CR2.
//00: 1 stop bit

//4. Enable the USART by writing the UE bit in USART_CR1 register to 1.
USART_CR1 |= 1<<0; //Bit 0 UE: USART enable

//6. Set the TE bit in USART_CR1 to send an idle frame as first transmission.
USART_CR1 |= 1<<3; //Bit 3 TE: Transmitter enable
USART_CR1 |= 1<<2; //Bit 2 RE: Receiver enable

USART_CR1 |= 1<<5; //Bit 5 RXNEIE: Interrupt enable
NVIC_ISER1 |= (1<<5);

asm volatile("CPSIE I");  //Глобальное разрешение прерываний


while(1)
    {
        /*7. Write the data to send in the USART_TDR register (this clears the TXE bit). Repeat this
        for each data to be transmitted in case of single buffer.*/


        /*8. After writing the last data into the USART_TDR register, wait until TC=1. This indicates
        that the transmission of the last frame is complete. This is required for instance when
        the USART is disabled or enters the Halt mode to avoid corrupting the last
        transmission.*/
        while(!(USART_ISR & (1<<7)));
        USART_TDR = 'S';
      //  led = !led;
       // thread_sleep_for(BLINKING_RATE_MS);

        _delay(6000000);
    }



}


void _delay(unsigned long int index) //Задержка
{
	while(index--);
}

void sendStringUSART1(unsigned char *txData)
{
  while(*txData)
    {
	USART_TDR = *txData++;
	while(!(USART_ISR & (1<<7))); //Ждём пока освободится TDR
    }
}


void USART1_IRQHandler(void)
{
  
 unsigned char data = USART_RDR;
  sendStringUSART1("Answer!\n");
}
