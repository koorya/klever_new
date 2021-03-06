#include <stm32f10x.h>
#include "../GPIO.h"
#include "keyboard.h"
#include "screen.h"


#define T_CLICK ((1*SYSTICKINTMS)/1000+1)
#define T_PRESS ((30*SYSTICKINTMS)/1000+1)
#define T_PRESS_INT ((32*SYSTICKINTMS)/1000+1)

#define RED_BUTTON_MASK (0x1<<2)
#define GREEN_BUTTON_MASK (0x1<<3)
#define BLUE_BUTTON_MASK (0x1<<0)
#define YELLOW_BUTTON_MASK (0x1<<1)


typedef enum {
	NO_PRESS,
	CLICK,
	PRESS
} STATE_TYPE;

typedef enum {
	EVENT_CLICK_SET_OF_BUTTONS,
	EVENT_PRESS_BUTTON,
	EVENT_CLICK_BUTTON,
	NO_EVENT
} EVENT_TYPE;

typedef struct {
	STATE_TYPE state;
	uint32_t t;
} BUTTON_SM;

volatile char keys_state_now, keys_state_backward; 
volatile BUTTON_SM red_b_sm, yellow_b_sm, green_b_sm, blue_b_sm;


// �������� � ���
void delay_us_KEYBOARD(uint32_t tick)
{ 
	uint32_t wait = 0;
	
	while(tick) 
 {
  __ASM("NOP"); 
	wait +=1; 
  tick--;
 } 
}

void initKeyboard(void){
	keys_state_now = 0;
	keys_state_backward = 0;
	red_b_sm.state = yellow_b_sm.state = green_b_sm.state = blue_b_sm.state = NO_PRESS;
}



char scanKey(char * ret_value){
	static char bt_state_high, bt_state_low, sm = 0;
	if(sm == 0){
		GPIOC->BSRR = GPIO_BSRR_BR11 | GPIO_BSRR_BS12;
	}else if(sm == 2){
		bt_state_low = ((GPIOB->IDR >> 14) & 0x03);
		GPIOC->BSRR = GPIO_BSRR_BS11 | GPIO_BSRR_BR12;
	}else if(sm == 4){		
		bt_state_high = ((GPIOB->IDR >> 14) & 0x03)<<2;
		GPIOC->BSRR = GPIO_BSRR_BR11 | GPIO_BSRR_BS12;
		*ret_value = bt_state_high | bt_state_low;
		sm = 0;
		return 1;
	}
	sm++;
	return 0;
}

EVENT_TYPE stMachineWork(volatile BUTTON_SM *button, char key_value, char key_mask, volatile char *keys_state_backward, volatile  char *keys_state_now){
	if(button->state == NO_PRESS){
		if(key_value){
			if(button->t++ > T_CLICK){
				button->state = CLICK;
				*keys_state_backward |= *keys_state_now |= key_mask;
			}
		}else{
			button->t = 0;
		}
	}else if(button->state == CLICK){
		if(key_value){
			if(button->t++ > T_PRESS)
				button->state = PRESS;
		}else{
			button->state = NO_PRESS;
			*keys_state_now &= ~key_mask;
			if(*keys_state_backward ^ key_mask){					//���� ���� ������ �����-�� ��� ������
				if(!(*keys_state_now))
					return EVENT_CLICK_SET_OF_BUTTONS;
			}else{
				*keys_state_backward = 0;
				return EVENT_CLICK_BUTTON;
			}
		}
	}else if(button->state == PRESS){
		if(key_value){
			if(button->t++ > T_PRESS_INT){
				button->t = T_PRESS;
				if(!(*keys_state_backward ^ key_mask)){
					return EVENT_PRESS_BUTTON;
				}
			}
		}else{
			button->state = NO_PRESS;
			*keys_state_now &= ~key_mask;
			if(*keys_state_backward ^ key_mask){					//���� ���� ������ �����-�� ��� ������
				if(!(*keys_state_now))
					return EVENT_CLICK_SET_OF_BUTTONS;
			}else{
				*keys_state_backward = 0;
			}
		}
	}
	return NO_EVENT;
}


