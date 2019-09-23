#include "stm32f10x.h"
#include "RTC.h"

void __nop(void){

}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Область для объявления переменных 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Настройка блока BDCR (backup domain control registers)
void RCC_LSE_Config(uint8_t RCC_LSE)
{
  
  RCC->BDCR = RCC_LSE_OFF;
	__nop();__nop();__nop();
	
  RCC->BDCR = RCC_LSE_OFF;
	__nop();__nop();__nop();
	
  switch(RCC_LSE)
  {
    case RCC_LSE_ON: /* Set LSEON bit */
			{  
				RCC->BDCR = RCC_BDCR_LSEON;
				break;
			}
      
    case RCC_LSE_Bypass: 
			{  
				RCC->BDCR = ( RCC_BDCR_LSEON | RCC_BDCR_LSEBYP ); 
				break;
			}			
      
    default: break;      
  }
	
	__nop();__nop();__nop();
}
//

// Установка делителя частоты для блока RTC
void RTC_SetPrescaler(uint32_t PrescalerValue)
{
  RTC->CRL |= RTC_CRL_CNF;
	
  /* Set RTC PRESCALER MSB word */
  RTC->PRLH = (PrescalerValue & PRLH_MSB_MASK) >> 16;
	
  /* Set RTC PRESCALER LSB word */
  RTC->PRLL = (PrescalerValue & RTC_LSB_MASK);
	
  RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);
}
//

// Инициализация RTC
uint8_t RTC_Init(void)
{
	// Включить тактирование модулей управления питанием и управлением резервной областью
	RCC->APB1ENR |= ( RCC_APB1ENR_BKPEN | RCC_APB1ENR_PWREN );
	
	// Разрешить доступ к области резервных данных
	PWR->CR |= PWR_CR_DBP;
	
	// Если RTC выключен - инициализировать
	if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN)
	{
		// Сброс данных в резервной области
		RCC->BDCR |= RCC_BDCR_BDRST;
		__nop();__nop();__nop();
		RCC->BDCR &= ~RCC_BDCR_BDRST;
 
		// Установить источник тактирования кварц 32768
		RCC_LSE_Config(RCC_LSE_ON);
		
		while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}
		RCC->BDCR |= RCC_RTCCLKSource_LSE;
			
		// Устанавливаем делитель, чтобы часы считали 1/32768 секунд за такт
		RTC_SetPrescaler(0);
 
		// Включаем RTC
		RCC->BDCR |= RCC_BDCR_RTCEN;
 
		// Ждем синхронизацию
		RTC->CRL &= ~RTC_CRL_RSF;
    while ( (RTC->CRL & RTC_CRL_RSF) == (uint16_t)RESET) {}
			
		return 1;	
	}
return 0;	
}
//

// Преобразование структуры Дата/Время в значение для счетчика милисекунд RTC (отсчет от 00:00:00 01.01.1970, т.е. JD0 = 2440588)
uint32_t RTC_GetRTC_Counter(RTC_Type* RTC_DateTimeStruct) 
{
	uint8_t a;
	uint16_t y;
	uint8_t m;
	uint32_t JDN;
 
	a=(14-RTC_DateTimeStruct->RTC_month)/12;
	y=RTC_DateTimeStruct->RTC_year+4800-a;
	m=RTC_DateTimeStruct->RTC_month+(12*a)-3;
 
	JDN=RTC_DateTimeStruct->RTC_date;
	JDN+=(153*m+2)/5;
	JDN+=365*y;
	JDN+=y/4;
	JDN+=-y/100;
	JDN+=y/400;
	JDN = JDN -32045;
	JDN = JDN - JULIAN_DATE_BASE;
	JDN*=86400;
	JDN+=(RTC_DateTimeStruct->RTC_hours*3600);
	JDN+=(RTC_DateTimeStruct->RTC_minutes*60);
	JDN+=(RTC_DateTimeStruct->RTC_seconds);
	
	return JDN*0x8000;
}

//

// Преобразование значения счетчика милисекунд RTC в структуру Дата/Время (отсчет от 00:00:00 01.01.1970, т.е. JD0 = 2440588)
void RTC_GetDateTime(uint32_t RTC_Counter, RTC_Type* RTC_DateTimeStruct) 
{
    unsigned long time;
    unsigned long t1, a, b, c, d, e, m;
    int year = 0;
    int mon = 0;
    int wday = 0;
    int mday = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    int msec = 0;
    uint64_t jd = 0;;
    uint64_t jdn = 0;

    msec = ((RTC_Counter % 0x8000) * 1000) / 0x8000;//берем остаток, который не войдет в секунды, умножаем его на единице времени за такт (1/32768), получаем время в секуднах, затем умножаем его на 1000, получаем милисекунды

    RTC_Counter /= 0x8000;
 
    jd = ((RTC_Counter+43200)/(86400>>1)) + (2440587<<1) + 1;
    jdn = jd>>1;
 
    time = RTC_Counter;
    t1 = time/60;
    sec = time - t1*60;
 
    time = t1;
    t1 = time/60;
    min = time - t1*60;
 
    time = t1;
    t1 = time/24;
    hour = time - t1*24;
 
    wday = jdn%7;
 
    a = jdn + 32044;
    b = (4*a+3)/146097;
    c = a - (146097*b)/4;
    d = (4*c+3)/1461;
    e = c - (1461*d)/4;
    m = (5*e+2)/153;
    mday = e - (153*m+2)/5 + 1;
    mon = m + 3 - 12*(m/10);
    year = 100*b + d - 4800 + (m/10);
 
    RTC_DateTimeStruct->RTC_year = year;
    RTC_DateTimeStruct->RTC_month = mon;
    RTC_DateTimeStruct->RTC_date = mday;
    RTC_DateTimeStruct->RTC_hours = hour;
    RTC_DateTimeStruct->RTC_minutes = min;
    RTC_DateTimeStruct->RTC_seconds = sec;
    RTC_DateTimeStruct->RTC_mseconds = msec;
    RTC_DateTimeStruct->RTC_wday = wday;
}
//

// Запись нового значения счетчика милисекунд RTC
void RTC_SetCounter(uint32_t CounterValue)
{ 
  RTC->CRL |= RTC_CRL_CNF;
  RTC->CNTH = CounterValue >> 16;
  RTC->CNTL = (CounterValue & RTC_LSB_MASK);
  RTC->CRL &= (uint16_t)~((uint16_t)RTC_CRL_CNF);
}
//

// Чтение значения счетчика милисекунд RTC
uint32_t RTC_GetCounter(void)
{
  uint16_t tmp = 0;
  tmp = RTC->CNTL;
  return (((uint32_t)RTC->CNTH << 16 ) | tmp) ;
}
//

//
