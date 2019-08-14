#include "stm32f10x.h"
#include "GPIO.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Область для объявления переменных 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Ожидание "аля-трамвай"
void GPIO_delay(uint32_t tick)
{ 
//	uint32_t wait = 0;
// 	tick = tick * 8000;
//	
//	while(tick) 
// {
//  __ASM("NOP"); 
//	wait +=1; 
//  tick--;
// } 
__ASM("NOP"); __ASM("NOP"); __ASM("NOP"); __ASM("NOP"); __ASM("NOP"); 	
}
//

// Настройка портов ввода/вывода
void GPIO_Init(void)
{
	// Подключаем перефирийную систему GPIO к шине тактирования
	// Подключаем систему альтернативных функций для GPIO к шине тактирования
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;
	
	// Отключаем подвязку выводов JTAG на PB3 и PB4 (теперь будут как обычные выводы PB3 и PB4)
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	
	// Включили переназначение выводов Таймера-3 на:
	// PC6 - Таймера 3 канал 1 
	// PC7 - Таймера 3 канал 2 
	// PC8 - Таймера 3 канал 3 
	// PC9 - Таймера 3 канал 4 
	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP;
	
	// LED подсветка GLCD - вывод PA10 - вывод работает как выход
	GPIOA->CRH &= ~( GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
	GPIOA->CRH |= ( GPIO_CRH_MODE10_0 );
	
	// GLCD подключен к интерфейсу SPI1
	// MOSI (RW) - вывод PA7 - альтернативная функция
	// Reset     - вывод PA0 - вывод работает как выход
	// CLK (E)   - вывод PA5 - альтернативная функция
	// CS (RS)   - вывод PA4 - вывод работает как выход
	GPIOA->CRL &= ~( GPIO_CRL_MODE0 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_MODE7 | GPIO_CRL_CNF0 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF7 );
	GPIOA->CRL |= ( GPIO_CRL_MODE0_0 | GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1 );
	
	// Цифровые входы 0..5 (PB0 - PB5) с подтяжкой к Vcc = 6 штук 
	// PB6 - I2C SCL - альтернативная функция, "open drain" , 50MHz
	// PB7 - I2C SDA - альтернативная функция, "open drain" , 50MHz
	GPIOB->CRL &= ~( GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_MODE6 | GPIO_CRL_MODE7 );
	GPIOB->CRL &= ~( GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF6 | GPIO_CRL_CNF7 );
	GPIOB->CRL |= ( GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_1 | GPIO_CRL_CNF4_1 | GPIO_CRL_CNF5_1 | GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7 );
	GPIOB->ODR |= ( GPIO_ODR_ODR0 | GPIO_ODR_ODR1 | GPIO_ODR_ODR2 | GPIO_ODR_ODR3 | GPIO_ODR_ODR4 | GPIO_ODR_ODR5 );
	
	// Цифровые выходы (PB8, PB9, PB12, PB13) = 4 штуки
	GPIOB->CRH &= ~( GPIO_CRH_MODE8 | GPIO_CRH_MODE9 | GPIO_CRH_MODE12 | GPIO_CRH_MODE13 );
	GPIOB->CRH &= ~( GPIO_CRH_CNF8 | GPIO_CRH_CNF9 | GPIO_CRH_CNF12 | GPIO_CRH_CNF13 );
	GPIOB->CRH |= ( GPIO_CRH_MODE8_0 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE12_0 | GPIO_CRH_MODE13_0 );
	
	// Матрица тактильных кнопок на плате
	// KI0 = PB14 - вывод работает как вход c подтяжкой к Gnd
	// KI1 = PB15 - вывод работает как вход c подтяжкой к Gnd
	// KB0 = PC11 - вывод работает как выход
	// KB1 = PC12 - вывод работает как выход
	GPIOB->CRH &= ~( GPIO_CRH_MODE14 | GPIO_CRH_MODE15 | GPIO_CRH_CNF14 | GPIO_CRH_CNF15 );
	GPIOB->CRH |= ( GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1 );
	GPIOB->ODR &= ~( GPIO_ODR_ODR14 | GPIO_ODR_ODR15 );
	GPIOC->CRH &= ~( GPIO_CRH_MODE11 | GPIO_CRH_MODE12 | GPIO_CRH_CNF11 | GPIO_CRH_CNF12 );
	GPIOC->CRH |= ( GPIO_CRH_MODE11_0 | GPIO_CRH_MODE12_0 );
	
	// Конфигурация выводов для аналоговых входных данных:
	// PC0 - 1-й аналоговый вход, для АЦП это AIN_10
	// PC1 - 2-й аналоговый вход, для АЦП это AIN_11
	// PC2 - 3-й аналоговый вход, для АЦП это AIN_12
	// PC3 - 4-й аналоговый вход, для АЦП это AIN_13
	GPIOC->CRL &= ~( GPIO_CRL_MODE0 | GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF3 );
		
	// PC4 - красный светодиод на плате
	// PC5 - зеленый светодиод на плате
	GPIOC->CRL &= ~( GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5 );
	GPIOC->CRL |= ( GPIO_CRL_MODE4_0 | GPIO_CRL_MODE5_0 );
	
	// Конфигурация выводов для генерации ШИМ сигналов (аналоговые выходные сигналы):
	// PC6 - как PWM выход (генератор импульсов), альтернативная функция
	// PC7 - как PWM выход (генератор импульсов), альтернативная функция
	// PC8 - как PWM выход (генератор импульсов), альтернативная функция
	// PC9 - как PWM выход (генератор импульсов), альтернативная функция
	GPIOC->CRL &= ~( GPIO_CRL_CNF6 | GPIO_CRL_MODE6 | GPIO_CRL_CNF7 | GPIO_CRL_MODE7 );
	GPIOC->CRL |= ( GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6 | GPIO_CRL_CNF7_1  | GPIO_CRL_MODE7 );
	GPIOC->CRH &= ~( GPIO_CRH_CNF8 | GPIO_CRH_MODE8 | GPIO_CRH_CNF9 | GPIO_CRH_MODE9 );
	GPIOC->CRH |= ( GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8 | GPIO_CRH_CNF9_1  | GPIO_CRH_MODE9 );
		
}
//

// Вращение вперед
void FC_rotate_Fwd(void)
{
D0_out_OFF;
GPIO_delay(5);	
D1_out_ON;
}

// Вращение назад
void FC_rotate_Rev(void)
{
D1_out_OFF;
GPIO_delay(5);	
D0_out_ON;	
}

// Остановить вращение
void FC_rotate_Stop(void)
{
D0_out_OFF;
D1_out_OFF;
GPIO_delay(5);
}	

// Увеличение натяжения
void FC_tension_Up(void)
{
D2_out_OFF;
GPIO_delay(5);	
D3_out_ON;
}

// Уменьшение натяжения
void FC_tension_Down(void)
{
D3_out_OFF;
GPIO_delay(5);	
D2_out_ON;
}

// Блокировка усилия
void FC_tension_Hold(void)
{
D2_out_OFF;
D3_out_OFF;
GPIO_delay(5);
}


//