void clickSetOfButtons(volatile char *keys_state_backward){
	if(*keys_state_backward == (RED_BUTTON_MASK | GREEN_BUTTON_MASK)){
		redGreenButtonClickHandler();
	}else if(*keys_state_backward == (RED_BUTTON_MASK | YELLOW_BUTTON_MASK)){
		redYellowButtonClickHandler();
	}else if(*keys_state_backward == (RED_BUTTON_MASK | BLUE_BUTTON_MASK)){
		redBlueButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | GREEN_BUTTON_MASK)){
		yellowGreenButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | BLUE_BUTTON_MASK)){
		yellowBlueButtonClickHandler();
	}else if(*keys_state_backward == (GREEN_BUTTON_MASK | BLUE_BUTTON_MASK)){
		greenBlueButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | GREEN_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | BLUE_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}else if(*keys_state_backward == (GREEN_BUTTON_MASK | BLUE_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}else if(*keys_state_backward == (YELLOW_BUTTON_MASK | GREEN_BUTTON_MASK | BLUE_BUTTON_MASK | RED_BUTTON_MASK)){
		threeButtonClickHandler();
	}
}

void keyboadrWork(void){

	char b_st;
	if(!scanKey(&b_st))
		return;

	EVENT_TYPE event = NO_EVENT;
	event = stMachineWork(&red_b_sm, b_st & RED_BUTTON_MASK, RED_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		redButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		redButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);
	}
	
	event = stMachineWork(&green_b_sm, b_st & GREEN_BUTTON_MASK, GREEN_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		greenButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		greenButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);		
	}

	event = stMachineWork(&yellow_b_sm, b_st & YELLOW_BUTTON_MASK, YELLOW_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		yellowButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		yellowButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);
	}

	event = stMachineWork(&blue_b_sm, b_st & BLUE_BUTTON_MASK, BLUE_BUTTON_MASK, &keys_state_backward, &keys_state_now);
	if(event == EVENT_CLICK_BUTTON){
		blueButtonClickHandler();
	}else if(event == EVENT_PRESS_BUTTON){
		blueButtonPressHandler();
	}else if(event == EVENT_CLICK_SET_OF_BUTTONS){
		clickSetOfButtons(&keys_state_backward);
	}	
	
	
	if(!keys_state_now) //���� ��� ������ ��������, �� �������� ��������� ������� ������
		keys_state_backward = 0;
}

void keyscan_work(uint8_t code){
	static uint8_t last_code = 0;
	if(last_code == 0xf0){ //������ ������ ��������, ������ �� ������
	}else if(code == 0xE0){ //������ ������ �������� �������, ������ �� ������
	}else if(code == 0x70){//0
	//����� �� ���������
		threeButtonClickHandler();
	}else if(code == 0x69){//1
	}else if(code == 0x72){//2
		screenSM(SCR_COMM_down);
	}else if(code == 0x7A){//3
	}else if(code == 0x6B){//4
		screenSM(SCR_COMM_back);
	}else if(code == 0x73){//5
		screenSM(SCR_COMM_select);
	}else if(code == 0x74){//6
		screenSM(SCR_COMM_select);
	}else if(code == 0x6C){//7
	}else if(code == 0x75){//8
		screenSM(SCR_COMM_up);
	}else if(code == 0x7D){//9
	}else if(code == 0x71){//.
	}else if(code == 0x79){//+
	//��������� ���������
		redYellowButtonClickHandler();
	}else if(code == 0x7B){//-
	//�������� ����� 
		yellowBlueButtonClickHandler();
	}else if(code == 0x4A){// "/"
	//����������� ��� ����
		switch_log_state();
	}else if(code == 0x7C){// *
		//�������� ���������
		redBlueButtonClickHandler();
	}else if(code == 0x77){//NumLock
	//��������� ��� ����
		switch_mode_puls_state();
	}else if(code == 0x66){// BackSpace
	//��� ��� ����
		switch_mode_pid_state();
	}else if(code == 0x5A){// Enter
		//����� ������, ������, ���������
		redButtonClickHandler();
	}else{
	}
	last_code = code;
}


void __attribute__ ((weak)) redButtonClickHandler(void){
}
void __attribute__ ((weak)) redButtonPressHandler(void){
}
void __attribute__ ((weak)) blueButtonClickHandler(void){
}
void __attribute__ ((weak)) blueButtonPressHandler(void){
}
void __attribute__ ((weak)) greenButtonClickHandler(void){
}
void __attribute__ ((weak)) greenButtonPressHandler(void){
}
void __attribute__ ((weak)) yellowButtonClickHandler(void){
}
void __attribute__ ((weak)) yellowButtonPressHandler(void){
}

void __attribute__ ((weak)) redGreenButtonClickHandler(void){
}
void __attribute__ ((weak)) redYellowButtonClickHandler(void){
}
void __attribute__ ((weak)) redBlueButtonClickHandler(void){
}

void __attribute__ ((weak)) yellowGreenButtonClickHandler(void){
}
void __attribute__ ((weak)) yellowBlueButtonClickHandler(void){
}
void __attribute__ ((weak)) greenBlueButtonClickHandler(void){
}

void __attribute__ ((weak)) threeButtonClickHandler(void){
}

