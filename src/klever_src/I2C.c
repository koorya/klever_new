#include "stm32f10x.h"
#include "I2C.h"          // Библиотека для работы по I2C

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Область для объявления переменных 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Задержки в мкс
void I2C_delay(uint32_t tick)
{ 
	uint32_t wait = 0;
	
	while(tick) 
 {
  __ASM("NOP"); 
	wait +=1; 
  tick--;
 } 
}
//

// Костыльная инициализация выводов SDA и SCL для I2C (согласно Errata стр.26)
void I2C_GPIO_conf_with_Errata(void)
{
	// PB6 - I2C SCL
	// PB7 - I2C SDA
	GPIOB->CRL &= ~( GPIO_CRL_MODE6 | GPIO_CRL_MODE7 );
	GPIOB->CRL &= ~( GPIO_CRL_CNF6 | GPIO_CRL_CNF7 );
	
	// General Output Open-Drain High Level
	// High Level
	GPIOB->CRL |= ( GPIO_CRL_MODE6 | GPIO_CRL_CNF6_0 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7_0 );
	GPIOB->ODR |= ( GPIO_ODR_ODR6 | GPIO_ODR_ODR7 );
	
	// Control SDA SCL High Level
	while(!(GPIOB->IDR & (GPIO_IDR_IDR7 | GPIO_IDR_IDR6))) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
	// SDA Low Level
	GPIOB->ODR &= ~( GPIO_ODR_ODR7 );
	
	// Control SDA Low Level
	while(GPIOB->IDR & GPIO_IDR_IDR7) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;	
	
	// SCL Low Level
	GPIOB->ODR &= ~( GPIO_ODR_ODR6 );
	
	// Control SCL Low Level
	while(GPIOB->IDR & GPIO_IDR_IDR6) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
		// SCL High Level
	GPIOB->ODR |= GPIO_ODR_ODR6;
	
	// Control SCL Low Level
	while(!(GPIOB->IDR & GPIO_IDR_IDR6)) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
	// SDA High Level
	GPIOB->ODR |= GPIO_ODR_ODR7;
	
	// Control SDA High Level
	while(!(GPIOB->IDR & GPIO_IDR_IDR7)) { GPIOC->BSRR = GPIO_BSRR_BS4; }
	GPIOC->BSRR = GPIO_BSRR_BR4;
	
	// PB6 - I2C SCL - альтернативная функция, "open drain" , 50MHz
	// PB7 - I2C SDA - альтернативная функция, "open drain" , 50MHz
	GPIOB->CRL &= ~( GPIO_CRL_MODE6 | GPIO_CRL_MODE7 );
	GPIOB->CRL &= ~( GPIO_CRL_CNF6 | GPIO_CRL_CNF7 );
	GPIOB->CRL |= ( GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7 );	
}
//

// Инициализация I2C (используем I2C2)
void I2C_Init(void)
{
	
// Включили тактирование перефирии I2C
RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;	
	
// Отключаю I2C
I2C1->CR1 &= ~I2C_CR1_PE;
	
// Жду пока отключится
while (I2C1->CR1 & I2C_CR1_PE) {};	

// Костыльная инициализация выводов SDA и SCL для I2C (согласно Errata стр.26)	
I2C_GPIO_conf_with_Errata();

// Сброс конфигурацию I2C
I2C1->CR1 |= I2C_CR1_SWRST;
	
// Убираем сброс конфигурацию I2C
I2C1->CR1 &= ~I2C_CR1_SWRST;
	
// !!!!!!!!! Основная настройка модуля I2C
	
// Частота тактирования модуля I2C = 8 МГц, 1/Fc = 1/8000000 = 125ns
// Standart Mode, 100 kHz = 100 000, 1000ns
I2C1->CCR = 80; //(SystemCoreClock / 100000UL);	
I2C1->TRISE = 9; // ( 1000ns / 125ns ) + 1
	
// Задаем частоту работы модуля I2C => FREQ[5:0] = 8 (MHz)
// Разрешаем генерировать прерывания по событиям от модуля I2C
I2C1->CR2 = 8;
I2C1->CR2 |= I2C_CR2_ITBUFEN;	// I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | 
	
// включили работу интерфейса I2C	
I2C1->CR1 |= I2C_CR1_PE;

// Жду пока включится
while (!(I2C1->CR1 & I2C_CR1_PE)) {};

// Разрешаем обработку прерываний от модуля I2C
// NVIC_EnableIRQ(I2C1_EV_IRQn);
// NVIC_EnableIRQ(I2C1_ER_IRQn);	

// NVIC_SetPriority(I2C1_EV_IRQn, 0);
// NVIC_SetPriority(I2C1_ER_IRQn, 1); // 7
}
//

