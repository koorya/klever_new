//#include <stddef.h>

#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "klever_src/TIM.h"
#include "klever_src/PWM.h"
#include "klever_src/GPIO.h"
#include "klever_src/ADC.h"
#include "klever_src/GLCD.h"
#include "klever_src/I2C.h"
#include "klever_src/RTC.h"
#include "klever_src/MEM.h"
#include "klever_src/pid.h"
#include "klever_src/usart.h"
#include "klever_src/log.h"
#include "klever_src/K_RCC/K_RCC.h"
#include "klever_src/lcd_menu/keyboard.h"
#include "klever_src/lcd_menu/screen.h"
#include "klever_src/main_sm.h"
#include "klever_src/modbus.h"
#include "klever_src/tension_math.h"


#define MOTOR_FOULT_COUNTER_START_VALUE 1000 //�������� 5 ������
#define MOTOR_FOULT_COUNTER_START_VALUE_START 4000 //20 ������ ����� �� ������
#define MAX_TENSION_VALUE 1000
#define MIN_TENSION_VALUE 5







//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//   T = (((2698*N)-(20020+(8/10)))/(475+(7/10)));
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ���� ������/������
volatile uint8_t Now_Alarm = 0;
//��� ������ ������ � �������
volatile uint8_t soft_error = 0;
volatile uint8_t display_error = 0;

// ������� ��������� ������� ���
volatile static uint8_t Global_State = 0;

// ����������� �������� ��������� ������� ���
const char *Global_Caption[]={ "�����", "����.��1", "����.��2", "����.��+", "����.��-", "����.���", "����.����", "�.�����", "�.�����", "�.����", "�.����", "�������"};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ����������-���� ������� ������/������ �� �����������
volatile static uint8_t FC_motion_state = 0;
volatile static uint8_t FC_tension_state = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ����������-���� ��� ������ ������� � EEPROM �� ���� I2C
volatile uint8_t I2C_update = 0;

// ����������-������� ��� ������ ������� � EEPROM �� ���� I2C
volatile uint16_t I2C_update_delay = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ����������-������� (�������� ������ �������)
volatile uint32_t sys_delay = 0;

// ����������-������� (�������-�������� ���������������)
volatile uint32_t adc_delay = 0;

// ����������-������� ��� ���������� ��������� �� �������
volatile uint32_t glcd_refresh_delay = 0;

// ����������-������� ��� ��������� ������������
volatile uint32_t leds_delay = 0;

//����������-������� ��� ����������� ��������� ���������, ����������� ������
volatile uint32_t motor_foult_counter = 0;

// ����������-������� ��� �����������
volatile uint32_t log_buff_delay = 0;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������� ������� ���� X
volatile uint8_t  cursor_x = 15;

// ������� ������� ���� Y
volatile uint8_t  cursor_y = 0;

// ������� ������� � ���� / ��������� ��������
volatile uint8_t  menu = 0;

// ����� �������� ���� ������� �� ������
volatile uint8_t  page_menu = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������� ��������� 1 ���� ������
volatile uint8_t bt_state_low = 0;

// ������� ��������� 2 ���� ������
volatile uint8_t bt_state_high = 0;

// ����� ��������� ���� ������
volatile uint16_t bt_state = 0;

// ���������� ����� ��������� ���� ������
volatile uint16_t bt_before_state = 0;

// ���������� ��� ����� ������ ��� ������
volatile uint8_t bt_toggle = 0;

// ����������-����� (���������� �������� ���������)
volatile uint16_t bt_count = 0;

// ������ ��� ������������� � ������
volatile uint16_t bt_delay = 200;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������� ��������� ���������� �������
volatile uint8_t Frame_up_limit_state = 0;
volatile uint8_t Frame_rope_tension_bottom_limit_state = 0;

// ������� ��������� ����������� �������
volatile uint8_t PNP_current_state = 0;

// ���������� ��������� ����������� �������
volatile uint8_t PNP_prev_state = 0;

// ����������-����� (0->1) ����������� �������
volatile uint8_t PNP_R_trigger = 0;

