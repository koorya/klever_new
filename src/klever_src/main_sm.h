#include <stdint.h>

typedef enum{
	NO_REG,
	PID_REG,
	LAZY_PID_REG,
	PULS_REG
} MAIN_SM_ENUM_TYPE;

typedef struct{
	uint32_t current;
	uint32_t backward;
	
} UINT32_BACKWARD_CURRENT_VALUE;

typedef struct{
	UINT32_BACKWARD_CURRENT_VALUE mode_puls_from_value;
	UINT32_BACKWARD_CURRENT_VALUE mode_puls_to_value;
	UINT32_BACKWARD_CURRENT_VALUE mode_puls_period_value;
	uint32_t time;
} PULS_MODE_STRUCT;

typedef struct{
	UINT32_BACKWARD_CURRENT_VALUE mode_lazy_pid_value;
	UINT32_BACKWARD_CURRENT_VALUE mode_lazy_pid_period;
	UINT32_BACKWARD_CURRENT_VALUE mode_lazy_pid_err;
	uint32_t time;
	uint32_t no_reg;
} LAZY_PID_MODE_STRUCT;

extern volatile MAIN_SM_ENUM_TYPE CURRENT_REG_MODE;

extern volatile PULS_MODE_STRUCT mode_puls_struct;
extern volatile LAZY_PID_MODE_STRUCT mode_lazy_pid_struct; 

extern volatile uint32_t rope_tention_target;
extern volatile int32_t rope_tension_error;

extern void mainSMInit(void);

extern void pulsModeWork(void);
extern void lazyPIDModeWork(void);
extern uint8_t isRegulatingLazyPidMode(void);

