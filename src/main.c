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


#define MOTOR_FOULT_COUNTER_START_VALUE 1000 //примерно 5 секунд
#define MOTOR_FOULT_COUNTER_START_VALUE_START 4000 //20 секунд время на разгон
#define MAX_TENSION_VALUE 1000
#define MIN_TENSION_VALUE 5







//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//   T = (((2698*N)-(20020+(8/10)))/(475+(7/10)));
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// флаг аварий/ошибок
volatile uint8_t Now_Alarm = 0;
//код ошибки работы с памятью
volatile uint8_t soft_error = 0;
volatile uint8_t display_error = 0;

// текущее состояние системы АСУ
volatile static uint8_t Global_State = 0;

// обозначение текущего состояния системы АСУ
const char *Global_Caption[]={ "Норма", "Авар.ПЧ1", "Авар.ПЧ2", "Авар.УС+", "Авар.УС-", "Авар.Кнт", "Авар.Двиг", "Р.Ослаб", "Р.Перет", "В.пред", "Н.пред", "ПРОВЕРЬ"};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// переменная-флаг наличия аварий/ошибок на частотниках
volatile static uint8_t FC_motion_state = 0;
volatile static uint8_t FC_tension_state = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// переменная-флаг для обмена данными с EEPROM на шине I2C
volatile uint8_t I2C_update = 0;

// переменная-счетчик для обмена данными с EEPROM на шине I2C
volatile uint16_t I2C_update_delay = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// переменная-счетчик (реальный таймер времени)
volatile uint32_t sys_delay = 0;

// переменная-счетчик (аналого-цифровой преобразователь)
volatile uint32_t adc_delay = 0;

// переменная-счетчик для обновления показаний на дисплеи
volatile uint32_t glcd_refresh_delay = 0;

// переменная-счетчик для индикации светодиодами
volatile uint32_t leds_delay = 0;

//переменная-счетчик для мониторинга остановки двигателя, отслеживает аварию
volatile uint32_t motor_foult_counter = 0;

// переменная-счетчик для логирования
volatile uint32_t log_buff_delay = 0;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// позиция курсора меню X
volatile uint8_t  cursor_x = 15;

// позиция курсора меню Y
volatile uint8_t  cursor_y = 0;

// позиция курсора в меню / выбранный параметр
volatile uint8_t  menu = 0;

// какая страница меню активна на экране
volatile uint8_t  page_menu = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// текущее состояние 1 пары кнопок
volatile uint8_t bt_state_low = 0;

// текущее состояние 2 пары кнопок
volatile uint8_t bt_state_high = 0;

// общее состояние всех кнопок
volatile uint16_t bt_state = 0;

// предыдущее общее состояние всех кнопок
volatile uint16_t bt_before_state = 0;

// переменная для смены опроса пар кнопок
volatile uint8_t bt_toggle = 0;

// переменная-буфер (устранения дребезга контактов)
volatile uint16_t bt_count = 0;

// таймер для мультизадания у кнопок
volatile uint16_t bt_delay = 200;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// текущее состояние магнитного датчика
volatile uint8_t Frame_up_limit_state = 0;
volatile uint8_t Frame_rope_tension_bottom_limit_state = 0;

// текущее состояние оптического датчика
volatile uint8_t PNP_current_state = 0;

// предыдущее состояние оптического датчика
volatile uint8_t PNP_prev_state = 0;

// переменная-буфер (0->1) оптического датчика
volatile uint8_t PNP_R_trigger = 0;

// переменная-буфер (1->0) оптического датчика
volatile uint8_t PNP_F_trigger = 0;

// переменная-буфер бездействия оптического датчика
volatile uint8_t PNP_stay_trigger = 1;

// переменная-буфер выполненных оборотов шкивов (через оптический датчик)
volatile uint32_t PNP_count = 0;

// переменная-буфер хранения выполненных оборотов шкивов
volatile uint8_t PNP_count_buffer[4] = {0};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// число N для циклического режима
volatile uint32_t cycle_N = 0;

