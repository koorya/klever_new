#include "screen.h"
#include "../tension_math.h"

void menuInit(void){
	static ITEM_OBJ obj[50];
	static WINDOW_TYPE menu_window[20];
	static ITEM_TYPE item[80];//56
	static uint32_t item_index = 0;
	uint32_t startup_index = 0;
	static uint32_t obj_index = 0;
	
		initWindow(W(MAIN), "ГЛАВНОЕ", 0);
		initWindow(W(DATA_MANAGE), "меню данных", W(MAIN));
		initWindow(W(LIMITS), "меню лимитов", W(MAIN));
		initWindow(W(MODE), "меню режимов", W(MAIN));	
			initWindow(W(FORMULAS), "углы", W(MODE));
				initWindow(W(DALNOMER), "Дальномер", W(FORMULAS));
					initWindow(W(L1COEFFICIENTS), "коэффициенты дальномера", W(DALNOMER));
			initWindow(W(PID_MODE), "режим ПИД", W(MODE));
			initWindow(W(PULS_MODE), "режим пульсации", W(MODE));
			initWindow(W(LAZY_PID_MODE), "режим ленивый пид", W(MODE));


	initItemParameter(&(item[item_index++]), "*статус*", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_status), 0, 0, unvisible_name);
	initItemParameter(&(item[item_index++]), "*ошибка записи*", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_interface_error_state), 8, 0, unvisible_name);
	initItemParameter(&(item[item_index++]), "*alarm*", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_alarm), 0, 1, unvisible_name);  
	initItemParameter(&(item[item_index++]), "РЕСУРС", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_resurs), 0, 6, column);  
