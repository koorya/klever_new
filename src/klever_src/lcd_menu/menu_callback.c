#include "screen.h"
#include "../MEM.h"
#include "../main_sm.h"
#include "../modbus.h"
#include "../tension_math.h"


extern volatile float N;
extern volatile float T;
extern volatile uint32_t rope_tension_up_limit;
extern volatile uint32_t rope_tension_bottom_limit;
extern volatile uint32_t rope_tention_target;

extern volatile uint32_t previous_rope_tention_target;
extern volatile uint16_t previous_PWM_Speed;
extern volatile uint32_t previous_rope_tension_up_limit;
extern volatile uint32_t previous_rope_tension_bottom_limit;

extern volatile uint16_t PWM_Speed_Backup;
extern volatile uint8_t PWM_Speed_state;
extern volatile uint16_t K_PWM_tension;

extern volatile uint32_t PNP_count;
extern volatile uint32_t motor_foult_counter;
extern volatile uint8_t PWM_Speed_state;

extern void enableRotation(void);
extern void disableRotation(void);
extern void sendNextByte(void);

extern void BDCR_Lock(void);
extern void BDCR_UnLock(void);
extern void BDCR_Write_Word(uint16_t BKP_DR, uint16_t raw);

extern volatile uint8_t I2C_update;

extern const char *Global_Caption[];
extern volatile uint8_t Now_Alarm;

extern volatile uint8_t onthefly_log_mode;
extern volatile uint8_t LOG_PROCESS_ON_FLAG;
extern volatile const char * usart_buff;

extern volatile uint8_t soft_error;

extern volatile uint8_t PWM_Tension_state;
extern volatile uint16_t PWM_tension;
extern void FC_tension_Down(void);
extern void FC_tension_Up(void);
extern void FC_tension_Hold(void);




void show_yes_no(char * ret, char param){
	if(param)
		sprintf(ret, "��");
	else
		sprintf(ret, "���");
}

void show_status(char * ret){

	if(!Now_Alarm){
		if(!PWM_Speed_state)
			sprintf(ret, "��������");
		else
			sprintf(ret, "������");
	}else{
		sprintf(ret, "!!����!!");
	}
}

void show_alarm(char * ret){
	ret[0] = 0;
	if(Now_Alarm)
		sprintf(ret, "%s", Global_Caption[Now_Alarm]);
}

void show_resurs(char * ret){
	sprintf(ret, "%i", (int)PNP_count);
}

extern volatile float N;
void show_tenzo(char * ret){
	sprintf(ret, "%i", (int)N);
}

void show_log_state(char * ret){
	sprintf(ret, "%1d", onthefly_log_mode);
}
void show_interface_error_state(char * ret){
	static uint8_t cnt = 0;
	if(soft_error){
		if(cnt < 99) cnt++;
		sprintf(ret, "%2d*", cnt);
	}else{
		sprintf(ret, "%2d ", cnt);
	}
}

uint16_t isSaved(){
	if (
			!compareTensionMathParam(&optical_sensor_math_param, &memory_optical_sensor_math_param)||
			(previous_rope_tention_target 		!= rope_tention_target) 	||
			(previous_PWM_Speed 				!= PWM_Speed_Backup) 		||
			(previous_rope_tension_up_limit 	!= rope_tension_up_limit)	||
			(previous_rope_tension_bottom_limit != rope_tension_bottom_limit)	)

		return 1;
	else
		return 0;
}
void show_save_state(char * ret){
	if (isSaved())
	{
		sprintf(ret, "0");
	}else{
		sprintf(ret, "1");
	}
}

extern volatile float tenzo_zero_value;

