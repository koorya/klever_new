/*
 * float_input_window.c
 *
 *  Created on: 27 сент. 2019 г.
 *      Author: ivanov.k
 */
#include "float_input_window.h"
#include "../GLCD.h"

typedef enum{
	FIW_SELECT_ITEM,
	FIW_MODIFY_DIGIT,
	FIW_MODIFY_POW
} FLOAT_INPUT_WINDOW_STATE_TYPE;

volatile FLOAT_INPUT_WINDOW_STATE_TYPE flnum_win_state;
volatile uint8_t digit_array[8] = {0};
volatile uint8_t digit_index = 0;
volatile int8_t pow = 0;
volatile ITEM_TYPE * item_link;

char showFloatNumberWindow(){
	glcd_String(item_link->name, 0, 5, FonOFF_InversOFF);
	return glcd_Show();
}

void initFloatNumberWindow(ITEM_TYPE * link){
	flnum_win_state = FIW_SELECT_ITEM;
	item_link = link;
}

SCREEN_COMMAND_TYPE floatNumberWindowSM(SCREEN_COMMAND_TYPE cmd){

	if (flnum_win_state == FIW_SELECT_ITEM){
	}else if(flnum_win_state == FIW_MODIFY_DIGIT){

	}else if(flnum_win_state == FIW_MODIFY_POW){

	}
	if (cmd == SCR_COMM_select)
		return SCR_COMM_back;
	return cmd;
}

