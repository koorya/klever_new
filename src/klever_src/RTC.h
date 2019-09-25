//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  ������������ ������������� ����� RTC ���������������� STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef RTC_H	// �������� ��������� ��������� ����� ������
#define RTC_H

#define RCC_LSE_OFF				((uint8_t)0x00)
#define RCC_LSE_ON				((uint8_t)0x01)
#define RCC_LSE_Bypass		((uint8_t)0x04)

#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100)
#define RCC_RTCCLKSource_LSI             ((uint32_t)0x00000200)
#define RCC_RTCCLKSource_HSE_Div128      ((uint32_t)0x00000300)

#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF)  /*!< RTC LSB Mask */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000)  /*!< RTC Prescaler MSB Mask */

// (UnixTime = 00:00:00 01.01.1970 = JD0 = 2440588)
#define JULIAN_DATE_BASE    2440588

// ==================================================

typedef struct
{
    uint8_t RTC_hours;
    uint8_t RTC_minutes;
    uint8_t RTC_seconds;
    uint8_t RTC_date;
    uint8_t RTC_wday;
    uint8_t RTC_month;
    uint16_t RTC_year;
} RTC_Type;

// ==================================================

// ������������� RTC
uint8_t RTC_Init(void);

// ��������� �������� ������� ��� ����� RTC
void RTC_SetPrescaler(uint32_t PrescalerValue);

// ��������� ����� BDCR (backup domain control registers)
void RCC_LSE_Config(uint8_t RCC_LSE);

// �������������� �������� �������� ���������� RTC � ��������� ����/����� (������ �� 00:00:00 01.01.1970, �.�. JD0 = 2440588)
void RTC_GetDateTime(uint32_t RTC_Counter, RTC_Type* RTC_DateTimeStruct);

// �������������� ��������� ����/����� � �������� ��� �������� ���������� RTC (������ �� 00:00:00 01.01.1970, �.�. JD0 = 2440588)
uint32_t RTC_GetRTC_Counter(RTC_Type* RTC_DateTimeStruct);

// ������ ������ �������� �������� ���������� RTC
void RTC_SetCounter(uint32_t CounterValue);

// ������ �������� �������� ���������� RTC
uint32_t RTC_GetCounter(void);

#endif
//