void save_tenzo_zero(void){
	BDCR_UnLock();
	uint16_t tenzo_zero_value_uint8 = 	(uint16_t)(tenzo_zero_value*100.0);
	BDCR_Write_Word(BKP_DR10, tenzo_zero_value_uint8);
	BDCR_Lock();
}
void inc_tenzo_zero(void){
	tenzo_zero_value += 0.01;
	if(tenzo_zero_value >= 10.0)
		tenzo_zero_value -= 0.01;
	save_tenzo_zero();
}
void dec_tenzo_zero(void){
	if(tenzo_zero_value>0)
		tenzo_zero_value -= 0.01;
	save_tenzo_zero();
}
void show_tenzo_zero(char * ret){
	sprintf(ret, "%1d.%02d", ((int)(tenzo_zero_value*100))/100, ((int)(tenzo_zero_value*100))%100);
//	sprintf(ret, "%2d.%1d", angle_right/10, angle_right%10);
}

void show_optical_sensor_voltage(char * ret){
	sprintf(ret, "%2d.%02d", (int)(optical_sensor_data.optical_sensor_voltage*1000/4095)/100, (int)(optical_sensor_data.optical_sensor_voltage*1000/4095)%100);
}

void show_cur_sensor(char * ret){
//	float N = 2655.345;//test
	int whole = ((int)(N*100))/100;
	if (whole<100)
		sprintf(ret, "%2d.%02d", whole, ((int)(N*100))%100);
	else if(whole < 1000)
		sprintf(ret, "%3d.%01d", whole, ((int)(N*10))%10);
	else
		sprintf(ret, "ovld");
}
void show_cur_tension(char * ret){
//	sprintf(ret, "[%.02f]", N);
	int div = (((int)(T*100))/100);
	if (div / 1000)
		sprintf(ret, "ovrld");
	else
		sprintf(ret, "%3d.%1d", div, ((int)(T*10))%10);
//	sprintf(ret, "%3d.%1d", ((int)(N*100))/100, ((int)(N*10))%10);
}
void show_mode(char * ret){
	if(CURRENT_REG_MODE == PID_REG){
		sprintf(ret, "[���]");
	}else if(CURRENT_REG_MODE == NO_REG){
		sprintf(ret, "[OFF]");
	}else if(CURRENT_REG_MODE == PULS_REG){
		sprintf(ret, "[���]");
	}else if(CURRENT_REG_MODE == LAZY_PID_REG){
		sprintf(ret, "[���]");
	}
}


void switch_log_state(void){
	if(!onthefly_log_mode){
		onthefly_log_mode = 1; //�������� ������������ ������
		usart_buff = "N\n";//������� ����� ������� ��� ���
		LOG_PROCESS_ON_FLAG = 1;
		sendNextByte();
	}else{
		onthefly_log_mode = 0; //�������� ������������ ������
	}
	
}
void show_log_state_yes_no(char * ret){
	show_yes_no(ret, onthefly_log_mode);
}

void switch_save_state(void){
	previous_rope_tention_target = rope_tention_target;
	previous_PWM_Speed = PWM_Speed_Backup;
	previous_rope_tension_up_limit = rope_tension_up_limit;
	previous_rope_tension_bottom_limit = rope_tension_bottom_limit;
	
	memory_optical_sensor_math_param.L2 = optical_sensor_math_param.L2;

	BDCR_UnLock();
	BDCR_Write_Word(BKP_DR4, previous_rope_tention_target);
	BDCR_Write_Word(BKP_DR5, previous_PWM_Speed);
	BDCR_Write_Word(BKP_DR6, previous_rope_tension_up_limit);		
	BDCR_Write_Word(BKP_DR7, previous_rope_tension_bottom_limit);		
	BDCR_Write_Word(BKP_DR8, memory_optical_sensor_math_param.L2);
//	BDCR_Write_Word(BKP_DR9, previous_angle_right);
	//����� ������ �������� ��������, �.�. ��� ������������ �� ����� �������. �������� � ����� �������� � ����������� ���� ��� ������ ���� ������������
	BDCR_Lock();
}


void show_save_state_yes_no(char * ret){
	if (isSaved())
	{
		sprintf(ret, "���");
	}else{
		sprintf(ret, "��");
	}
}	

