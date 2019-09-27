/*
 * float_input_window.c
 *
 *  Created on: 27 сент. 2019 г.
 *      Author: ivanov.k
 */
#include <stdio.h>
#include <math.h>
#include "float_input_window.h"
#include "../GLCD.h"

typedef enum{
	FIW_SELECT_SAVE,
	FIW_SELECT_BACKUP,
	FIW_SELECT_DIGIT,
	FIW_MODIFY_DIGIT,
	FIW_MODIFY_POW
} FLOAT_INPUT_WINDOW_STATE_TYPE;

volatile FLOAT_INPUT_WINDOW_STATE_TYPE flnum_win_state;
volatile uint8_t digit_array[8] = {0};
volatile uint8_t digit_index = 0;
volatile float saved_value;

volatile ITEM_TYPE * item_link;


void setDigitArrayByFloat(float val){

	float tmpVal = (val < 0) ? -val : val;

	int tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = round(tmpFrac * 10000);  // Turn into integer (123).
	int i;
	for (i = 0; i<4; i++){
		digit_array[3-i] = tmpInt1%10;
		digit_array[4+3-i] = tmpInt2%10;
		tmpInt1 /= 10;
		tmpInt2 /= 10;
	}

}
void showDigitArray(char * ret){
	sprintf (ret, "%d%d%d%d.%d%d%d%d",
			digit_array[0],
			digit_array[1],
			digit_array[2],
			digit_array[3],
			digit_array[4],
			digit_array[5],
			digit_array[6],
			digit_array[7]);
}

char showFloatNumberWindow(){
	glcd_String(item_link->name, 0, 0, FonOFF_InversOFF);
	char numb[15];
	show_float(numb, *(item_link->obj->value));
	glcd_String(numb, 1, 1, FonOFF_InversOFF);

	show_float(numb, saved_value);
	char label[] = "last: +0000.0000   ";
	sprintf(label, "last: %s", numb);
	glcd_String(label, 1, 5, FonOFF_InversOFF);

	showDigitArray(numb);
	glcd_String(numb, 1, 3, FonOFF_InversOFF);
	if (flnum_win_state == FIW_SELECT_DIGIT){
		glcd_String("^", digit_index>3?digit_index+2:digit_index+1, 4, FonOFF_InversOFF);
	}else if (flnum_win_state == FIW_MODIFY_DIGIT){
		glcd_String("_", digit_index>3?digit_index+2:digit_index+1, 3, FonOFF_InversOFF);
		glcd_String("_", digit_index>3?digit_index+2:digit_index+1, 2, FonOFF_InversOFF);
	}else if (flnum_win_state == FIW_SELECT_SAVE){
		glcd_String("Save and exit", 1, 7, FonOFF_InversOFF);
	}else if (flnum_win_state == FIW_SELECT_BACKUP){
		glcd_String("Restore ", 1, 7, FonOFF_InversOFF);
	}
	return glcd_Show();
}

void initFloatNumberWindow(ITEM_TYPE * link){
	flnum_win_state = FIW_SELECT_DIGIT;
	item_link = link;
	saved_value = *(item_link->obj->value);
	setDigitArrayByFloat(saved_value);
}

float getFloatByDigitArray(){
	float ret = 0.00001;
	float mul = 0.0001;
	int i = 0;
	for(i=0; i<8; i++){
		ret +=  mul * (float)digit_array[7-i];
		mul *= 10;
	}
	return ret;
}

SCREEN_COMMAND_TYPE floatNumberWindowSM(SCREEN_COMMAND_TYPE cmd){

	if (flnum_win_state == FIW_SELECT_DIGIT){
		if(cmd == SCR_COMM_up){
			if (digit_index < 7){
				digit_index++;
			}else{
				flnum_win_state = FIW_SELECT_BACKUP;
			}
		}else if(cmd == SCR_COMM_down){
			if (digit_index > 0){
				digit_index--;
			}else{
				flnum_win_state = FIW_SELECT_SAVE;
			}
		}else if(cmd == SCR_COMM_select){
			flnum_win_state = FIW_MODIFY_DIGIT;
		}else if(cmd == SCR_COMM_back){
			*(item_link->obj->value) = saved_value;
			return SCR_COMM_back;
		}
	}else if(flnum_win_state == FIW_MODIFY_DIGIT){
		if(cmd == SCR_COMM_up){
			if (digit_array[digit_index] < 9){
				digit_array[digit_index]++;
			}else{
				digit_array[digit_index] = 0;
			}
		}else if(cmd == SCR_COMM_down){
			if (digit_array[digit_index] > 0){
				digit_array[digit_index]--;
			}else{
				digit_array[digit_index] = 9;
			}
		}else if(cmd == SCR_COMM_select){
			flnum_win_state = FIW_SELECT_DIGIT;
		}else if(cmd == SCR_COMM_back){
			*(item_link->obj->value) = saved_value;
			return SCR_COMM_back;
		}
		*(item_link->obj->value) = getFloatByDigitArray();
	}else if(flnum_win_state == FIW_SELECT_BACKUP){
		if(cmd == SCR_COMM_up){
			flnum_win_state = FIW_SELECT_SAVE;
		}else if(cmd == SCR_COMM_down){
			flnum_win_state = FIW_SELECT_DIGIT;
			digit_index = 7;
		}else if(cmd == SCR_COMM_select){
			*(item_link->obj->value) = saved_value;
			setDigitArrayByFloat(saved_value);
		}else if(cmd == SCR_COMM_back){
			*(item_link->obj->value) = saved_value;
			return SCR_COMM_back;
		}
	}else if(flnum_win_state == FIW_SELECT_SAVE){
		if(cmd == SCR_COMM_up){
			flnum_win_state = FIW_SELECT_DIGIT;
			digit_index = 0;
		}else if(cmd == SCR_COMM_down){
			flnum_win_state = FIW_SELECT_BACKUP;
		}else if(cmd == SCR_COMM_select){
			return SCR_COMM_back;
		}else if(cmd == SCR_COMM_back){
			*(item_link->obj->value) = saved_value;
			return SCR_COMM_back;
		}
	}

	return SCR_COMM_defoult;
}

//SCR_COMM_up,//screen command up (кнопка вверх)
//SCR_COMM_down,
//SCR_COMM_select,
//SCR_COMM_back,
//SCR_COMM_defoult



