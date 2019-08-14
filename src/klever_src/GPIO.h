//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  Конфигурация портов ввода/вывода [GPIO] микроконтроллера STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef GPIO_H	// Блокирую повторное включение этого модуля
#define GPIO_H

#define D0_out_OFF GPIOB->BSRR = GPIO_BSRR_BS9;
#define D0_out_ON GPIOB->BRR = GPIO_BRR_BR9;
#define D0_out_Toggle GPIOB->ODR ^= GPIO_ODR_ODR9;

#define D1_out_OFF GPIOB->BSRR = GPIO_BSRR_BS8;
#define D1_out_ON GPIOB->BRR = GPIO_BRR_BR8;
#define D1_out_Toggle GPIOB->ODR ^= GPIO_ODR_ODR8;

#define D2_out_OFF GPIOB->BSRR = GPIO_BSRR_BS12;
#define D2_out_ON GPIOB->BRR = GPIO_BRR_BR12;
#define D2_out_Toggle GPIOB->ODR ^= GPIO_ODR_ODR12;

#define D3_out_OFF GPIOB->BSRR = GPIO_BSRR_BS13;
#define D3_out_ON GPIOB->BRR = GPIO_BRR_BR13;
#define D3_out_Toggle GPIOB->ODR ^= GPIO_ODR_ODR13;

#define Dout_all_ON   GPIOB->BSRR = ( GPIO_BSRR_BS13 | GPIO_BSRR_BS12 | GPIO_BSRR_BS9 | GPIO_BSRR_BS8 );
#define Dout_all_OFF	GPIOB->BRR = ( GPIO_BRR_BR13 | GPIO_BRR_BR12 | GPIO_BRR_BR9 | GPIO_BRR_BR8 );
#define Dout_all_Toggle GPIOB->ODR ^= ( GPIO_ODR_ODR13 | GPIO_ODR_ODR12 | GPIO_ODR_ODR9 | GPIO_ODR_ODR8 );

#define Red_LED_ON GPIOC->BSRR = GPIO_BSRR_BS4;
#define Red_LED_OFF GPIOC->BRR = GPIO_BRR_BR4;
#define Red_LED_Toggle GPIOC->ODR ^= GPIO_ODR_ODR4;

#define Green_LED_ON GPIOC->BSRR = GPIO_BSRR_BS5;
#define Green_LED_OFF GPIOC->BRR = GPIO_BRR_BR5;
#define Green_LED_Toggle GPIOC->ODR ^= GPIO_ODR_ODR5;

#define D0_in_state (uint8_t)((GPIOB->IDR & 0x01)^0x01)
#define D1_in_state (uint8_t)(((GPIOB->IDR>>1) & 0x01)^0x01)
#define D2_in_state (uint8_t)(((GPIOB->IDR>>2) & 0x01)^0x01)
#define D3_in_state (uint8_t)(((GPIOB->IDR>>3) & 0x01)^0x01)
#define D4_in_state (uint8_t)(((GPIOB->IDR>>4) & 0x01)^0x01)
#define D5_in_state (uint8_t)(((GPIOB->IDR>>5) & 0x01)^0x01)
#define All_DIn_state (uint8_t)((GPIOB->IDR & 0x3F)^0x3F)

// ==================================================

// Настройка портов ввода/вывода
void GPIO_Init(void);

// Вращение вперед
void FC_rotate_Fwd(void);

// Вращение назад
void FC_rotate_Rev(void);

// Остановить вращение
void FC_rotate_Stop(void);

// Увеличение натяжения
void FC_tension_Up(void);

// Уменьшение натяжения
void FC_tension_Down(void);

// Блокировка усилия
void FC_tension_Hold(void);

#endif
//
//