void switch_default_state(void){
	PNP_count = 0;

	previous_rope_tention_target = rope_tention_target = DEF_TENSION_LIMIT; //14000
	previous_PWM_Speed = PWM_Speed_Backup = DEF_PWM_SPEED; //50 
	previous_rope_tension_up_limit = rope_tension_up_limit = DEF_MAX_TENSION_VALUE;//150;
	
	I2C_update = 1;
	
	BDCR_UnLock();
	BDCR_Write_Word(BKP_DR4, previous_rope_tention_target);
	BDCR_Write_Word(BKP_DR5, previous_PWM_Speed);
	BDCR_Write_Word(BKP_DR6, previous_rope_tension_up_limit);

	BDCR_Lock();
}
void show_default_state(char * ret){
	if ((rope_tention_target 	!= DEF_TENSION_LIMIT) 	||
			(PWM_Speed_Backup 						!= DEF_PWM_SPEED) 			||
			(rope_tension_up_limit != DEF_MAX_TENSION_VALUE)		)
	{
		sprintf(ret, "���");
	}else{
		sprintf(ret, "��");
	}
}

void switch_mode_not_reg_state(void){
	CURRENT_REG_MODE = NO_REG;
	PWM_tension = 0;
	FC_tension_Hold();
}
void show_mode_not_reg_state(char * ret){
	if(CURRENT_REG_MODE == NO_REG){
		sprintf(ret, "��");
	}else{
		sprintf(ret, "���");
	}
}
/*void switch_mode_lazy_pid_state(void){
		if(CURRENT_REG_MODE == LAZY_PID_REG){
			switch_mode_not_reg_state();
		}else{
			CURRENT_REG_MODE = LAZY_PID_REG;
			K_PWM_tension = 1;
		}
}
void show_mode_lazy_pid_state(char * ret){
	if(CURRENT_REG_MODE == LAZY_PID_REG){
		sprintf(ret, "��");
	}else{
		sprintf(ret, "���");
	}
}
void inc_mode_lazy_pid_period(void){
		mode_lazy_pid_struct.mode_lazy_pid_period.current++;
}
void dec_mode_lazy_pid_period(void){
		mode_lazy_pid_struct.mode_lazy_pid_period.current--;
}
void show_mode_lazy_pid_period(char * ret){
	sprintf(ret, "%3d", mode_lazy_pid_struct.mode_lazy_pid_period.current);
}
void inc_mode_lazy_pid_value(void){
		mode_lazy_pid_struct.mode_lazy_pid_value.current++;
}
void dec_mode_lazy_pid_value(void){
		mode_lazy_pid_struct.mode_lazy_pid_value.current--;
}
void show_mode_lazy_pid_value(char * ret){
	sprintf(ret, "%3d", mode_lazy_pid_struct.mode_lazy_pid_value.current);
}
*/
void switch_mode_lazy_pid_state(void){
		if(CURRENT_REG_MODE == LAZY_PID_REG){
			switch_mode_not_reg_state();
		}else{
			CURRENT_REG_MODE = LAZY_PID_REG;
			K_PWM_tension = 1;
		}
}
void show_mode_lazy_pid_state(char * ret){
	if(CURRENT_REG_MODE == LAZY_PID_REG){
		sprintf(ret, "��");
	}else{
		sprintf(ret, "���");
	}
}
void validate_mode_lazy_pid_period(void){
	if(mode_lazy_pid_struct.mode_lazy_pid_period.current > 100) {
		mode_lazy_pid_struct.mode_lazy_pid_period.current = 100;
	}
	if(mode_lazy_pid_struct.mode_lazy_pid_period.current < 2) {
		mode_lazy_pid_struct.mode_lazy_pid_period.current = 2;
	}
}
void inc_mode_lazy_pid_period(void){
	mode_lazy_pid_struct.mode_lazy_pid_period.current++;
	validate_mode_lazy_pid_period();
}
void dec_mode_lazy_pid_period(void){
	mode_lazy_pid_struct.mode_lazy_pid_period.current--;
	validate_mode_lazy_pid_period();
}
void show_mode_lazy_pid_period(char * ret){
	sprintf(ret, "%3d", (int)(mode_lazy_pid_struct.mode_lazy_pid_period.current));
}
void validate_mode_lazy_pid_value(void){
	if(mode_lazy_pid_struct.mode_lazy_pid_value.current >rope_tension_up_limit*100) {
		mode_lazy_pid_struct.mode_lazy_pid_value.current = rope_tension_up_limit*100;
	}
	if(mode_lazy_pid_struct.mode_lazy_pid_value.current < rope_tension_bottom_limit*100) {
		mode_lazy_pid_struct.mode_lazy_pid_value.current = rope_tension_bottom_limit*100;
	}
}
void inc_mode_lazy_pid_value(void){
	mode_lazy_pid_struct.mode_lazy_pid_value.current += 100;
	validate_mode_lazy_pid_value();
}
void dec_mode_lazy_pid_value(void){
	mode_lazy_pid_struct.mode_lazy_pid_value.current -= 100;
	validate_mode_lazy_pid_value();
}
void show_mode_lazy_pid_value(char * ret){
	sprintf(ret, "%3d", (int)(mode_lazy_pid_struct.mode_lazy_pid_value.current/100));
}