// число M для циклического режима
volatile uint32_t cycle_M = 0;

// текущая усилие натяжения каната (расчетное значение)
volatile uint32_t rope_tension = 0;

// текущая граница для натяжения
volatile uint32_t rope_tention_target = DEF_TENSION_LIMIT;
volatile uint32_t rope_tension_bottom_limit = 5;
volatile uint32_t previous_rope_tension_bottom_limit = 0;

// установленная (записанная) граница для натяжения
volatile uint32_t previous_rope_tention_target = DEF_TENSION_LIMIT;

// шаг выставления границы для натяжения
volatile uint8_t rope_limit_step = 100;

//аварийное натяжение (текущая и записанная)
volatile uint32_t rope_tension_up_limit = DEF_MAX_TENSION_VALUE;
volatile uint32_t previous_rope_tension_up_limit = 0;



// гистерезис обработки натяжения
volatile uint16_t rope_limit_range = 200;

volatile int32_t rope_tension_error = 0;
volatile int32_t rope_tension_avg = 0;
volatile int32_t rope_tension_avg_arr[10]={0};
volatile uint8_t r_t_i = 0;


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// состояние работы АЦП (автомат работы АЦП)
volatile uint8_t ADC_state = 0;

// переменная для выбора канала АЦП
volatile uint8_t ADC_ch = 0;

// переменная-таймер усреднение по времени
volatile static uint16_t ADC_delay = 20;

// переменная-счетчик выборок усреднения
volatile static uint16_t ADC_samples = 0;

// результат работы АЦП на 0 канале (IN_10)
volatile uint16_t A0_raw = 0;

// результат работы АЦП на 1-канале (IN_11)
volatile uint16_t A1_raw = 0;

// результат работы АЦП на 2-канале (IN_12)
volatile uint16_t A2_raw = 0;

// результат работы АЦП на 3-канале (IN_13)
volatile uint16_t A3_raw = 0;

// результат АЦП на 0 канале (тензометрический датчик)
volatile uint32_t Tenzo_buffer = 0;
volatile uint32_t Tenzo_result = 0;

// переменная-буфер для вывода значений работы АЦП на 0 канале (тензометрический датчик) на дисплей
volatile uint32_t Tenzo_value = 0;

volatile float N = 0;
volatile float tenzo_zero_value = 0;
volatile float T = 0;



// переменная-коэффициент для 1-го аналогово входа АЦП (IN_11)
volatile uint8_t K_a1 = 1;

// переменная-коэффициент для 2-го аналогово входа АЦП (IN_12)
volatile uint8_t K_a2 = 1;

// переменная-коэффициент для 3-го аналогово входа АЦП (IN_13)
volatile uint8_t K_a3 = 1;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// переменная-флаг состояния PWM (с 0 по 4 канал)
volatile uint8_t PWM_Speed_state = 0;
volatile uint8_t PWM_Tension_state = 0;

// переменная-буфер для формирования ШИМ (аналоговый выход 0)
volatile uint16_t PWM_Speed = 0;
volatile uint16_t PWM_Speed_Backup = 0;
volatile uint16_t previous_PWM_Speed = 0;
volatile uint16_t K_PWM_Speed = 20;

// переменная-буфер для формирования ШИМ (аналоговый выход 1)
volatile uint16_t PWM_tension = 0;
volatile uint16_t previous_PWM_tension = 0;
volatile uint16_t K_PWM_tension = 10;



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// переменная-структура Даты/Времени
RTC_Type RTC_DateTime;

// переменная-буфер счетчика милисекунд блока RTC
uint32_t RTC_Counter = 0;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Задержки в мкс
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

// Задержка в мс
void delay_ms(uint32_t tick)
{
	sys_delay = tick;
  while(sys_delay)
 {
 // ждем
 }
}
//

