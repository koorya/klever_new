/*
 * float_input_window.h
 *
 *  Created on: 27 сент. 2019 г.
 *      Author: ivanov.k
 */

#ifndef KLEVER_SRC_LCD_MENU_FLOAT_INPUT_WINDOW_H_
#define KLEVER_SRC_LCD_MENU_FLOAT_INPUT_WINDOW_H_

#include <stdint.h>
#include "screen_types.h"


char showFloatNumberWindow();

void initFloatNumberWindow(ITEM_TYPE * );

SCREEN_COMMAND_TYPE floatNumberWindowSM(SCREEN_COMMAND_TYPE cmd);

#endif /* KLEVER_SRC_LCD_MENU_FLOAT_INPUT_WINDOW_H_ */
