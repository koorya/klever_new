#include "stm32f10x.h"
#include "MEM.h"          // Библиотека для работы с BDCR и Flash-памятью

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Область для объявления переменных 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

void BDCR_Lock(void){
	PWR->CR &= ~PWR_CR_DBP;
}
void BDCR_UnLock(void){
	PWR->CR |= PWR_CR_DBP;
}

// Инициализация Backup Domain Control Registers (BDCR)
void BDCR_Init(void)
{
	//RCC->APB1RSTR &= ~( RCC_APB1RSTR_BKPRST | RCC_APB1RSTR_PWRRST );
	RCC->APB1ENR |= ( RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN );
	//PWR->CR &= ~PWR_CR_DBP;
}

// Отключение Backup Domain Control Registers (BDCR)
void BDCR_Reset(void)
{
	//RCC->APB1ENR &= ~( RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN );
	RCC->APB1RSTR |= ( RCC_APB1RSTR_BKPRST | RCC_APB1RSTR_PWRRST );
	PWR->CR &= ~PWR_CR_DBP;
}
//

// Записать значение в область Backup Domain Control Registers (BDCR)
void BDCR_Write_Word(uint16_t BKP_DR, uint16_t raw)
{
__IO uint32_t tmp = 0;
tmp = (uint32_t)BKP_BASE; 
tmp += BKP_DR;
*(__IO uint32_t *) tmp = raw;
}
//

// Прочитать значение из области Backup Domain Control Registers (BDCR)
uint16_t BDCR_Read_Word(uint16_t BKP_DR)
{
__IO uint32_t tmp = 0;
tmp = (uint32_t)BKP_BASE;
tmp += BKP_DR;
return (*(__IO uint16_t *) tmp);
}
//

//
