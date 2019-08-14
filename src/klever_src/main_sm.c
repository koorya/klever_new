#include "main_sm.h"

volatile MAIN_SM_ENUM_TYPE CURRENT_REG_MODE = NO_REG;

volatile PULS_MODE_STRUCT mode_puls_struct;
volatile LAZY_PID_MODE_STRUCT mode_lazy_pid_struct; 

void mainSMInit(void){
	mode_puls_struct.mode_puls_from_value.current = 20;
	mode_puls_struct.mode_puls_to_value.current = 90;
	mode_puls_struct.mode_puls_period_value.current = 20;
	mode_puls_struct.time = 0;
	
	mode_lazy_pid_struct.mode_lazy_pid_value.current = 10400;
	mode_lazy_pid_struct.mode_lazy_pid_period.current = 5;
	mode_lazy_pid_struct.mode_lazy_pid_err.current = 10;
	mode_lazy_pid_struct.time = 0;
	
}


void pulsModeWork(void){ //every 1ms must calls
	if(CURRENT_REG_MODE != PULS_REG){
		return;
	}
	static uint32_t time = 0;
	uint32_t to_v = mode_puls_struct.mode_puls_to_value.current;
	uint32_t from_v = mode_puls_struct.mode_puls_from_value.current;
	uint32_t per_v = mode_puls_struct.mode_puls_period_value.current*1000;
	time++;
	if(time > per_v)
		time = 0;
	if(time <= per_v/2 ){
		rope_tention_target = 100 * (from_v + ( ( to_v - from_v ) * time ) / (per_v/2));
	}else{
		rope_tention_target = 100 * (to_v - ( ( to_v - from_v ) * ( time - (per_v/2) ) ) / (per_v/2));
	}
}


uint8_t isRegulatingLazyPidMode(void){
	int32_t pid_err = rope_tension_error;
	if(pid_err < 0)
		pid_err = -pid_err;
	if(mode_lazy_pid_struct.no_reg) //если еще не вылезли из окна устновки
		return 0;
	else{
		if(pid_err < mode_lazy_pid_struct.mode_lazy_pid_err.current * 100/2){//если попадаем в вдвое меньшее окно
			mode_lazy_pid_struct.no_reg = 1; //выключаем регулирование
		}
		return 1;
	}
}

void lazyPIDModeWork(void){ //every 1ms must calls
	if(CURRENT_REG_MODE != LAZY_PID_REG){
		return;
	}
	rope_tention_target = mode_lazy_pid_struct.mode_lazy_pid_value.current;
	mode_lazy_pid_struct.time++;
	static int32_t avg_err= 0;
	avg_err += rope_tension_error;
	if(avg_err > 0x40000000){ //защита от переполнения
		mode_lazy_pid_struct.no_reg = 0;
		mode_lazy_pid_struct.time = 0;
	}
	if(mode_lazy_pid_struct.time >= mode_lazy_pid_struct.mode_lazy_pid_period.current * 1000){
		mode_lazy_pid_struct.time = 0;
		avg_err /= mode_lazy_pid_struct.mode_lazy_pid_period.current * 1000;
		if(avg_err<0){
			avg_err = -avg_err;
		}
		if(avg_err > mode_lazy_pid_struct.mode_lazy_pid_err.current * 100){//если ошибка выходит за окно
			mode_lazy_pid_struct.no_reg = 0; //включаем регулирование
		}else{
			if(mode_lazy_pid_struct.no_reg == 0){ //если не выходит за окно, но регулирование уже было включено
				if(avg_err < mode_lazy_pid_struct.mode_lazy_pid_err.current * 100/2){//если попадаем в вдвое меньшее окно
					mode_lazy_pid_struct.no_reg = 1; //выключаем регулирование
				}else{
					mode_lazy_pid_struct.no_reg = 0; //оставляем регулирование включенным
				}
			}
		}
		avg_err = 0;
	}
/*	static uint32_t time = 0;
	uint32_t to_v = mode_puls_struct.mode_puls_to_value.current;
	uint32_t from_v = mode_puls_struct.mode_puls_from_value.current;
	uint32_t per_v = mode_puls_struct.mode_puls_period_value.current*1000;
	time++;
	if(time > per_v)
		time = 0;
	if(time <= per_v/2 ){
		rope_tention_target = 100 * (from_v + ( ( to_v - from_v ) * time ) / (per_v/2));
	}else{
		rope_tention_target = 100 * (to_v - ( ( to_v - from_v ) * ( time - (per_v/2) ) ) / (per_v/2));
	}
*/
}


