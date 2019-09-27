/*
 * tension_math.h
 *
 *  Created on: 30 ���. 2019 �.
 *      Author: ivanov.k
 */

#ifndef KLEVER_SRC_TENSION_MATH_H_
#define KLEVER_SRC_TENSION_MATH_H_

#include <stdint.h>

typedef struct {
	uint32_t optical_sensor_voltage;
	uint32_t time_stamp; //� �������������
} Optical_Sensor_Struct;

typedef struct {
	uint16_t L2; //���������� � ������� ���������� ������������ ����� �� ������ ����. ���� 204.3 ��, �� L2 = 2043
	uint16_t L1; //������������� �� ��������� ���������� ���������� ����� ��������� ����� � �������
	uint16_t V; //���������� ����������� �� N_avg ���������� ����� � ����������
	uint16_t N_avg; //���������� ����� ����������
	uint16_t angle_left;//����� ����, �������������� ����������
	uint16_t angle_right;//������ ���� �������������� ����������

	uint16_t R;//���������� �� ������ ����� �� ������ ������
	float A, B, C, D, E, F;	//������������ 7 �������� ����
} TensionMathParameters;

extern volatile Optical_Sensor_Struct optical_sensor_data;

extern volatile TensionMathParameters optical_sensor_math_param;
extern volatile TensionMathParameters memory_optical_sensor_math_param;

uint8_t compareTensionMathParam(volatile TensionMathParameters* a, volatile TensionMathParameters * b);
void calculateTension(uint16_t A0_raw, volatile TensionMathParameters * math_param); //A0_raw - �������� ��� (0-4095), a_l_degr, a_r_degr - ���� � �������� * 10
void calculateOpticalSensorVoltage(uint16_t Ax_raw);
void updateTensionCalculation(void);
#endif /* KLEVER_SRC_TENSION_MATH_H_ */
