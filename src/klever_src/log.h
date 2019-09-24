#include <stdint.h>
#include <stdio.h>
#include "RTC.h"
#include "usart.h"

#define LOG_BUFFER_SIZE 900 //12kb

typedef struct{ //12 bytes + 4 = 16 bytes
	uint32_t time_stamp;
	int32_t regulation_cmd;
	float tension;

	uint32_t cycle_cnt;//13,06,2019 добавил ресурс
	uint32_t optical_sensor_voltage;

} LogTermStruct;

extern volatile LogTermStruct log_array[2][LOG_BUFFER_SIZE/2];

extern volatile uint8_t LOG_PROCESS_ON_FLAG;
extern volatile uint8_t onthefly_log_mode;

extern volatile char line_to_uart_send[USART_BUFF_SIZE];
extern volatile uint16_t log_buf_send_cnt;
extern volatile uint16_t curr_send_cnt;

void initLog(void);
void pushValueToBuffer(LogTermStruct value);
LogTermStruct pullValueFromBuffer(void);
LogTermStruct getValueFromBuffer(void);

void LogTermToStr(LogTermStruct * term, char * str);

