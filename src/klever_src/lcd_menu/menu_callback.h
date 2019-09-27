#define MAX_ROTATION_SPEED 500 //оборотов в секунду
#define MOTOR_FOULT_COUNTER_START_VALUE 1000 //примерно 5 секунд
#define MOTOR_FOULT_COUNTER_START_VALUE_START 4000 //20 секунд время на разгон

#define DEF_TENSION_LIMIT 14000 //rope_tention_target
#define DEF_PWM_SPEED 50 //PWM_Speed_Backup
#define DEF_MAX_TENSION_VALUE 150 //rope_tension_up_limit


void show_yes_no(char * ret, char param);
void show_status(char * ret);
void show_alarm(char * ret);
void show_resurs(char * ret);
void show_tenzo(char * ret);
void show_log_state(char * ret);
void show_interface_error_state(char * ret);
void show_save_state(char * ret);
void show_up_limit_brackets(char * ret);
void save_tenzo_zero(void);
void inc_tenzo_zero(void);
void dec_tenzo_zero(void);
void show_tenzo_zero(char * ret);
void show_optical_sensor_voltage(char * ret);
void show_cur_sensor(char * ret);
void show_cur_tension(char * ret);
void show_mode(char * ret);
void switch_log_state(void);
void show_log_state_yes_no(char * ret);
void switch_save_state(void);
void show_save_state_yes_no(char * ret);
void switch_default_state(void);
void show_default_state(char * ret);
void switch_mode_not_reg_state(void);
void show_mode_not_reg_state(char * ret);
void switch_mode_lazy_pid_state(void);
void show_mode_lazy_pid_state(char * ret);
void inc_mode_lazy_pid_period(void);
void dec_mode_lazy_pid_period(void);
void show_mode_lazy_pid_period(char * ret);
void inc_mode_lazy_pid_err(void);
void dec_mode_lazy_pid_err(void);
void show_mode_lazy_pid_err(char * ret);
void inc_mode_lazy_pid_value(void);
void dec_mode_lazy_pid_value(void);
void show_mode_lazy_pid_value(char * ret);
void switch_mode_pid_state(void);
void show_mode_pid_state(char * ret);
void inc_mode_pid_value(void);
void dec_mode_pid_value(void);
void show_mode_pid_value(char * ret);
void switch_mode_puls_state(void);
void show_mode_puls_state(char * ret);
void inc_mode_puls_from_value(void);
void dec_mode_puls_from_value(void);
void show_mode_puls_from_value(char * ret);
void inc_mode_puls_to_value(void);
void dec_mode_puls_to_value(void);
void show_mode_puls_to_value(char * ret);
void inc_mode_puls_period_value(void);
void dec_mode_puls_period_value(void);
void show_mode_puls_period_value(char * ret);
void inc_freq(void);
void dec_freq(void);
void show_freq(char * ret);
void inc_rope_tension_bottom_limit(void);
void dec_rope_tension_bottom_limit(void);
void show_rope_tension_bottom_limit(char * ret);
void inc_rope_tension_up_limit(void);
void dec_rope_tension_up_limit(void);
void show_rope_tension_up_limit(char * ret);

void inc_tension_math_L2(void);
void dec_tension_math_L2(void);
void show_tension_math_L2(char * ret);

void inc_tension_math_R(void);
void dec_tension_math_R(void);
void show_tension_math_R(char * ret);

void inc_tension_math_N_avg(void);
void dec_tension_math_N_avg(void);
void show__tension_math_N_avg(char * ret);

void show_tension_math_L1(char * ret);





