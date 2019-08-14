#include <stm32f10x.h>
#include "K_RCC.h"

uint32_t volatile K_EXT_OSC_FREQ = K_STURTUP_EXT_OSC_FREQ; //10MHz
uint32_t volatile K_INT_OSC_FREQ = K_STURTUP_INT_OSC_FREQ; //8MHz
K_CLOC_CONTROL volatile K_CLOCK_CONTROL_STRUCT;

void K_updatePeriphFreq(void){

	K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ = K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ;

	if(!(RCC->CFGR & RCC_CFGR_HPRE_3)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 1;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV512)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 512;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV256)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 256;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV128)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 128;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV64)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 64;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV16)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 16;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV8)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 8;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV4)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 4;
	}else if(!(RCC->CFGR & RCC_CFGR_HPRE_DIV2)){
		K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ /= 2;
	}
	
	K_CLOCK_CONTROL_STRUCT.K_APB1_FREQ = K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ;
	
	if(!(RCC->CFGR & RCC_CFGR_PPRE1_2)){
		K_CLOCK_CONTROL_STRUCT.K_APB1_FREQ /= 1;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE1_DIV16)){
		K_CLOCK_CONTROL_STRUCT.K_APB1_FREQ /= 16;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE1_DIV8)){
		K_CLOCK_CONTROL_STRUCT.K_APB1_FREQ /= 8;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE1_DIV4)){
		K_CLOCK_CONTROL_STRUCT.K_APB1_FREQ /= 4;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE1_DIV2)){
		K_CLOCK_CONTROL_STRUCT.K_APB1_FREQ /= 2;
	}
	
	K_CLOCK_CONTROL_STRUCT.K_APB2_FREQ = K_CLOCK_CONTROL_STRUCT.K_AHB_FREQ;
	
	if(!(RCC->CFGR & RCC_CFGR_PPRE2_2)){
		K_CLOCK_CONTROL_STRUCT.K_APB2_FREQ /= 1;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE2_DIV16)){
		K_CLOCK_CONTROL_STRUCT.K_APB2_FREQ /= 16;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE2_DIV8)){
		K_CLOCK_CONTROL_STRUCT.K_APB2_FREQ /= 8;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE2_DIV4)){
		K_CLOCK_CONTROL_STRUCT.K_APB2_FREQ /= 4;
	}else if(!(RCC->CFGR & RCC_CFGR_PPRE2_DIV2)){
		K_CLOCK_CONTROL_STRUCT.K_APB2_FREQ /= 2;
	}


	
}



