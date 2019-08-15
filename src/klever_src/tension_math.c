/*
 * tension_math.c
 *
 *  Created on: 30 ���. 2019 �.
 *      Author: ivanov.k
 */

#include <math.h>
#include "tension_math.h"

extern volatile float N;
extern volatile float tenzo_zero_value;
extern volatile float T;

// ����������-����������� ��� 0-�� ��������� ����� ��� (IN_10)
//volatile uint8_t K_a0 = 25;
volatile uint8_t K_a0 = 10;

volatile uint16_t angle_left = 200;
volatile uint16_t angle_right = 200;
volatile uint16_t previous_angle_left;
volatile uint16_t previous_angle_right;

void calculateTension(uint16_t A0_raw, uint16_t a_l, uint16_t a_r){ //A0_raw - �������� ��� (0-4095), a_l_degr, a_r_degr - ���� � �������� * 10
	N = (K_a0 * 3.3 * A0_raw * 3.7243053 ) / 4095; //
	N -= tenzo_zero_value;
//	N = (K_a0 * 3.3 * A0_raw * 3.7 ) / 4095;
	//3.3 - ���������� ������� ���,
	//3.7 - ���������� ������� ����� ���, (3.7243053 - �������� ����������� ������ �� 54 �� ���������� 0,053 ��.
	//A0_raw - �������� ���
	//4095 - ������������ �������� ���
	//K_a0 - ���������� ��������� �� ���������� � ���� � ��

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
//		denom = (cos(a_r_f - a_l_f) * sin((a_l_f+a_r_f)/2)); //������ ������� (����� 1)
		denom = (sin(a_r_f) + sin(a_l_f)); //����� ������� �� ������� ������ 08.08.2019 (����� 2)(����� 3)
	}
//	T = (2.1244 * N - 12.6) / denom; //������ ������� (����� 1)
//	T = (3.736 * N - 23.450) / denom; //����� ������� �� ������� ������ 08.08.2019 (����� 2)
	T = (3.9214 * N - 27.147) / denom; //����� ������� �� ������� ������ 13.08.2019 (����� 3)
//	T = (2.1244 * N - 14) / denom; //����� ������ �������, �������� ������ ������
	if(T < 0) T = 0;

}