// ����������-����� (1->0) ����������� �������
volatile uint8_t PNP_F_trigger = 0;

// ����������-����� ����������� ����������� �������
volatile uint8_t PNP_stay_trigger = 1;

// ����������-����� ����������� �������� ������ (����� ���������� ������)
volatile uint32_t PNP_count = 0;

// ����������-����� �������� ����������� �������� ������
volatile uint8_t PNP_count_buffer[4] = {0};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ����� N ��� ������������ ������
volatile uint32_t cycle_N = 0;

// ����� M ��� ������������ ������
volatile uint32_t cycle_M = 0;

// ������� ������ ��������� ������ (��������� ��������)
volatile uint32_t rope_tension = 0;

// ������� ������� ��� ���������
volatile uint32_t rope_tention_target = DEF_TENSION_LIMIT;
volatile uint32_t rope_tension_bottom_limit = 5;
volatile uint32_t previous_rope_tension_bottom_limit = 0;

// ������������� (����������) ������� ��� ���������
volatile uint32_t previous_rope_tention_target = DEF_TENSION_LIMIT;

// ��� ����������� ������� ��� ���������
volatile uint8_t rope_limit_step = 100;

//��������� ��������� (������� � ����������)
volatile uint32_t rope_tension_up_limit = DEF_MAX_TENSION_VALUE;
volatile uint32_t previous_rope_tension_up_limit = 0;



// ���������� ��������� ���������
volatile uint16_t rope_limit_range = 200;

volatile int32_t rope_tension_error = 0;
volatile int32_t rope_tension_avg = 0;
volatile int32_t rope_tension_avg_arr[10]={0};
volatile uint8_t r_t_i = 0;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ��������� ������ ��� (������� ������ ���)
volatile uint8_t ADC_state = 0;

// ���������� ��� ������ ������ ���
volatile uint8_t ADC_ch = 0;

// ����������-������ ���������� �� �������
volatile static uint16_t ADC_delay = 20;

// ����������-������� ������� ����������
volatile static uint16_t ADC_samples = 0;

// ��������� ������ ��� �� 0 ������ (IN_10)
volatile uint16_t A0_raw = 0;

// ��������� ������ ��� �� 1-������ (IN_11)
volatile uint16_t A1_raw = 0;

// ��������� ������ ��� �� 2-������ (IN_12)
volatile uint16_t A2_raw = 0;

// ��������� ������ ��� �� 3-������ (IN_13)
volatile uint16_t A3_raw = 0;

// ��������� ��� �� 0 ������ (���������������� ������)
volatile uint32_t Tenzo_buffer = 0;
volatile uint32_t Tenzo_result = 0;

// ����������-����� ��� ������ �������� ������ ��� �� 0 ������ (���������������� ������) �� �������
volatile uint32_t Tenzo_value = 0;

volatile float N = 0;
volatile float T = 0;



// ����������-����������� ��� 1-�� ��������� ����� ��� (IN_11)
volatile uint8_t K_a1 = 1;

// ����������-����������� ��� 2-�� ��������� ����� ��� (IN_12)
volatile uint8_t K_a2 = 1;

// ����������-����������� ��� 3-�� ��������� ����� ��� (IN_13)
volatile uint8_t K_a3 = 1;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ����������-���� ��������� PWM (� 0 �� 4 �����)
volatile uint8_t PWM_Speed_state = 0;
volatile uint8_t PWM_Tension_state = 0;

// ����������-����� ��� ������������ ��� (���������� ����� 0)
volatile uint16_t PWM_Speed = 0;
volatile uint16_t PWM_Speed_Backup = 0;
volatile uint16_t previous_PWM_Speed = 0;
volatile uint16_t K_PWM_Speed = 20;

// ����������-����� ��� ������������ ��� (���������� ����� 1)
volatile uint16_t PWM_tension = 0;
volatile uint16_t previous_PWM_tension = 0;
volatile uint16_t K_PWM_tension = 10;



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ����������-��������� ����/�������
RTC_Type RTC_DateTime;