void K_SetExtClockPLL(char mult,	//mult - 4, 5, 6, 7, 8, 9, 65 as 6.5 
	char prediv1, // 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
	char prediv2, // 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
	char mult2)										//8, 9, 10, 11, 12, 13, 14, (not 15) 16, 20; if mult2 == 1 (not used) --> prediv2=1
{ 
	
	K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ = K_EXT_OSC_FREQ;
	
	RCC->CR |= RCC_CR_HSEON; //включаем внешний генератор
	while (!(RCC->CR & RCC_CR_HSERDY));// ждем, пока внешний генератор запустится и будет готов
	
	RCC->CR |= RCC_CR_HSION; //включаем внутренний генератор
	while (!(RCC->CR & RCC_CR_HSIRDY));// ждем, пока внутренний генератор запустится и будет готов

	RCC->CFGR &= ~RCC_CFGR_SW;//обнуляем регистор выбора источника тактирования (выставляется внутренний напрямую), чтобы можно было конфигурировать внешний и умножитель
	
	RCC->CR &= ~(RCC_CR_PLLON | RCC_CR_PLL2ON | RCC_CR_PLL3ON);//выключаем все три умножителя для того, чтобы можно было сконфигурировать
	
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;//обнуляем множитель
	if(mult == 4){
		RCC->CFGR |= RCC_CFGR_PLLMULL4;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 4;
	}else if(mult == 5){
		RCC->CFGR |= RCC_CFGR_PLLMULL5;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 5;
	}else if(mult == 6){
		RCC->CFGR |= RCC_CFGR_PLLMULL6;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 6;
	}else if(mult == 7){
		RCC->CFGR |= RCC_CFGR_PLLMULL7;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 7;
	}else if(mult == 8){
		RCC->CFGR |= RCC_CFGR_PLLMULL8;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 8;
	}else if(mult == 9){
		RCC->CFGR |= RCC_CFGR_PLLMULL9;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 9;
	}else if(mult == 65){
		RCC->CFGR |= RCC_CFGR_PLLMULL6_5;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 65;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 10;
	}
	


	
	RCC->CFGR &= ~RCC_CFGR_PLLSRC;
	RCC->CFGR |= RCC_CFGR_PLLSRC_PREDIV1; //переключаем вход умножителя на предделенный внешний

	RCC->CFGR2 &= ~(RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV2);//выключаем предделители prediv1, prediv2


	RCC->CFGR2 &= ~RCC_CFGR2_PREDIV1SRC; //сбрасываем выбор prediv1src
	if(mult2 == 1){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1SRC_HSE; //если  mult2 == 1, т.е. без умноженеия, то включаем вход предделителя напрямую
	}else{
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1SRC_PLL2; //включаем вход предделителя к выходу второго умножителя
		
		RCC->CFGR2 &= ~RCC_CFGR2_PLL2MUL; //сбрасываем умножитель pll2
		if(mult2 == 8){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL8;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 8;
		}else if(mult2 == 9){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL9;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 9;
		}else if(mult2 == 10){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL10;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 10;
		}else if(mult2 == 11){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL11;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 11;
		}else if(mult2 == 12){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL12;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 12;
		}else if(mult2 == 13){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL13;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 13;
		}else if(mult2 == 14){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL14;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 14;
		}else if(mult2 == 16){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL16;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 16;
		}else if(mult2 == 20){
			RCC->CFGR2 |= RCC_CFGR2_PLL2MUL20;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ *= 20;
		}	
		
		if (prediv2 == 1){ //выставляем предделитель 2
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV1;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 1;
		}else if (prediv2 == 2){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV2;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 2;
		}else if (prediv2 == 3){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV3;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 3;
		}else if (prediv2 == 4){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV4;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 4;
		}else if (prediv2 == 5){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV5;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 5;
		}else if (prediv2 == 6){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV6;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 6;
		}else if (prediv2 == 7){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV7;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 7;
		}else if (prediv2 == 8){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV8;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 8;
		}else if (prediv2 == 9){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV9;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 9;
		}else if (prediv2 == 10){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV10;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 10;
		}else if (prediv2 == 11){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV11;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 11;
		}else if (prediv2 == 12){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV12;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 12;
		}else if (prediv2 == 13){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV13;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 13;
		}else if (prediv2 == 14){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV14;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 14;
		}else if (prediv2 == 15){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV15;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 15;
		}else if (prediv2 == 16){
			RCC->CFGR2 |= RCC_CFGR2_PREDIV2_DIV16;
			K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 16;
		}
		
		

	}
	
		if (prediv1 == 1){ 
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV1;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 1;
	}else if (prediv1 == 2){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV2;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 2;
	}else if (prediv1 == 3){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV3;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 3;
	}else if (prediv1 == 4){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV4;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 4;
	}else if (prediv1 == 5){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV5; 
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 5;
	}else if (prediv1 == 6){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV6;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 6;
	}else if (prediv1 == 7){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV7;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 7;
	}else if (prediv1 == 8){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV8;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 8;
	}else if (prediv1 == 9){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV9;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 9;
	}else if (prediv1 == 10){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV10;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 10;
	}else if (prediv1 == 11){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV11; 
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 11;
	}else if (prediv1 == 12){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV12;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 12;
	}else if (prediv1 == 13){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV13;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 13;
	}else if (prediv1 == 14){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV14;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 14;
	}else if (prediv1 == 15){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV15;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 15;
	}else if (prediv1 == 16){
		RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV16;
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ /= 16;
	}else{
		K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ = 0;
	}


	RCC->CR |= RCC_CR_PLL2ON;//включаем умнжитель pll2
	while(!(RCC->CR & RCC_CR_PLL2RDY));//ждем пока он будет готов

	
	RCC->CR |= RCC_CR_PLLON;//включаем умнжитель pll
	while(!(RCC->CR & RCC_CR_PLLRDY));//ждем пока он будет готов

	
	
	RCC->CFGR |= RCC_CFGR_SW_PLL;// переключаем вход источника тактирования на умножитель

	
}