// Получение значений аналоговых входов с АЦП (каналы с 0 по 3)
void ADC_work(void) {

	// Обработка АЦП (выбор канала, запуск одиночного преобразование, получение значений, обработка)
	switch (ADC_state) {
	case 0: {
		if (!adc_delay) {
			// Режим одиночного преобразования (1 канал в регулярной группе)
			ADC1->SQR1 = 0;
			ADC1->SQR2 = 0;
			ADC1->SQR3 = 0;

			// Выбираем канал для работы с АЦП (10,11,12,13) => 1 канал в регулярной группе (записываем номер канала для преобразования)
			ADC1->SQR3 = ADC_ch + 10;
			ADC_state = 1;
//									adc_delay = 25;
			break;
		}
	}

	case 1: {
		if (!adc_delay) {
			ADC1->CR2 |= ADC_CR2_SWSTART;
			ADC_state = 2;
			break;
		}
	}

	case 2: {
		if (ADC1->SR & ADC_SR_EOC) {
			switch (ADC_ch) {
			case 0:
				A0_raw = ADC1->DR;
				ADC_ch = 1;
				break;
			case 1:
				A1_raw = ADC1->DR;
				ADC_ch = 0;
				break;
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
			ADC_state = 0;
			//					adc_delay = 25;
		}
		break;
	}
	default:
		break;
	}
}
//

void enableRotation(void){
//уже не нужна, так как через матбас все работает
	PWM_Speed = PWM_Speed_Backup; //
	PWM_Speed_state = 1;
	FC_rotate_Fwd();
	PWM_Update(1, PWM_Speed * K_PWM_Speed);
}

volatile static int32_t tension_cmd=0;

void disableRotation(void){
//уже не нужна, так как через матбас все работает
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
			if(!onthefly_log_mode)
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



// Прерывание от системного таймера SysTick
void SysTick_Handler(void)
{
	Green_LED_Toggle;

	keyboadrWork();
	screenWork();
	pulsModeWork();
	lazyPIDModeWork();
	// Обработка дискретных входов
	{
		PNP_current_state = D0_in_state;
		FC_motion_state = D1_in_state;
		FC_tension_state = D2_in_state;

		Frame_up_limit_state = D5_in_state;
		Frame_rope_tension_bottom_limit_state = D4_in_state;

	}


	//

	// Анализ оптического датчика (счетчик оборотов ведущего шкива)
	{
		if(PNP_prev_state != PNP_current_state)
		{
			 // проверяем что был переход из 1 в 0
			 if((PNP_prev_state) && (!PNP_current_state)) { PNP_F_trigger = 1; PNP_R_trigger = 0; PNP_count++; I2C_update = 1;  }

			 // проверяем что был переход из 0 в 1
			 if((!PNP_prev_state) && (PNP_current_state)) { PNP_F_trigger = 0; PNP_R_trigger = 1; }

			 // обновляем значение состояния для оценки на следующий раз
			 PNP_prev_state = PNP_current_state;

			 // обнуляем триггер бездействия индуктивного датчика
			 PNP_stay_trigger = 0;

		} else { PNP_stay_trigger = 1; }
	}

	if(!PNP_stay_trigger){ //если на датчике оборотов было изменение, то взводим счетчик бездействия заново
		motor_foult_counter = MOTOR_FOULT_COUNTER_START_VALUE;//примерно 5 секунд
	}
#ifndef TABLE_TEST
	if ((PWM_Speed_state == 1) && (!motor_foult_counter)){ //если должен куртиться, но счетчик уже досчитал до 0, то возникла авария
		//ошибка мотора
		generateErrorOfMotor();
	}
#endif


/*--------------------БЛОК ПИД(сейчас не пид) РЕГУЛЯТОРА НАТЯЖЕНИЯ--------------------------*/

	//каждую 1 миллисекунду попадаем сюда


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
		if(tension_cmd != 0){//решение, чтоб двигатель остонавливался при отключении режима
			tension_cmd = 0;
			PWM_tension = 0;
			FC_tension_Hold();
		}
		PIDreset();

	//две строки ниже добавил, когда при отключении пида двигатель не остановился
		//с этими двумя строчками блокируется "ручное" управление
//		PWM_tension = 0;
//		FC_tension_Hold();
	}


/*--------------------КОНЕЦ БЛОКА ПИД РЕГУЛЯТОРА НАТЯЖЕНИЯ-------------------*/


	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// Обработка данных с АЦП по всем входным аналоговым входам ( канал 0 -> канал 3)
	ADC_work();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	calculateOpticalSensorVoltage(A1_raw);
	A1_raw = 0;

	calculateTension(A0_raw, &optical_sensor_math_param);

	////////////////////////////////
	if (!log_buff_delay) {
		log_buff_delay = 20;

		if(LOG_PROCESS_ON_FLAG){ //заполняем лог или отправляем
			LogTermStruct log_term;
			log_term.regulation_cmd = tension_cmd;

			log_term.tension = T;
			log_term.time_stamp = RTC_GetCounter();
			log_term.cycle_cnt = PNP_count;
			log_term.optical_sensor = optical_sensor_data;
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
				//тут два варианта, либо отправляем текущие 1000 значений, либо уже из буфера
				if(curr_send_cnt == LOG_BUFFER_SIZE+1){//создаем папку под текущие значения
					usart_buff = "N\n";
					sendNextByte();
					curr_send_cnt--;
				}else	if(curr_send_cnt){//если еще не все значения после аварии отправили
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
				}else{//если все данные отправили, заново взводим буфер
					initLog();
					LOG_PROCESS_ON_FLAG = 1;
				}

			}

		}
	}







	if (T<rope_tension_bottom_limit) { generateErrorOfLowTension(); } //если ремень ослаб
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

	// Брякалка-мигалка на плате (для тестирования работы системного таймера)
//	if (!leds_delay) { Green_LED_Toggle; leds_delay = 200; } //1 секунда
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

// Чтение из энергонезависимых регистров величин ранее заданных параметров
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

	optical_sensor_math_param.L2 = BDCR_Read_Word(BKP_DR8);
	memory_optical_sensor_math_param.L2 = optical_sensor_math_param.L2;

	memory_optical_sensor_math_param.A = 0.1308;
	memory_optical_sensor_math_param.B = 3.5751;
	memory_optical_sensor_math_param.C = 37.1262;
	memory_optical_sensor_math_param.D = 181.1357;
	memory_optical_sensor_math_param.E = 390.734;
	memory_optical_sensor_math_param.F = 173.0224;
	memory_optical_sensor_math_param.N_avg = 1500;
	memory_optical_sensor_math_param.R = 3000;

	optical_sensor_math_param = memory_optical_sensor_math_param;

	uint16_t tenzo_zero_value_uint16 = BDCR_Read_Word(BKP_DR10);
	tenzo_zero_value = tenzo_zero_value_uint16;
	tenzo_zero_value /= 100.0;

	BDCR_Lock();
}
//