// ����������-����� �������� ���������� ����� RTC
uint32_t RTC_Counter = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// �������� � ���
void delay_us(uint32_t tick)
{
	uint32_t wait = 0;

	while(tick)
 {
  __ASM("NOP");
	wait +=1;
  tick--;
 }
}
//

// �������� � ��
void delay_ms(uint32_t tick)
{
	sys_delay = tick;
  while(sys_delay)
 {
 // ����
 }
}
//

// ��������� �������� ���������� ������ � ��� (������ � 0 �� 3)
void ADC_work(void)
{

	// ��������� ��� (����� ������, ������ ���������� ��������������, ��������� ��������, ���������)
		switch(ADC_state)
		{
		   case 0: {
								if(!adc_delay)
								{
									// ����� ���������� �������������� (1 ����� � ���������� ������)
									ADC1->SQR1 = 0;
									ADC1->SQR2 = 0;
									ADC1->SQR3 = 0;

									// �������� ����� ��� ������ � ��� (10,11,12,13) => 1 ����� � ���������� ������ (���������� ����� ������ ��� ��������������)
									ADC1->SQR3 = ADC_ch + 10;
									ADC_state = 1;
//									adc_delay = 25;
									break;
							  }
							}

       case 1: {
								if(!adc_delay)
								{
								ADC1->CR2 |= ADC_CR2_SWSTART;
								ADC_state = 2;
								break;
								}
							 }

       case 2: {
			         if(ADC1->SR & ADC_SR_EOC)
							 {
								switch(ADC_ch)
								{
									case 0: A0_raw = ADC1->DR; ADC_ch = 0; break;
/*
									case 0: A0_raw = ADC1->DR; ADC_ch = 1; break;
									case 1: A1_raw = ADC1->DR; ADC_ch = 2; break;
									case 2: A2_raw = ADC1->DR; ADC_ch = 3; break;
									case 3: A3_raw = ADC1->DR; ADC_ch = 0; break;
*/
									default:
										ADC_ch = 0;
									break;

								}
								ADC_state= 0;
			//					adc_delay = 25;
			         }
					     break;
				      }
			 default: break;
		 }
}
//

void enableRotation(void){
//��� �� �����, ��� ��� ����� ������ ��� ��������
	PWM_Speed = PWM_Speed_Backup; //
	PWM_Speed_state = 1;
	FC_rotate_Fwd();
	PWM_Update(1, PWM_Speed * K_PWM_Speed);
}

volatile static int32_t tension_cmd=0;

void disableRotation(void){
//��� �� �����, ��� ��� ����� ������ ��� ��������
	PWM_Speed = 0;
	PWM_tension = 0;
	PWM_Speed_state = 0;
	PWM_Tension_state = 0;
	FC_rotate_Stop();
	FC_tension_Hold();
	PWM_Update(1, PWM_Speed * K_PWM_Speed);
}

void generateError(){


	disableRotation();
	stopAtv12();
	PWM_Tension_state = 0;
	FC_rotate_Stop();
	FC_tension_Hold();

	PIDreset();
}

void generateErrorOfMotor(){
	Now_Alarm = 6;
	generateError();
}

void generateErrorOfLowTension(){
	if(Now_Alarm != 7)
		if(Now_Alarm != 9)
			if(Now_Alarm != 10){
				Now_Alarm = 7;
				generateError();
			}
}

void generateErrorOfHighTension(){


	if(Now_Alarm == 10)
		return;
	if(Now_Alarm != 8)
		if(Now_Alarm != 9){
			Now_Alarm = 8;

			LOG_PROCESS_ON_FLAG = 0;

			LogTermStruct log_term;
			log_term.regulation_cmd = tension_cmd;

			log_term.tension = T;
			log_term.time_stamp = RTC_GetCounter();
			log_term.cycle_cnt = PNP_count;

			pushValueToBuffer(log_term);

			generateError();
		}
}

void generateErrorOfUpLimit(){
	if(Now_Alarm == 9)
		return;
	Now_Alarm = 9;
	generateError();
}