void K_SetExtClockDirect(void)
{ 
	RCC->CR |= RCC_CR_HSEON; //включаем внешний генератор
	while (!(RCC->CR & RCC_CR_HSERDY));// ждем, пока внешний генератор запустится и будет готов
	
	RCC->CFGR &= ~RCC_CFGR_SW;//обнуляем регистор выбора источника тактирования (выставляется внутренний напрямую)
	RCC->CFGR |= RCC_CFGR_SW_HSE;//выставляем внешний напрямую

	K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ = K_EXT_OSC_FREQ;

}


void K_SetIntClockDirect(void){
	RCC->CR |= RCC_CR_HSION; //включаем внутренний генератор
	while (!(RCC->CR & RCC_CR_HSIRDY));// ждем, пока внутренний генератор запустится и будет готов
	
	RCC->CFGR &= ~RCC_CFGR_SW;//обнуляем регистор выбора источника тактирования (выставляется внутренний напрямую)
	RCC->CFGR |= RCC_CFGR_SWS_HSI;//выставляем внутренний напрямую

	K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ = K_INT_OSC_FREQ;

}


void K_SetIntClockPLL(char freq)	//freq - 16, 20, 24, 28, 32, 36, 26 MHz
{ 
	RCC->CR |= RCC_CR_HSION; //включаем внутренний генератор
	while (!(RCC->CR & RCC_CR_HSIRDY));// ждем, пока внутренний генератор запустится и будет готов
	
	RCC->CFGR &= ~RCC_CFGR_SW;//обнуляем регистор выбора источника тактирования (выставляется внутренний напрямую), чтобы можно было конфигурировать внешний и умножитель
	
	RCC->CR &= ~RCC_CR_PLLON;//выключаем умножитель для того, чтобы его можно было сконфигурировать
	
	RCC->CFGR &= ~RCC_CFGR_PLLSRC;
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_Div2; //переключаем вход умножителя предделенный внутренний


	RCC->CFGR &= ~RCC_CFGR_PLLMULL;//обнуляем множитель
	
	if(freq == 16){
		RCC->CFGR |= RCC_CFGR_PLLMULL4;
	}else if(freq == 20){
		RCC->CFGR |= RCC_CFGR_PLLMULL5;
	}else if(freq == 24){
		RCC->CFGR |= RCC_CFGR_PLLMULL6;
	}else if(freq == 28){
		RCC->CFGR |= RCC_CFGR_PLLMULL7;
	}else if(freq == 32){
		RCC->CFGR |= RCC_CFGR_PLLMULL8;
	}else if(freq == 36){
		RCC->CFGR |= RCC_CFGR_PLLMULL9;
	}else if(freq == 26){
		RCC->CFGR |= RCC_CFGR_PLLMULL6_5;
	}
	
	RCC->CR |= RCC_CR_PLLON;//включаем умнжитель
	while(!(RCC->CR & RCC_CR_PLLRDY));//ждем пока он будет готов
	RCC->CFGR |= RCC_CFGR_SW_PLL;// переключаем вход источника тактирования на умножитель
	
	K_CLOCK_CONTROL_STRUCT.K_SYSCLK_FREQ = freq * 1000000;

}




void K_initMCOoutput(K_MCOSource source){

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //включаем тактирование для gpioa, где находится наш MCO (PA8)
	GPIOA->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8); //Обнуляем регистры конфигурации вывода PA8
	GPIOA->CRH |= GPIO_CRH_MODE8 | GPIO_CRH_CNF8_1; //output 50MHz, alternate furction (MCO) push-pull

	RCC->CFGR &= ~RCC_CFGR_MCO;
	if(source == mcoHSI){
		RCC->CFGR |= RCC_CFGR_MCO_HSI;//0b100
	}else if(source == mcoHSE){
		RCC->CFGR |= RCC_CFGR_MCO_HSE;//0b100
	}else if(source == mcoSYS){
		RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;//0b100
	}

}	





