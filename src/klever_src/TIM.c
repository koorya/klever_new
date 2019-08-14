#include "stm32f10x.h"
#include "TIM.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ќбласть дл€ объ€влени€ переменных 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// »нициализаци€ системного таймера (прерывание 1мс)
void SysTick_Init(void)
{
	SysTick->VAL = 0;
//	SysTick->LOAD = (SystemCoreClock/6250);//50000); // 6250
// SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	
	SysTick->LOAD = (72000000/1000)-1;
//	SysTick->LOAD = (10000000/1000)-1;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

//
}
//

//




