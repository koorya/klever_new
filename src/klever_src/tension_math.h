/*
 * tension_math.h
 *
 *  Created on: 30 ���. 2019 �.
 *      Author: ivanov.k
 */

#ifndef KLEVER_SRC_TENSION_MATH_H_
#define KLEVER_SRC_TENSION_MATH_H_

#include <stdint.h>


extern volatile uint16_t angle_left;
extern volatile uint16_t angle_right;
extern volatile uint16_t previous_angle_left;
extern volatile uint16_t previous_angle_right;
extern volatile uint32_t optical_sensor_voltage;

void calculateTension(uint16_t A0_raw, uint16_t a_l, uint16_t a_r); //A0_raw - �������� ��� (0-4095), a_l_degr, a_r_degr - ���� � �������� * 10
void calculateOpticalSensorVoltage(uint16_t Ax_raw);

#endif /* KLEVER_SRC_TENSION_MATH_H_ */
