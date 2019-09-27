#include <stdint.h>
#include <stdio.h>
#include "screen_types.h"
#include "menu_callback.h"








void screenWork(void);

char updateScreen(void);

void menuInit(void);

void initScreen(uint32_t interval, volatile ITEM_TYPE * curr_item);
void initWindow(volatile WINDOW_TYPE * window, char *name, volatile WINDOW_TYPE * par_window);

void voidFunct(void);
void voidFunct_c(char *);
void back_menu_calback(void);

void initItemLabel(volatile ITEM_TYPE * 		item,
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							char 										x_pos, 
							char 										y_pos); 

void initItemSubmenu(volatile ITEM_TYPE * 		item,
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							volatile WINDOW_TYPE * 	chld_window, 
							char 										x_pos, 
							char 										y_pos);
							
void initItemParameter(volatile ITEM_TYPE * 		item,
							char *									name, 
							volatile WINDOW_TYPE * 	par_window, 
							ITEM_OBJ * 							obj,
							char 										x_pos, 
							char 										y_pos,	
							PARAM_VIEW_ENUM_TYPE 		view_mode);	
							
							

ITEM_OBJ * initObj(ITEM_OBJ * obj, void (*dec) (void), void (*inc) (void), void (*show) (char *), OBJ_TYPE_ENUM type);

ITEM_OBJ * initObjChangeable(ITEM_OBJ * obj, void (*inc) (void), void (*dec) (void), void (*show) (char *));
ITEM_OBJ * initObjNotchangeable(ITEM_OBJ * obj, void (*show) (char *));
ITEM_OBJ * initObjChangeableFloatNumb(ITEM_OBJ * obj, float * float_num);

void screenSM(SCREEN_COMMAND_TYPE cmd);
