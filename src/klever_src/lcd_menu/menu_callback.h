#define MAX_ROTATION_SPEED 500 //оборотов в секунду
#define MOTOR_FOULT_COUNTER_START_VALUE 1000 //примерно 5 секунд
#define MOTOR_FOULT_COUNTER_START_VALUE_START 4000 //20 секунд время на разгон

#define DEF_TENSION_LIMIT 14000 //rope_tention_target
#define DEF_PWM_SPEED 50 //PWM_Speed_Backup
#define DEF_MAX_TENSION_VALUE 150 //rope_tension_up_limit


extern void show_yes_no(char * ret, char param);
extern void show_status(char * ret);
extern void show_alarm(char * ret);
extern void show_resurs(char * ret);
extern void show_tenzo(char * ret);
extern void show_log_state(char * ret);
extern void show_interface_error_state(char * ret);
extern void show_save_state(char * ret);
extern void show_up_limit_brackets(char * ret);
extern void save_tenzo_zero(void);
extern void inc_tenzo_zero(void);
extern void dec_tenzo_zero(void);
extern void show_tenzo_zero(char * ret);
extern void show_cur_sensor(char * ret);
extern void show_cur_tension(char * ret);
extern void show_mode(char * ret);
extern void switch_log_state(void);
extern void show_log_state_yes_no(char * ret);
extern void switch_save_state(void);
extern void show_save_state_yes_no(char * ret);	
extern void switch_default_state(void);
extern void show_default_state(char * ret);
extern void switch_mode_not_reg_state(void);
extern void show_mode_not_reg_state(char * ret);
extern void switch_mode_lazy_pid_state(void);
extern void show_mode_lazy_pid_state(char * ret);
extern void inc_mode_lazy_pid_period(void);
extern void dec_mode_lazy_pid_period(void);
extern void show_mode_lazy_pid_period(char * ret);
extern void inc_mode_lazy_pid_err(void);
extern void dec_mode_lazy_pid_err(void);
extern void show_mode_lazy_pid_err(char * ret);
extern void inc_mode_lazy_pid_value(void);
extern void dec_mode_lazy_pid_value(void);
extern void show_mode_lazy_pid_value(char * ret);
extern void switch_mode_pid_state(void);
extern void show_mode_pid_state(char * ret);
extern void inc_mode_pid_value(void);
extern void dec_mode_pid_value(void);
extern void show_mode_pid_value(char * ret);
extern void switch_mode_puls_state(void);
extern void show_mode_puls_state(char * ret);
extern void inc_mode_puls_from_value(void);
extern void dec_mode_puls_from_value(void);
extern void show_mode_puls_from_value(char * ret);
extern void inc_mode_puls_to_value(void);
extern void dec_mode_puls_to_value(void);
extern void show_mode_puls_to_value(char * ret);
extern void inc_mode_puls_period_value(void);
extern void dec_mode_puls_period_value(void);
extern void show_mode_puls_period_value(char * ret);
extern void inc_freq(void);
extern void dec_freq(void);
extern void show_freq(char * ret);
extern void inc_rope_tension_bottom_limit(void); 
extern void dec_rope_tension_bottom_limit(void);
extern void show_rope_tension_bottom_limit(char * ret);	
extern void inc_rope_tension_up_limit(void); 
extern void dec_rope_tension_up_limit(void);
extern void show_rope_tension_up_limit(char * ret);	

extern void inc_angle_left(void);
extern void dec_angle_left(void);
extern void show_angle_left(char * ret);
extern void inc_angle_right(void);
extern void dec_angle_right(void);
extern void show_angle_right(char * ret);
