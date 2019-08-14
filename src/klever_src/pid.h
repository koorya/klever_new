#ifndef PID_H	// Блокирую повторное включение этого модуля
#define PID_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#define PID_TIMER_UP_LIMIT 10000


typedef enum {ReceivingE, ComputeP, ComputeI, ComputeD, MakeAction} PIDstep;    // тип данных «итерация вычислений»

typedef struct {uint32_t time_value; } Timer;     // структура для часов реального времени


void setTime(volatile Timer * time_var, volatile Timer * time_val);
void setTimeByValue(volatile Timer * time_var, volatile uint32_t time_val);

char timeIsLower(volatile Timer * time_var1, volatile Timer * time_var2);

char addTime(volatile Timer * time_var, volatile Timer * time_val);

char incremTime(volatile Timer * time_var);

void fixOverflowIssue(volatile Timer * main_var, volatile Timer * var1, volatile Timer * var2);

int32_t receivingE(void);

void makeAction(volatile int32_t u);

void checkU(volatile int32_t *u);

void PIDreset(void);

void PIDregulating (int32_t err, volatile  int32_t *U_res);

#endif



