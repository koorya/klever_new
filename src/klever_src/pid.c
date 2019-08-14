#include "pid.h"

void setTime(volatile Timer * time_var, volatile Timer * time_val){
	if(!time_val){
		time_var->time_value = 0;
		return;
	}
	time_var->time_value = time_val->time_value;
}

void setTimeByValue(volatile Timer * time_var, volatile uint32_t time_val){
	time_var->time_value = time_val;
}


char timeIsLower(volatile Timer * time_var1, volatile Timer * time_var2){
	if(time_var1->time_value > time_var2->time_value)
		return 0;
	return 1;
}

char addTime(volatile Timer * time_var, volatile Timer * time_val){
	if((time_var->time_value += time_val->time_value) > PID_TIMER_UP_LIMIT)
		return 1;
	return 0;
}

char incremTime(volatile Timer * time_var){
	if(++(time_var->time_value) > PID_TIMER_UP_LIMIT)
		return 1;
	return 0;
}

void fixOverflowIssue(volatile Timer * main_var, volatile Timer * var1, volatile Timer * var2){
	if(var1->time_value > var2->time_value){
		var1->time_value -= var2->time_value;
		main_var->time_value -= var2->time_value;;
		var2->time_value = 0;
	}else{
		var2->time_value -= var1->time_value;
		main_var->time_value -= var1->time_value;;
		var1->time_value = 0;
	}		
}



void makeAction(volatile int32_t u){
}

void checkU(volatile int32_t *u){
//	if ((*u)>1000) (*u) = 0;
	return;
}









	volatile char is_first_call = 1;
  volatile int32_t U, I, Kp, Ki, Kd, E, Eprev;
  volatile PIDstep PID_step;
  volatile Timer next_measuring, next_action, current_time;
  volatile Timer next_PID_step;



  volatile int32_t _Kp = 150, _Ki = 20, _Kd = 0;

	volatile int32_t max_I = 50000;


void PIDreset(){
	is_first_call = 1;
}


void PIDregulating (int32_t err, volatile int32_t *U_res)
{

	
  // . инициализация
  if (is_first_call)
  {
    is_first_call = 0;
    I = 0;
    Eprev = 0;
    PID_step = ReceivingE;
    // ... инициализация коэффициентов Kp, Ki, Kd
		Kp = _Kp;//два 0 после запятой
		Ki = _Ki;//100;
		Kd = _Kd;//100;
		setTime(&current_time, 0);
		setTime(&next_measuring, &current_time);
		setTime(&next_action, &current_time);
    // ... задание смещения next_PID_step
		setTimeByValue(&next_PID_step, 5);
		
  }

  switch (PID_step)
  {
  case ReceivingE:   // получение текущей ошибки
    if (!timeIsLower (&next_measuring, &current_time))
      break;         // время для измерения не подошло
    if(addTime(&next_measuring, &next_PID_step))
				fixOverflowIssue(&next_measuring, &current_time, &next_action);
    E = err;
    U = 0;
		PID_step = ComputeI;
	break;
		

  case ComputeI:    // вычисление интегрального составл.
    PID_step = ComputeP;
    if (Ki)
    {
			
      U = I + (Ki * E)/10;// по-моему тут ошибка (понял, ошибки нет, т.к. в u у нас пока только интегральная компонента, к которой мы добавили текущую ошибку)
      I = U;
			if(I>max_I) I = max_I; //2000;
			if(I<-max_I/2) I = -max_I/2; //-2000;
      break;
    }

  case ComputeP:    // вычисление пропорционального составл.
    PID_step = ComputeD;
    if (Kp)
    {
      U += Kp * E;
      break;
    }

  case ComputeD:    // вычисление дифференциального составл.
    PID_step = MakeAction;
    if (Kd)
    {
      U += Kd * (E - Eprev);
      Eprev = E;
      break;
    }

  case MakeAction:  // воздействие
    checkU (&U);    // проверка корректности U
    if (!timeIsLower (&next_action, &current_time))
      break;         // время для воздействия не подошло
    if(addTime (&next_action, &next_PID_step))
			fixOverflowIssue(&next_action, &current_time, &next_measuring);
    *U_res = U/10000; // воздействие U
    PID_step = ReceivingE;
    break;
  }
	if(incremTime(&current_time)) fixOverflowIssue(&current_time, &next_action, &next_measuring);
}

