#include "log.h"



volatile LogTermStruct log_array[2][LOG_BUFFER_SIZE/2];
volatile uint8_t log_array_ptr;
volatile uint32_t log_array_index;
volatile uint8_t LOG_PROCESS_ON_FLAG = 1;
volatile uint8_t onthefly_log_mode=0;



volatile char line_to_uart_send[USART_BUFF_SIZE] = "Hello uart!";
volatile uint16_t log_buf_send_cnt = LOG_BUFFER_SIZE;
volatile uint16_t curr_send_cnt = LOG_BUFFER_SIZE;

void pushValueToBuffer(LogTermStruct value){
	if(log_array_index>=LOG_BUFFER_SIZE/2){
		log_array_index = 0;
		if(log_array_ptr == 0)
			log_array_ptr =1;
		else
			log_array_ptr = 0;
	}
	log_array[log_array_ptr][log_array_index] = value;
	

	log_array_index++;
}

void initLog(void){
	log_buf_send_cnt = LOG_BUFFER_SIZE+1;
	curr_send_cnt = LOG_BUFFER_SIZE+1;

	log_array_ptr = 0;
	log_array_index = 0;
	LogTermStruct value;
	value.regulation_cmd = 0;
	value.tension = 0;
	value.time_stamp = 0;

	value.cycle_cnt = 0;

	uint16_t i;
	for(i=0; i<LOG_BUFFER_SIZE/2; i++){
			log_array[1][i] = log_array[0][i] = value;
	}
}

LogTermStruct pullValueFromBuffer(void){ 

	if(log_array_index==0){
		log_array_index = LOG_BUFFER_SIZE/2;
		if(log_array_ptr == 0)
			log_array_ptr = 1;
		else
			log_array_ptr = 0;
	}
	log_array_index--;
	return log_array[log_array_ptr][log_array_index];
}
	

LogTermStruct getValueFromBuffer(void){
	if(log_array_index>=LOG_BUFFER_SIZE/2){
		log_array_index = 0;
		if(log_array_ptr == 0)
			log_array_ptr = 1;
		else
			log_array_ptr = 0;
	}

	return log_array[log_array_ptr][log_array_index++];

}


void LogTermToStr(LogTermStruct * term, char * str){

	RTC_Type RTC_DateTime;
	RTC_GetDateTime(term->time_stamp, &RTC_DateTime);


	
	/* %[флаги][ширина][.точность][размер]тип */
	
//	sprintf(str, "%02d.%02d.%04d_%02d.%02d.%02d - time | %lf - tension | %li - cmd || %i - test\n", (uint8_t)0, (uint8_t)0, (uint16_t)0, (uint8_t)0, (uint8_t)0, (uint8_t)0, (float)0, (int32_t)0, (int32_t) 0);
	
//	sprintf(str, "%02d.%02d.%04d_%02d.%02d.%02d - time | %i - tension | %i - cmd || %i - test\n", (int)0, (int)0, (int)0, (int)0, (int)0, (int)0, (int)((float)1.3*100),  (int)0, (int) 0);

	  sprintf(str, "%02d.%02d.%04d_%02d.%02d.%02d - time | %5i - tension | %4i - cmd | %8i - cycles | %3d.%02d V %i\n",
									RTC_DateTime.RTC_date, 
												RTC_DateTime.RTC_month, 
														RTC_DateTime.RTC_year, 
																	RTC_DateTime.RTC_hours, 
																				RTC_DateTime.RTC_minutes, 
																							RTC_DateTime.RTC_seconds, 
																													(int)(term->tension*100), 
																														(int)term->regulation_cmd, (int)term->cycle_cnt,
																															(int)(term->optical_sensor.optical_sensor_voltage*1000/4095)/100, (int)(term->optical_sensor.optical_sensor_voltage*1000/4095)%100,
																																(int)(term->optical_sensor.time_stamp));
//  sprintf(str, "%02d.%02d.%04d_%02d.%02d.%02d - time | %i - ptr | %i - index\n", RTC_DateTime.RTC_date, RTC_DateTime.RTC_month, RTC_DateTime.RTC_year, RTC_DateTime.RTC_hours, RTC_DateTime.RTC_minutes, RTC_DateTime.RTC_seconds, term->ptr, term->index);

}














