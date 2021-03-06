//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  ������������ �������� ���������������� STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef PWM_H	// �������� ��������� ��������� ����� ������
#define PWM_H

/*
 ��� ��������j� ������ (����� LM358) 1-�� CCR = 0.0118� ( 11,8�� )
 ��� ������� ������ (����� MOSFET) 1-�� CCR = 0.0242� ( 24,2�� )
*/

// ==================================================

// ������������� ���
void PWM_Init(void);

// ��������� PWM
void PWM_Enable(void);

// ���������� PWM
void PWM_Disable(void);

// ���������� ���-�� PWM
void PWM_Update(uint8_t Ch, uint16_t CCR);

#endif
//
//