void generateErrorOfBottomLimit(){
	if(Now_Alarm == 10)
		return;
	Now_Alarm = 10;
	generateError();
}

void shutdownErrorOfBottomLimit(){
	if(Now_Alarm == 10)
		Now_Alarm = 0;
}
void shutdownErrorOfUpLimit(){
	if(Now_Alarm == 9)
		Now_Alarm = 0;
}



int abs_k(int a){
	if(a>=0)
		return a;
	return -a;
}



volatile int32_t tension_cmd_PID = 0;



// ���������� �� ���������� ������� SysTick
void SysTick_Handler(void)
{
	Green_LED_Toggle;

	keyboadrWork();
	screenWork();
	pulsModeWork();
	lazyPIDModeWork();
	// ��������� ���������� ������
	{
		PNP_current_state = D0_in_state;
		FC_motion_state = D1_in_state;
		FC_tension_state = D2_in_state;

		Frame_up_limit_state = D5_in_state;
		Frame_rope_tension_bottom_limit_state = D4_in_state;

	}


	//

	// ������ ����������� ������� (������� �������� �������� �����)
	{
		if(PNP_prev_state != PNP_current_state)
		{
			 // ��������� ��� ��� ������� �� 1 � 0
			 if((PNP_prev_state) && (!PNP_current_state)) { PNP_F_trigger = 1; PNP_R_trigger = 0; PNP_count++; I2C_update = 1;  }

			 // ��������� ��� ��� ������� �� 0 � 1
			 if((!PNP_prev_state) && (PNP_current_state)) { PNP_F_trigger = 0; PNP_R_trigger = 1; }

			 // ��������� �������� ��������� ��� ������ �� ��������� ���
			 PNP_prev_state = PNP_current_state;

			 // �������� ������� ����������� ������������ �������
			 PNP_stay_trigger = 0;

		} else { PNP_stay_trigger = 1; }
	}

	if(!PNP_stay_trigger){ //���� �� ������� �������� ���� ���������, �� ������� ������� ����������� ������
		motor_foult_counter = MOTOR_FOULT_COUNTER_START_VALUE;//�������� 5 ������
	}
#ifndef TABLE_TEST
	if ((PWM_Speed_state == 1) && (!motor_foult_counter)){ //���� ������ ���������, �� ������� ��� �������� �� 0, �� �������� ������
		//������ ������
		generateErrorOfMotor();
	}
#endif


/*--------------------���� ���(������ �� ���) ���������� ���������--------------------------*/

	//������ 1 ������������ �������� ����


#define COUNT_ELEMENT_AVERAGE 10




	if(r_t_i>=COUNT_ELEMENT_AVERAGE){
		r_t_i = 0;
		int32_t tmp_r_t = 0;
		for(unsigned char j=0; j<COUNT_ELEMENT_AVERAGE; j++)
			tmp_r_t +=rope_tension_avg_arr[j];
		rope_tension_avg = (tmp_r_t/COUNT_ELEMENT_AVERAGE);
	}

	rope_tension_avg_arr[r_t_i++] = T*100;

	rope_tension_error = (rope_tention_target) - rope_tension_avg; //T*100;//

	if(	(!Now_Alarm) &&
			(!PWM_Tension_state) &&
			(CURRENT_REG_MODE != NO_REG) &&
			((CURRENT_REG_MODE != LAZY_PID_REG) || (isRegulatingLazyPidMode()) )
		)
	{
		PIDregulating(rope_tension_error, &tension_cmd);
		if(tension_cmd<0){
	//		if(tension_cmd>-3) tension_cmd = -3;
			PWM_tension = -tension_cmd; FC_tension_Down();
		}else if(tension_cmd>0){
	//		if(tension_cmd<3) tension_cmd = 3;
			PWM_tension = tension_cmd; FC_tension_Up();
		}else{
			PWM_tension = 0;
//			FC_tension_Hold();
		}
	}else{
		if(tension_cmd != 0){//�������, ���� ��������� �������������� ��� ���������� ������
			tension_cmd = 0;
			PWM_tension = 0;
			FC_tension_Hold();
		}
		PIDreset();

	//��� ������ ���� �������, ����� ��� ���������� ���� ��������� �� �����������
		//� ����� ����� ��������� ����������� "������" ����������
//		PWM_tension = 0;
//		FC_tension_Hold();
	}


/*--------------------����� ����� ��� ���������� ���������-------------------*/

	////////////////////////////////
	if (!log_buff_delay) {
		log_buff_delay = 20;

		if(LOG_PROCESS_ON_FLAG){ //��������� ��� ��� ����������
			LogTermStruct log_term;
			log_term.regulation_cmd = tension_cmd;

			log_term.tension = T;
			log_term.time_stamp = RTC_GetCounter();
			log_term.cycle_cnt = PNP_count;

			pushValueToBuffer(log_term);

			if(!usart_buff){
				if(onthefly_log_mode){
					LogTermToStr(&log_term, (char *)line_to_uart_send);
					usart_buff = line_to_uart_send;
					sendNextByte();
				}
			}
		}else{
			onthefly_log_mode = 0;
			if(!usart_buff)
			{
				//��� ��� ��������, ���� ���������� ������� 1000 ��������, ���� ��� �� ������
				if(curr_send_cnt == LOG_BUFFER_SIZE+1){//������� ����� ��� ������� ��������
					usart_buff = "N\n";
					sendNextByte();
					curr_send_cnt--;
				}else	if(curr_send_cnt){//���� ��� �� ��� �������� ����� ������ ���������
					curr_send_cnt--;

					LogTermStruct log_term;
					log_term.regulation_cmd = tension_cmd;

					log_term.tension = T;
					log_term.time_stamp = RTC_GetCounter();

					LogTermToStr(&log_term, (char *)line_to_uart_send);
					usart_buff = line_to_uart_send;
					sendNextByte();

				}else if(log_buf_send_cnt--){

					if(log_buf_send_cnt == LOG_BUFFER_SIZE){
						usart_buff = "N\n";
						sendNextByte();
					}else{
						LogTermStruct log_term;

						log_term = getValueFromBuffer();
						LogTermToStr(&log_term, (char *)line_to_uart_send);
						usart_buff = line_to_uart_send;
						sendNextByte();
					}
				}else{//���� ��� ������ ���������, ������ ������� �����
					initLog();
					LOG_PROCESS_ON_FLAG = 1;
				}

			}

		}
	}




	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// ��������� ������ � ��� �� ���� ������� ���������� ������ ( ����� 0 -> ����� 3)
	ADC_work();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	calculateTension(A0_raw, angle_left, angle_right);


	if (T<rope_tension_bottom_limit) { generateErrorOfLowTension(); } //���� ������ �����
	if (T>rope_tension_up_limit) { generateErrorOfHighTension(); }



	if(Frame_rope_tension_bottom_limit_state == 1){
		generateErrorOfUpLimit();
	}else{
		shutdownErrorOfUpLimit();
	}
	if(Frame_up_limit_state == 1){
		generateErrorOfBottomLimit();
	}else{
		shutdownErrorOfBottomLimit();
	}

	// ��������-������� �� ����� (��� ������������ ������ ���������� �������)
//	if (!leds_delay) { Green_LED_Toggle; leds_delay = 200; } //1 �������
//	if (!leds_delay) { USART3->DR = 0xaa; leds_delay = 200; }
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if(sys_delay>0) { sys_delay--; }
	if(leds_delay>0) { leds_delay--; }
	if(adc_delay>0) { adc_delay--; }
	if(glcd_refresh_delay>0) { glcd_refresh_delay--; }
	if(I2C_update_delay>0) { I2C_update_delay--; }
	if(bt_delay>0) { bt_delay--; }
	if(log_buff_delay>0) log_buff_delay--;
	if((PWM_Speed_state == 1) && (motor_foult_counter>0)) {motor_foult_counter--;}

	Green_LED_Toggle;




}
//

