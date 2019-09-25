/*
 * tension_math.c
 *
 *  Created on: 30 апр. 2019 г.
 *      Author: ivanov.k
 */

#include <math.h>
#include "tension_math.h"

extern volatile float N;
extern volatile float tenzo_zero_value;
extern volatile float T;

// переменная-коэффициент для 0-го аналогово входа АЦП (IN_10)
//volatile uint8_t K_a0 = 25;
volatile uint8_t K_a0 = 10;

volatile uint16_t angle_left = 200;
volatile uint16_t angle_right = 200;
volatile uint16_t previous_angle_left;
volatile uint16_t previous_angle_right;
volatile volatile Optical_Sensor_Struct optical_sensor_data;

void calculateTension(uint16_t A0_raw, uint16_t a_l, uint16_t a_r){ //A0_raw - Значение АЦП (0-4095), a_l_degr, a_r_degr - углы в градусах * 10
	N = (K_a0 * 3.3 * A0_raw * 3.7243053 ) / 4095; //
	N -= tenzo_zero_value;
//	N = (K_a0 * 3.3 * A0_raw * 3.7 ) / 4095;
	//3.3 - напряжение питания АЦП,
	//3.7 - коэфициент деления входа АЦП, (3.7243053 - точномть определения усилия до 54 кг составляет 0,053 кг.
	//A0_raw - значение АЦП
	//4095 - максимальное значение АЦП
	//K_a0 - коэфициент пересчета из напряжения в силу в кг

#ifdef TABLE_TEST
	N = 15;
#endif

	if (N<0) { N = 0; }
//	T = ((5.67 * N) - 42.08);
//	if (T<0) { T = 0; }

	static uint16_t a_l_local = 0;
	static uint16_t a_r_local = 0;
	static float denom = 1;
	if((a_l != a_l_local) || (a_r != a_r_local)){
		a_l_local = a_l;
		a_r_local = a_r;
		float a_l_f = a_l*M_PI/1800;
		float a_r_f = a_r*M_PI/1800;

		denom = (sin(a_r_f) + sin(a_l_f));

		if(denom < 0)
			denom = - denom;
		if(denom < 1e-6)
			denom = 1e-6;

	}


	T = (4.1124 * N - 25.3748) / denom;

	if(T < 0) T = 0;

}

void calculateOpticalSensorVoltage(uint16_t Ax_raw){
	static uint8_t is_calcilation_not_first = 1;
	static uint16_t data_counter = 0;
	uint16_t window_size = 1;
	static uint32_t optical_sensor_voltage_summ = 0;
	static uint32_t time_stamp = 0;
	time_stamp++;
	if (Ax_raw == 0) return;

	optical_sensor_voltage_summ += Ax_raw;
	data_counter++;
	if (data_counter >= window_size){
		optical_sensor_data.optical_sensor_voltage = optical_sensor_voltage_summ / window_size;
		optical_sensor_data.time_stamp = time_stamp;
		data_counter = 0;
		optical_sensor_voltage_summ = 0;
		is_calcilation_not_first = 0;
	}
	if (is_calcilation_not_first == 1){
		optical_sensor_data.optical_sensor_voltage = Ax_raw;
		optical_sensor_data.time_stamp = time_stamp;
	}

}