void validate_mode_lazy_pid_err(void){
	if(mode_lazy_pid_struct.mode_lazy_pid_err.current > 20) {
		mode_lazy_pid_struct.mode_lazy_pid_err.current = 20;
	}
	if(mode_lazy_pid_struct.mode_lazy_pid_err.current < 1) {
		mode_lazy_pid_struct.mode_lazy_pid_err.current = 1;
	}
}
void inc_mode_lazy_pid_err(void){
	mode_lazy_pid_struct.mode_lazy_pid_err.current += 1;
	validate_mode_lazy_pid_err();
}
void dec_mode_lazy_pid_err(void){
	mode_lazy_pid_struct.mode_lazy_pid_err.current -= 1;
	validate_mode_lazy_pid_err();
}
void show_mode_lazy_pid_err(char * ret){
	sprintf(ret, "%3d", (int)(mode_lazy_pid_struct.mode_lazy_pid_err.current));
}

void switch_mode_pid_state(void){
		if(CURRENT_REG_MODE == PID_REG){
			switch_mode_not_reg_state();
		}else{
			CURRENT_REG_MODE = PID_REG;
			K_PWM_tension = 1;
		}
}
void show_mode_pid_state(char * ret){
	if(CURRENT_REG_MODE == PID_REG){
		sprintf(ret, "��");
	}else{
		sprintf(ret, "���");
	}
}
void inc_mode_pid_value(void){
	rope_tention_target += 100; 
	if(rope_tention_target >rope_tension_up_limit*100) { rope_tention_target = rope_tension_up_limit*100; } 
}
void dec_mode_pid_value(void){
	if( rope_tention_target > 100 ) rope_tention_target -= 100; 
	if(rope_tention_target < rope_tension_bottom_limit*100) { rope_tention_target = rope_tension_bottom_limit*100; }
}
void show_mode_pid_value(char * ret){
	sprintf(ret, "%3d", (int)(rope_tention_target/100));
}



void switch_mode_puls_state(void){
		if(CURRENT_REG_MODE == PULS_REG){
			switch_mode_not_reg_state();
		}else{
			CURRENT_REG_MODE = PULS_REG;
			K_PWM_tension = 10;
		}
}
void show_mode_puls_state(char * ret){
	if(CURRENT_REG_MODE == PULS_REG){
		sprintf(ret, "��");
	}else{
		sprintf(ret, "���");
	}
}