// ������ �� ����������������� ��������� ������� ����� �������� ����������
void Read_BDCR(void)
{
	BDCR_UnLock();

	previous_rope_tention_target = BDCR_Read_Word(BKP_DR4);
	rope_tention_target = previous_rope_tention_target;

	previous_rope_tension_up_limit = BDCR_Read_Word(BKP_DR6);
	rope_tension_up_limit = previous_rope_tension_up_limit;

	previous_rope_tension_bottom_limit = BDCR_Read_Word(BKP_DR7);
	rope_tension_bottom_limit = previous_rope_tension_bottom_limit;

	previous_PWM_Speed = BDCR_Read_Word(BKP_DR5);
	PWM_Speed_Backup = previous_PWM_Speed;

	previous_angle_left = BDCR_Read_Word(BKP_DR8);
	angle_left = previous_angle_left;
	previous_angle_right = BDCR_Read_Word(BKP_DR9);
	angle_right = previous_angle_right;

	BDCR_Lock();
}
//

// ���������� �� ������-3 (���������� �� ��� => ������� �������� �������� ��� ������������� ����� ���������� 0.10� �� ������ c ����� STM)
void TIM3_IRQHandler(void)
{
  TIM2->SR &= ~(TIM_SR_UIF);

	PWM_Update(1, PWM_Speed * K_PWM_Speed);
	PWM_Update(2, PWM_tension * K_PWM_tension);
}