// Чтение состояний битов в регистре SR1
uint16_t I2C_read_SR1(void)
{
// 
return I2C1->SR1;
}
//

// Чтение состояний битов в регистре SR1
uint16_t I2C_read_SR2(void)
{
// 
return I2C1->SR2;
}
//

// Процедура записи одного слова (одна страница памяти, 16бит, WORD) в EEPROM на шине I2C
char EEPROM_write_DWord(uint8_t device,  uint8_t cell, uint32_t raw)
{
	uint16_t MSB_h = 0, MSB_l = 0;
	uint16_t LSB_h = 0, LSB_l = 0;
	
  uint16_t MSB = ( (raw & 0xFFFF0000) >> 16);
  uint16_t LSB = (raw & 0x0000FFFF);
	
	MSB_h = ( (MSB & 0xFF00) >> 8 );
	MSB_l = ( MSB & 0x00FF );
	LSB_h = ( (LSB & 0xFF00) >> 8 );
	LSB_l = ( LSB & 0x00FF );

	// Старт епта :)	
	I2C1->CR1 |= I2C_CR1_START;	

#define ERR_CNT_LIM 100000
	
	uint32_t err_cnt;

  // Событие EV5
  // Ждем выставления флага START BYTE
	err_cnt = ERR_CNT_LIM;
	while(!( I2C1->SR1 & I2C_SR1_SB ) && --err_cnt) {};
	if(!err_cnt) return 1;
	
	// Читаем безответственно регистр состояния SR1
	(void) I2C1->SR1;
	
	// Выдаем на шину адрес устройства с 0 в конце (режим записи)
	I2C1->DR = I2C_Set_Address(device, I2C_MODE_WRITE); 

	// Ожидаем выставления бита ADDR (адрес устройства принят и понят)
	err_cnt = ERR_CNT_LIM;
	while(!( I2C1->SR1 & I2C_SR1_ADDR ) && --err_cnt) {};
	if(!err_cnt) return 2;
		
	// Читаем безответственно регистры состояния SR1 и SR2
	(void) I2C1->SR1;
	(void) I2C1->SR2;
		
	// Событие EV8
	// Ожидаем выставление бита окончания передачи
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 3;
		
	// Передаем адрес стартовой ячейки	
	I2C1->DR = cell; 
	
	// Событие EV8
	// Ожидаем выставление бита окончания передачи
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 4;
		
	// Отправляем старшую часть слова MSB
	I2C1->DR = MSB_h;	

	// Событие EV8
	// Ожидаем выставление бита окончания передачи
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 5;
		
	// Отправляем младшую часть слова MSB
	I2C1->DR = MSB_l;	
		
	// Событие EV8
	// Ожидаем выставление бита окончания передачи
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 6;
		
	// Отправляем старшую часть слова LSB
	I2C1->DR = LSB_h;	

	// Событие EV8
	// Ожидаем выставление бита окончания передачи
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 7;
		
	// Отправляем младшую часть слова LSB
	I2C1->DR = LSB_l;			

	// Ожидаем выставление бита окончания передачи
	err_cnt = ERR_CNT_LIM;
	while(!(I2C1->SR1 & I2C_SR1_TXE) && --err_cnt){};
	if(!err_cnt) return 8;
		
	// окончание передачи	
	I2C1->CR1 |= I2C_CR1_STOP;	
		
	// пауза после записи	
	I2C_delay(25000);	

	return 0;
}
//