void inc_mode_puls_from_value(void){
	mode_puls_struct.mode_puls_from_value.current++;
	if(mode_puls_struct.mode_puls_from_value.current >= mode_puls_struct.mode_puls_to_value.current) 
		mode_puls_struct.mode_puls_from_value.current = mode_puls_struct.mode_puls_to_value.current - 1;	
}
void dec_mode_puls_from_value(void){
	mode_puls_struct.mode_puls_from_value.current--;
	if(mode_puls_struct.mode_puls_from_value.current <= rope_tension_bottom_limit) 
		mode_puls_struct.mode_puls_from_value.current = rope_tension_bottom_limit;	
}
void show_mode_puls_from_value(char * ret){
	sprintf(ret, "%3d", (int)(mode_puls_struct.mode_puls_from_value.current));
}
void inc_mode_puls_to_value(void){
	mode_puls_struct.mode_puls_to_value.current++;
	if(mode_puls_struct.mode_puls_to_value.current >= rope_tension_up_limit) 
		mode_puls_struct.mode_puls_to_value.current = rope_tension_up_limit;	
}
void dec_mode_puls_to_value(void){
	mode_puls_struct.mode_puls_to_value.current--;
	if(mode_puls_struct.mode_puls_to_value.current <= mode_puls_struct.mode_puls_from_value.current) 
		mode_puls_struct.mode_puls_to_value.current = mode_puls_struct.mode_puls_from_value.current+1;	
}
void show_mode_puls_to_value(char * ret){
	sprintf(ret, "%3d", (int)(mode_puls_struct.mode_puls_to_value.current));
}
void inc_mode_puls_period_value(void){
	mode_puls_struct.mode_puls_period_value.current++;
	if (mode_puls_struct.mode_puls_period_value.current > 300)
		mode_puls_struct.mode_puls_period_value.current = 300;
}
void dec_mode_puls_period_value(void){
	mode_puls_struct.mode_puls_period_value.current--;
	if (mode_puls_struct.mode_puls_period_value.current<=1)
		mode_puls_struct.mode_puls_period_value.current = 1;
}
void show_mode_puls_period_value(char * ret){
	sprintf(ret, "%3d�", (int)(mode_puls_struct.mode_puls_period_value.current));
}

void inc_freq(void){
	PWM_Speed_Backup += 1; 
	if(PWM_Speed_Backup>MAX_ROTATION_SPEED) { PWM_Speed_Backup = MAX_ROTATION_SPEED; }
	setFreqAtv12(PWM_Speed_Backup);
	if(PWM_Speed_state == 1) enableRotation();//���� ��������, �� ������� ��� ������ �������� ��������
}
void dec_freq(void){
	PWM_Speed_Backup -= 1;  if(PWM_Speed_Backup<1) { PWM_Speed_Backup = 1; }
	setFreqAtv12(PWM_Speed_Backup);
	if(PWM_Speed_state == 1) enableRotation();//���� ��������, �� ������� ��� ������ �������� ��������
}
void show_freq(char * ret){
	sprintf(ret, "%2d.%1d", PWM_Speed_Backup/10, PWM_Speed_Backup%10);
}



void inc_rope_tension_bottom_limit(void){
	rope_tension_bottom_limit+=1;
	rope_tension_bottom_limit = rope_tension_bottom_limit>rope_tension_up_limit?rope_tension_up_limit:rope_tension_bottom_limit;
} 
void dec_rope_tension_bottom_limit(void){
	if(rope_tension_bottom_limit > 1 ) rope_tension_bottom_limit-=1;
	rope_tension_bottom_limit = rope_tension_bottom_limit<5?5:rope_tension_bottom_limit;
}

void show_rope_tension_bottom_limit(char * ret){
	sprintf(ret, "%3d", (int)(rope_tension_bottom_limit/1));
}	

void inc_rope_tension_up_limit(void){
	rope_tension_up_limit+=1;
	rope_tension_up_limit = rope_tension_up_limit>300?300:rope_tension_up_limit;
} 
void dec_rope_tension_up_limit(void){
	if(rope_tension_up_limit>1) rope_tension_up_limit-=1;
	rope_tension_up_limit = rope_tension_up_limit<rope_tension_bottom_limit ? rope_tension_bottom_limit : rope_tension_up_limit;
}