// Прерывание по Таймер-3 (Прерывание по ШИМ => Задание скорости вращения для асинхронников через напряжение 0.10В на выходе c платы STM)
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

	// Корректировка текущей даты/времени (разово при прошивки
/*
	RTC_DateTime.RTC_date = 25;
	RTC_DateTime.RTC_month = 9;
	RTC_DateTime.RTC_year = 2019;
	RTC_DateTime.RTC_hours = 12;
	RTC_DateTime.RTC_minutes = 44;
	RTC_DateTime.RTC_seconds = 00;
	RTC_SetCounter(RTC_GetRTC_Counter(&RTC_DateTime));
*/

	initKeyboard();

	menuInit();

	glcd_Buffer_Clear();
	glcd_String("** ЭНЕРГОЗАПС *", 0, 0, FonON_InversOFF);
	glcd_String("* Стенд КЛЕВЕР *", 0, 3, FonON_InversOFF);
	glcd_String("*   FW.12a18   *", 0, 7, FonON_InversOFF);
	glcd_Show();
	glcd_Backlight_On();

	delay_ms(200);

	PWM_Enable();


//устранение глюка частотников//
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
	Now_Alarm = 11;//ПРОВЕРЬ
	while(1)
	{

		display_error = updateScreen();
		updateTensionCalculation();

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