void K_setClock(K_SOURCE source, //если HSI, то presc1 = 2, presc2 и PLL2 не используются
	char presc1, //1-16; 
	char presc2, //1-16;
	char kPLL, //1, 4-9, 65 as 6.5 (если kPLL==1, то умножители PLL и PLL2 не используются, делители presc1, presc2 не используются, SW поздключается напрямую)
	char kPLL2, //1, 8-14, 16, 20 (если kPLL2==1, то умножитель PLL2 не используется, делитель presc2 не используется)
	char ahbpresc, // 1, 2, 4, 8, 16, (not 32) 64, 128, 256, 512
	char apb1presc, // 1, 2, 4, 8, 16
	char apb2presc, // 1, 2, 4, 8, 16
	char enable_mco) //0 - disable, !0 - enable (sysclock)
{
	
	if(enable_mco){
		K_initMCOoutput(mcoSYS);
	}
	
	if(kPLL == 1){
		if(source == cHSI)
			K_SetIntClockDirect();
		else
			K_SetExtClockDirect();
	}else	if(source == cHSI){
		if(kPLL == 65) 
			K_SetIntClockPLL(4*6.5);
		else
			K_SetIntClockPLL(4*kPLL);
	}else if(source == cHSE){
		K_SetExtClockPLL(kPLL, presc1, presc2, kPLL2);
	}
	
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	if(ahbpresc == 1){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	}else if(ahbpresc == 2){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV2;
	}else if(ahbpresc == 4){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV4;
	}else if(ahbpresc == 8){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV8;
	}else if(ahbpresc == 16){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV16;
	}else if(ahbpresc == 64){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV64;
	}else if(ahbpresc == 128){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV128;
	}else if(ahbpresc == 256){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV256;
	}else if(ahbpresc == 512){
		RCC->CFGR |= RCC_CFGR_HPRE_DIV512;
	}	

	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	if(apb1presc == 1){
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
	}else if(apb1presc == 2){
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	}else if(apb1presc == 4){
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	}else if(apb1presc == 8){
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV8;
	}else if(apb1presc == 16){
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV16;
	}
	
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	if(apb2presc == 1){
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	}else if(apb2presc == 2){
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	}else if(apb2presc == 4){
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV4;
	}else if(apb2presc == 8){
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV8;
	}else if(apb2presc == 16){
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV16;
	}
	
	
	K_updatePeriphFreq();
	
}

void K_setClockHSE_2param(char freq, char mco_en) //freq - 1, 2, 4, 8, 10, 48, 72 [MHz]
{
	if(freq == 1){
		K_setClock(cHSE, 0, 0, 1, 0, 8, 1, 1, mco_en);
	}else if(freq == 2){
		K_setClock(cHSE, 0, 0, 1, 0, 4, 1, 1, mco_en);
	}else if(freq == 4){
		K_setClock(cHSE, 0, 0, 1, 0, 2, 1, 1, mco_en);
	}else if(freq == 8){
		K_setClock(cHSE, 0, 0, 1, 0, 1, 1, 1, mco_en);
	}else if(freq == 10){
		K_setClock(cHSE, 4, 2, 4, 10, 4, 1, 1, mco_en);
	}else if(freq == 48){
		K_setClock(cHSE, 1, 1, 6, 1, 1, 2, 1, mco_en);
	}else if(freq == 72){
		K_setClock(cHSE, 4, 2, 6, 12, 1, 2, 1, mco_en);
	}		
		
}
void K_setClockHSE(char freq)	//freq - 1, 2, 4, 8, 10, 48, 72 [MHz]
{
	K_setClockHSE_2param(freq, 0);
}
void K_setClockHSE_MCO(char freq) //freq - 1, 2, 4, 8, 10, 48, 72 [MHz]
{
	K_setClockHSE_2param(freq, 1);
}