// Процедура чтения одного слова (страницs памяти, 16бит, WORD) из EEPROM на шине I2C
uint16_t EEPROM_read_word(uint8_t device,  uint8_t cell)
{
	uint16_t data_raw = 0;
	uint8_t data_raw_low = 0;
	uint8_t data_raw_high = 0;
	
	// Старт епта :)	
	I2C1->CR1 |= I2C_CR1_START;	
	
  // Событие EV5
  // Ждем выставления флага START BYTE
	while(!( I2C1->SR1 & I2C_SR1_SB )) {}; 
	
	// Читаем безответственно регистр состояния SR1
	(void) I2C1->SR1;
	
	// Выдаем на шину адрес устройства с 0 в конце (режим записи)
	I2C1->DR = I2C_Set_Address(device, I2C_MODE_WRITE);
		
	// Ожидаем выставления бита ADDR (адрес устройства принят и понят)
	while(!( I2C1->SR1 & I2C_SR1_ADDR )) {}; 
		
	// Читаем безответственно регистры состояния SR1 и SR2
	(void) I2C1->SR1;
	(void) I2C1->SR2;	
		
	// Событие EV8
	// Ожидаем выставление бита окончания передачи
  while(!(I2C1->SR1 & I2C_SR1_TXE)){};	
	I2C1->DR = cell;

	// Событие EV8
	// Ожидаем выставление бита окончания передачи
  while(!(I2C1->SR1 & I2C_SR1_TXE)){};

	// Повторный старт епта :)	
	I2C1->CR1 |= I2C_CR1_START;	
	
  // Событие EV5
  // Ждем выставления флага START BYTE
	while(!( I2C1->SR1 & I2C_SR1_SB )) {}; 
	
	// Читаем безответственно регистр состояния SR1
	(void) I2C1->SR1;
	
	// Выдаем на шину адрес устройства с 0 в конце (режим записи)
	I2C1->DR = I2C_Set_Address(device, I2C_MODE_READ);
		
	// 	
  I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_POS;		// 1
		
	// Ожидаем выставления бита ADDR (адрес устройства принят и понят)
	while(!( I2C1->SR1 & I2C_SR1_ADDR )) {}; 
		
	// Читаем безответственно регистры состояния SR1 и SR2
	(void) I2C1->SR1;
	(void) I2C1->SR2;		
		
	I2C1->CR1 &= ~I2C_CR1_ACK;	 // 2
		
	while(!(I2C1->SR1 & I2C_SR1_BTF)){};

  I2C1->CR1 |= I2C_CR1_STOP;

	data_raw_high = I2C1->DR;
	data_raw_low = I2C1->DR;	

	data_raw = ( (data_raw_high <<8 ) | data_raw_low );	
		
	I2C1->CR1 &= ~( I2C_CR1_ACK | I2C_CR1_POS );	

	return data_raw;
}
//

// Чтение 32битного (Double Word) слова из EEPROM на шине I2C
uint32_t EEPROM_read_DWord(uint8_t device, uint8_t cell)
{
uint16_t LW = 0, HW = 0;
uint32_t raw_out = 0;
	
HW = EEPROM_read_word(device, cell);
//I2C_delay(3000);
	
LW = EEPROM_read_word(device, cell + 2);
//I2C_delay(7500);
	
raw_out = (uint32_t)( (HW << 16) | LW );	
	
return raw_out;
}
//

/*

//
// Прерывание по событиям I2C
void I2C1_EV_IRQHandler(void)
{ 

	// Событие EV5
	// ( 0 - записывам данные в устройство, 1 - читаем данные из устройства )
	if( I2C1->SR1 & I2C_SR1_SB ) 
	{ 
		(void) I2C1->SR1;
		I2C1->DR = I2C_Set_Address(EEPROM_I2C_24Cxx, I2C_action); 
		if(I2C_action) { I2C_read_step = 0; } else { I2C_write_step = 0; }
	}
	
	// Событие EV6
	if( I2C1->SR1 & I2C_SR1_ADDR ) 
	{ 
		(void) I2C1->SR1;
		(void) I2C1->SR2;
	}
	
	// Событие EV8
  if( I2C1->SR1 & I2C_SR1_TXE) 
	{
   if( I2C_write_step < 2 )
		{
			I2C1->DR = I2C_write_packet[I2C_write_step++]; 
		}
		else 
		{ 
			I2C1->CR1 |= I2C_CR1_STOP;
		}
	}
	
	// Событие EV8 - выставился флаг данных для чтения => 
	if( I2C1->SR1 & I2C_SR1_RXNE ) 
	{
		if( I2C_read_step == 2 ) 
		{
			//I2C1->CR1 &= ~I2C_CR1_ACK;
		}
		else if(I2C_read_step == 1) 
		{
			I2C1->CR1 &= ~I2C_CR1_ACK;
			I2C1->CR1 |= I2C_CR1_STOP;
		}
		
		if( I2C_read_step < 2 ) 
		{
			I2C_read_packet[I2C_read_step++] = I2C1->DR;
    }
	}	

}
//

// Прерывание по ошибкам I2C
void I2C1_ER_IRQHandler(void)
{
  // Очищаем STOP флаг - необходимо для дальнейшей работы шины
	if (I2C1->SR1 & I2C_SR1_STOPF) 
	{ 
		I2C1->CR1 &= ~I2C_CR1_STOP; 
	}	
	
	// Очищаем NACK флаг - необходимо для дальнейшей работы шины
	if (I2C1->SR1 & I2C_SR1_AF) 
	{ 
		I2C1->SR1 &= ~I2C_SR1_AF;
	}

	// Если есть ошибки на шине - очищаем флаги ошибок
	if (I2C1->SR1 & (I2C_SR1_ARLO | I2C_SR1_BERR))
	{
		I2C1->SR1 &= ~(I2C_SR1_ARLO | I2C_SR1_BERR);
	}
}
//

*/

////////


