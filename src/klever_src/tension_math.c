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

volatile Optical_Sensor_Struct optical_sensor_data;

volatile TensionMathParameters optical_sensor_math_param;
volatile TensionMathParameters memory_optical_sensor_math_param;

volatile uint8_t update_flag = 0;
/*
 * 1 is equals
 * 0 isnt equals
 */
uint8_t compareTensionMathParam(volatile TensionMathParameters* a, volatile TensionMathParameters * b){
	if (a->L2 != b->L2)
		return 0;
	return 1;
}


void calculateTension(uint16_t A0_raw, volatile TensionMathParameters * math_param){ //A0_raw - Значение АЦП (0-4095), a_l_degr, a_r_degr - углы в градусах * 10
	uint16_t a_l = math_param->angle_left;
	uint16_t a_r = math_param->angle_right;

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
void updateTensionCalculation(void){
	if(update_flag == 0)
		return;
	double A, B, C, D, E, F, V, V_2, V_3, V_4, V_5;
	A = optical_sensor_math_param.A;
	B = optical_sensor_math_param.B;
	C = optical_sensor_math_param.C;
	D = optical_sensor_math_param.D;
	E = optical_sensor_math_param.E;
	F = optical_sensor_math_param.F;
	V = (double)(optical_sensor_math_param.V)/100.0;
	V_2 = V * V;
	V_3 = V * V_2;
	V_4 = V * V_3;
	V_5 = V * V_4;
	optical_sensor_math_param.L1 = round(100.0 * (A*V_5 - B*V_4 + C*V_3 - D*V_2 + E*V - F))/10;

	double L1 = (double)optical_sensor_math_param.L1 / 10.0;
	double L2 = (double)optical_sensor_math_param.L2 / 10.0;
	double R_2 = (double)(optical_sensor_math_param.R*2) / 10.0;
	double R_R_4 = R_2 * R_2;
	double phi, xc, yc,xe, ye, xb, yb, ya, xd, yd, xa;
	phi = -0.051*L1 + 1.5632;
	xc = 260.065;
	yc = 117.774;
	xe = 64.038;
	ye = -109.245;
	xb = 701.0;
	yb = 336.0;
	ya = 116.0;
	xd = 64.038 + 635.3 * cos(phi);
	yd = -109.245 + 635.3 * sin(phi);
	xa = 1473.0 - L2;

	double al, ar;

	double ydc = yd - yc;
	double xdc = xd - xc;
	double sqrt_xdc_ydc_4R = sqrt(xdc * xdc + ydc * ydc - R_R_4);
	double fraq_r = (R_2 * ydc - xdc * sqrt_xdc_ydc_4R)/(-R_2 * xdc - ydc * sqrt_xdc_ydc_4R);
	ar = asin(sqrt(1.0 + fraq_r * fraq_r)) - phi;

	double yda = yd - ya;
	double xda = xd - xa;
	double sqrt_xda_yda_4R = sqrt(xda * xda + yda * yda - R_R_4);
	double fraq_l = (R_2 * yda + xda * sqrt_xda_yda_4R)/(-R_2 * xda + yda * sqrt_xda_yda_4R);
	al = asin(sqrt(1.0 + fraq_l * fraq_l)) + phi;


	optical_sensor_math_param.angle_left = round((al*180.0/M_PI)*1000)/10;
	optical_sensor_math_param.angle_right = round((ar*180.0/M_PI)*1000)/10;
	update_flag = 0;
}

void calculateOpticalSensorVoltage(uint16_t Ax_raw){
	static uint8_t is_calcilation_not_first = 1;
	static uint16_t data_counter = 0;
	uint16_t window_size = optical_sensor_math_param.N_avg;
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

		optical_sensor_math_param.V =  (optical_sensor_data.optical_sensor_voltage*1000)/4095;
		update_flag = 1;
	}
	if (is_calcilation_not_first == 1){
		optical_sensor_data.optical_sensor_voltage = Ax_raw;
		optical_sensor_data.time_stamp = time_stamp;
		optical_sensor_math_param.V =  (optical_sensor_data.optical_sensor_voltage*1000)/4095;
	}

}