void show_rope_tension_up_limit(char * ret){
	sprintf(ret, "%3d", (int)(rope_tension_up_limit/1));
}	
void show_up_limit_brackets(char * ret){
	sprintf(ret, "[%3d]", (int)(rope_tension_up_limit/1));
}

void inc_tension_math_L2(void){

}
void dec_tension_math_L2(void){

}
void show_tension_math_L2(char * ret){

}

void inc_tension_math_R(void){

}
void dec_tension_math_R(void){

}
void show_tension_math_R(char * ret){

}

void inc_tension_math_N_avg(void){

}
void dec_tension_math_N_avg(void){

}
void show__tension_math_N_avg(char * ret){

}

void show_tension_math_L1(char * ret){

}


void inc_tension_math_A(void){

}
void dec_tension_math_A(void){

}
void show_float(char * ret, float val){
	/*
	char str[100];

	char *tmpSign = (val < 0) ? "-" : "";
	float tmpVal = (val < 0) ? -val : val;

	int tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = trunc(tmpFrac * 10000);  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.

	sprintf (str, "adc_read = %s%d.%04d\n", tmpSign, tmpInt1, tmpInt2);
	*/
}

void show_tension_math_A(char * ret){
//	optical_sensor_math_param.A;
//	dtostrf();




	sprintf(ret, "%f", 0.43f);
}

void inc_tension_math_B(void){

}
void dec_tension_math_B(void){

}
void show_tension_math_B(char * ret){

}

void inc_tension_math_C(void){

}
void dec_tension_math_C(void){

}
void show_tension_math_C(char * ret){

}

void inc_tension_math_D(void){

}
void dec_tension_math_D(void){

}
void show_tension_math_D(char * ret){

}

void inc_tension_math_E(void){

}
void dec_tension_math_E(void){

}
void show_tension_math_E(char * ret){

}

void inc_tension_math_F(void){

}
void dec_tension_math_F(void){

}
void show_tension_math_F(char * ret){

}


/*   <----KEY CALLBACKS---->   */

void blueButtonClickHandler(void){
	screenSM(SCR_COMM_up);
}

void yellowButtonClickHandler(void){
	screenSM(SCR_COMM_down);
}



void redButtonClickHandler(void){
	if(Now_Alarm){
		Now_Alarm = 0;
		return;
	}
	if((!PWM_Speed_state)&&(!PWM_Tension_state)) {
		enableRotation();
		motor_foult_counter = MOTOR_FOULT_COUNTER_START_VALUE_START;
		runAtv12();
	}else{
		disableRotation();
		stopAtv12();
	}
}
void greenButtonClickHandler(void){
	screenSM(SCR_COMM_select);
}

void blueButtonPressHandler(void){
	screenSM(SCR_COMM_up);
}

void yellowButtonPressHandler(void){
	screenSM(SCR_COMM_down);
}

void redButtonPressHandler(void){
	redButtonClickHandler();
}

void greenButtonPressHandler(void){
	screenSM(SCR_COMM_select);
}


void yellowBlueButtonClickHandler(void){
	if(!PWM_Tension_state) { 
		switch_mode_not_reg_state();
		PWM_tension = 4*200/K_PWM_tension; 
		PWM_Tension_state = 1; 
		FC_tension_Down(); 
	} else {
		PWM_tension = 0;  
		PWM_Tension_state = 0; 
		FC_tension_Hold(); 
	}
}

void redYellowButtonClickHandler(void){
	if(!PWM_Tension_state) { 
		if (Now_Alarm!=8 && Now_Alarm!=9){
			switch_mode_not_reg_state();
			PWM_tension = 200/K_PWM_tension; 
			PWM_Tension_state = 1; 
			FC_tension_Up(); 
		} 
	}else{ 
		PWM_tension = 0;
		PWM_Tension_state = 0;
		FC_tension_Hold();
	}
}

void redBlueButtonClickHandler(void){ //������� � ����� ������ ������
	switch_save_state();
}

void threeButtonClickHandler(void){
	switch_default_state();
}