//	initItemParameter(&(item[item_index++]), "ТЕНЗОДАТЧИК", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_tenzo), 0, 6, column);  
	
	initItemParameter(&(item[item_index++]), "лог ", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_log_state), 11, 0, line);  
	initItemParameter(&(item[item_index++]), "зап ", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_save_state), 11, 1, line);  

	initItemParameter(&(item[item_index++]), "режим", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_mode), 11, 2, column);  
	initItemParameter(&(item[item_index++]), "лимит", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_up_limit_brackets), 11, 4, column);  
	initItemParameter(&(item[item_index++]), "натяг", W(MAIN), initObjNotchangeable(&(obj[obj_index++]), show_cur_tension), 11, 6, column);  

	initItemSubmenu(&(item[item_index++]), "Данные  ", W(MAIN), W(DATA_MANAGE), 0, 2);  
		initItemLabel(&(item[item_index++]), "Меню данных", W(DATA_MANAGE), 0, 0);
		initItemParameter(&(item[item_index++]), 	"Логирование ", W(DATA_MANAGE), initObjChangeable(&(obj[obj_index++]), switch_log_state, switch_log_state, show_log_state_yes_no), 1, 2, line);  
		initItemParameter(&(item[item_index++]), 	"Сохранить   ", W(DATA_MANAGE), initObjChangeable(&(obj[obj_index++]), switch_save_state, switch_save_state, show_save_state_yes_no), 1, 3, line);  
		initItemParameter(&(item[item_index++]), 	"Сброс       ", W(DATA_MANAGE), initObjChangeable(&(obj[obj_index++]), switch_default_state, switch_default_state, show_default_state), 1, 4, line);  
		initItemSubmenu(&(item[item_index++]), "<назад", W(DATA_MANAGE), W(MAIN), 0, 7);  
	initItemSubmenu(&(item[item_index++]), "Лимиты  ", W(MAIN), W(LIMITS), 0, 3);  
		initItemLabel(&(item[item_index++]), "Меню лимитов", W(LIMITS), 0, 0);
		initItemParameter(&(item[item_index++]), 	"Верхний  ", W(LIMITS), initObjChangeable(&(obj[obj_index++]), inc_rope_tension_up_limit, dec_rope_tension_up_limit, show_rope_tension_up_limit), 1, 2, line);  
		initItemParameter(&(item[item_index++]), 	"Нижний   ", W(LIMITS), initObjChangeable(&(obj[obj_index++]), inc_rope_tension_bottom_limit, dec_rope_tension_bottom_limit, show_rope_tension_bottom_limit), 1, 4, line);  
		initItemSubmenu(&(item[item_index++]), "<назад", W(LIMITS), W(MAIN), 0, 7);  
	
	initItemSubmenu(&(item[item_index++]), 		"Режимы  ", W(MAIN), W(MODE), 0, 4);  
		initItemLabel(&(item[item_index++]), "Меню режимов", W(MODE), 0, 0);
		initItemSubmenu(&(item[item_index++]), 		"Формулы  ", W(MODE), W(FORMULAS), 1, 1);
			initItemLabel(&(item[item_index++]), "Формулы", W(FORMULAS), 0, 0);
			initItemParameter(&(item[item_index++]),"L2   ", W(FORMULAS), initObjChangeable(&(obj[obj_index++]), inc_tension_math_L2, dec_tension_math_L2, show_tension_math_L2), 1, 2, line);//NEW clallback
			initItemParameter(&(item[item_index++]),"R    ", W(FORMULAS), initObjChangeable(&(obj[obj_index++]), inc_tension_math_R,  dec_tension_math_R,  show_tension_math_R), 1, 3, line);//NEW clallback
			initItemParameter(&(item[item_index++]),"Z тд ", W(FORMULAS), initObjChangeable(&(obj[obj_index++]), inc_tenzo_zero, dec_tenzo_zero, show_tenzo_zero), 1, 4, line);
			initItemSubmenu(&(item[item_index++]), 		"Дальном", W(FORMULAS), W(DALNOMER), 1, 5);
				initItemLabel(&(item[item_index++]), "Дальномер", W(DALNOMER), 0, 0);
				initItemParameter(&(item[item_index++]),"Усред, В ", W(DALNOMER), initObjNotchangeable(&(obj[obj_index++]), show_optical_sensor_voltage), 1, 2, line);
				initItemParameter(&(item[item_index++]),"Усред L1 ", W(DALNOMER), initObjNotchangeable(&(obj[obj_index++]), show_tension_math_L1), 1, 3, line);//NEW clallback
				initItemParameter(&(item[item_index++]),"N т.уср. ", W(DALNOMER), initObjChangeable(&(obj[obj_index++]), inc_tension_math_N_avg, dec_tension_math_N_avg, show__tension_math_N_avg), 1, 4, line);//NEW clallback
				initItemSubmenu(&(item[item_index++]), 	"Коэфф-ты", W(DALNOMER), W(L1COEFFICIENTS), 1, 5);
					initItemLabel(&(item[item_index++]), "Коэффициенты L1", W(L1COEFFICIENTS), 0, 0);
					initItemParameter(&(item[item_index++]),"A ", W(L1COEFFICIENTS), initObjChangeableFloatNumb(&(obj[obj_index++]),  &(optical_sensor_math_param.A)), 1, 1, line);//NEW clallback
					initItemParameter(&(item[item_index++]),"B ", W(L1COEFFICIENTS), initObjChangeableFloatNumb(&(obj[obj_index++]),  &(optical_sensor_math_param.B)), 1, 2, line);//NEW clallback
					initItemParameter(&(item[item_index++]),"C ", W(L1COEFFICIENTS), initObjChangeableFloatNumb(&(obj[obj_index++]),  &(optical_sensor_math_param.C)), 1, 3, line);//NEW clallback
					startup_index = item_index;

					initItemParameter(&(item[item_index++]),"D ", W(L1COEFFICIENTS), initObjChangeableFloatNumb(&(obj[obj_index++]),  &(optical_sensor_math_param.D)), 1, 4, line);//NEW clallback
					initItemParameter(&(item[item_index++]),"E ", W(L1COEFFICIENTS), initObjChangeableFloatNumb(&(obj[obj_index++]),  &(optical_sensor_math_param.E)), 1, 5, line);//NEW clallback
					initItemParameter(&(item[item_index++]),"F ", W(L1COEFFICIENTS), initObjChangeableFloatNumb(&(obj[obj_index++]),  &(optical_sensor_math_param.F)), 1, 6, line);//NEW clallback

					initItemSubmenu(&(item[item_index++]), "<назад", W(L1COEFFICIENTS), W(DALNOMER), 0, 7);
				initItemSubmenu(&(item[item_index++]), "<назад", W(DALNOMER), W(FORMULAS), 0, 7);
			initItemSubmenu(&(item[item_index++]), "<назад", W(FORMULAS), W(MODE), 0, 7);
			initItemParameter(&(item[item_index++]), "датч.", W(FORMULAS), initObjNotchangeable(&(obj[obj_index++]), show_cur_sensor), 11, 4, column);
			initItemParameter(&(item[item_index++]), "натяг", W(FORMULAS), initObjNotchangeable(&(obj[obj_index++]), show_cur_tension), 11, 6, column);

		initItemParameter(&(item[item_index++]), 	"Не регул  ", W(MODE), initObjChangeable(&(obj[obj_index++]), switch_mode_not_reg_state, switch_mode_not_reg_state, show_mode_not_reg_state), 1, 2, line);  
		initItemSubmenu(&(item[item_index++]), 		"ПИД регул ", W(MODE), W(PID_MODE), 1, 3);  
			initItemLabel(&(item[item_index++]), "Режим ПИД", W(PID_MODE), 0, 0);
			initItemParameter(&(item[item_index++]), 	"Выбор     ", W(PID_MODE), initObjChangeable(&(obj[obj_index++]), switch_mode_pid_state, switch_mode_pid_state, show_mode_pid_state), 1, 2, line);  
			initItemParameter(&(item[item_index++]), 	"Значение  ", W(PID_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_pid_value, dec_mode_pid_value, show_mode_pid_value), 1, 3, line);  
			initItemSubmenu(&(item[item_index++]), "<назад", W(PID_MODE), W(MODE), 0, 7);  
			initItemParameter(&(item[item_index++]), "натяг", W(PID_MODE), initObjNotchangeable(&(obj[obj_index++]), show_cur_tension), 11, 6, column);  
		initItemSubmenu(&(item[item_index++]), 		"ПИД ленив.", W(MODE), W(LAZY_PID_MODE), 1, 4);  
			initItemLabel(&(item[item_index++]), "Режим ПИД ленив.", W(LAZY_PID_MODE), 0, 0);
			initItemParameter(&(item[item_index++]), 	"Выбор     ", W(LAZY_PID_MODE), initObjChangeable(&(obj[obj_index++]), switch_mode_lazy_pid_state, switch_mode_lazy_pid_state, show_mode_lazy_pid_state), 1, 2, line);  
			initItemParameter(&(item[item_index++]), 	"Уставка   ", W(LAZY_PID_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_lazy_pid_value, dec_mode_lazy_pid_value, show_mode_lazy_pid_value), 1, 3, line);  
			initItemParameter(&(item[item_index++]), 	"Окно      ", W(LAZY_PID_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_lazy_pid_err, dec_mode_lazy_pid_err, show_mode_lazy_pid_err), 1, 4, line);
			initItemParameter(&(item[item_index++]), 	"Период    ", W(LAZY_PID_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_lazy_pid_period, dec_mode_lazy_pid_period, show_mode_lazy_pid_period), 1, 5, line);
			initItemSubmenu(&(item[item_index++]), "<назад", W(LAZY_PID_MODE), W(MODE), 0, 7);  
			initItemParameter(&(item[item_index++]), "натяг", W(LAZY_PID_MODE), initObjNotchangeable(&(obj[obj_index++]), show_cur_tension), 11, 6, column);  
		initItemSubmenu(&(item[item_index++]), 		"Пульсация ", W(MODE), W(PULS_MODE), 1, 5);  
			initItemLabel(&(item[item_index++]), "Режим пульсации", W(PULS_MODE), 0, 0);
			initItemParameter(&(item[item_index++]), 	"Выбор     ", W(PULS_MODE), initObjChangeable(&(obj[obj_index++]), switch_mode_puls_state, switch_mode_puls_state, show_mode_puls_state), 1, 2, line);  
			initItemParameter(&(item[item_index++]), 	"От        ", W(PULS_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_puls_from_value, dec_mode_puls_from_value, show_mode_puls_from_value), 1, 3, line);  
			initItemParameter(&(item[item_index++]), 	"До        ", W(PULS_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_puls_to_value, dec_mode_puls_to_value, show_mode_puls_to_value), 1, 4, line);  
			initItemParameter(&(item[item_index++]), 	"Период    ", W(PULS_MODE), initObjChangeable(&(obj[obj_index++]), inc_mode_puls_period_value, dec_mode_puls_period_value, show_mode_puls_period_value), 1, 5, line);  
			initItemSubmenu(&(item[item_index++]), "<назад", W(PULS_MODE), W(MODE), 0, 7);  
			initItemParameter(&(item[item_index++]), "натяг", W(PULS_MODE), initObjNotchangeable(&(obj[obj_index++]), show_cur_tension), 11, 6, column);  
		initItemSubmenu(&(item[item_index++]), "<назад", W(MODE), W(MAIN), 0, 7);  
		initItemParameter(&(item[item_index++]), "натяг", W(MODE), initObjNotchangeable(&(obj[obj_index++]), show_cur_tension), 11, 6, column);  

	initItemParameter(&(item[item_index++]), 	"Част. ", W(MAIN), initObjChangeable(&(obj[obj_index++]), inc_freq, dec_freq, show_freq), 0, 5, line);  

	initScreen(200, &(item[startup_index]));

}