int main(void)
{

	K_setClockHSE(72); //freq - 1, 2, 4, 8, 10, 48, 72 [MHz]

	SysTick_Init();

	GPIO_Init();

 // RTC_init_flag = RTC_Init();
	RTC_Init();
	BDCR_Init();
	I2C_Init();
	ADC_Init();
	PWM_Init();
	GLCD_Init();

	// ������������� ������� ����/������� (������ ��� ��������
/*
	RTC_DateTime.RTC_date = 14;
	RTC_DateTime.RTC_month = 3;
	RTC_DateTime.RTC_year = 2019;
	RTC_DateTime.RTC_hours = 16;
	RTC_DateTime.RTC_minutes = 33;
	RTC_DateTime.RTC_seconds = 00;
	RTC_SetCounter(RTC_GetRTC_Counter(&RTC_DateTime));
*/

	initKeyboard();

	menuInit();

	glcd_Buffer_Clear();
	glcd_String("** ���������� *", 0, 0, FonON_InversOFF);
	glcd_String("* ����� ������ *", 0, 3, FonON_InversOFF);
	glcd_String("*   FW.12a18   *", 0, 7, FonON_InversOFF);
	glcd_Show();
	glcd_Backlight_On();

	delay_ms(200);

	PWM_Enable();


//���������� ����� �����������//
	FC_rotate_Stop(); FC_tension_Hold();
	delay_ms(100);
	D1_out_ON;
	D0_out_ON;
	D2_out_ON;
	D3_out_ON;
	delay_ms(100);
	FC_rotate_Stop(); FC_tension_Hold();

	delay_ms(200);

	PNP_count = EEPROM_read_DWord(EEPROM_I2C_24Cxx,0);

	mainSMInit();
	Read_BDCR();

//	calculateTension();

	initLog();
	usart2_init();


	usart3_init_keyboard();

	initAtv12(PWM_Speed_Backup);
	Now_Alarm = 11;//�������
	while(1)
	{

		display_error = updateScreen();


		if(I2C_update==1)
		{
		  if(!I2C_update_delay)
				{
			  	  	soft_error = EEPROM_write_DWord(EEPROM_I2C_24Cxx,0,PNP_count);
					if(!soft_error)
						I2C_update = 0;
					I2C_update_delay = 3000;
				}
		}
#ifndef TABLE_TEST
//		if (!FC_motion_state) { Now_Alarm = 1; generateError();}
		if (!FC_tension_state) { Now_Alarm = 2; generateError();}
#endif
		if(Now_Alarm) {   }

	}

}

