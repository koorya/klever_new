/*
 * tension_math.h
 *
 *  Created on: 30 апр. 2019 г.
 *      Author: ivanov.k
 */

#ifndef KLEVER_SRC_TENSION_MATH_H_
#define KLEVER_SRC_TENSION_MATH_H_

#include <stdint.h>

typedef struct {
	uint32_t optical_sensor_voltage;
	uint32_t time_stamp; //в миллисекундах
} Optical_Sensor_Struct;

typedef struct {
	uint16_t L2; //расстояние в десятых миллиметра расположение шкива на задней раме. если 204.3 мм, то L2 = 2043
	uint16_t L1; //пересчитанное из показаний дальномера расстояние между подвижной рамой и зеленой
	uint16_t V; //напряжение усредненное за N_avg количество точек с дальномера
	uint16_t N_avg; //количество точек усреднения
	uint16_t angle_left;//левый угол, рассчитывается программно
	uint16_t angle_right;//правый угол рассчитывается программно

	uint16_t R;//расстояние от центра шкива до центра каната
	float A, B, C, D, E, F;	//коэффициенты 7 значущих цифр
} TensionMathParameters;

extern volatile Optical_Sensor_Struct optical_sensor_data;

extern volatile TensionMathParameters optical_sensor_math_param;
extern volatile TensionMathParameters memory_optical_sensor_math_param;

uint8_t compareTensionMathParam(volatile TensionMathParameters* a, volatile TensionMathParameters * b);
void calculateTension(uint16_t A0_raw, volatile TensionMathParameters * math_param); //A0_raw - Значение АЦП (0-4095), a_l_degr, a_r_degr - углы в градусах * 10
void calculateOpticalSensorVoltage(uint16_t Ax_raw);
void updateTensionCalculation(void);
#endif /* KLEVER_SRC_TENSION_MATH_H_ */
