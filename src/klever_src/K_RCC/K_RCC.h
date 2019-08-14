#define K_STURTUP_EXT_OSC_FREQ 10000000 //10MHz
#define K_STURTUP_INT_OSC_FREQ 8000000 //8MHz

typedef struct{
	uint32_t K_SYSCLK_FREQ; 
	uint32_t K_AHB_FREQ; 
	uint32_t K_APB1_FREQ; 
	uint32_t K_APB2_FREQ; 
} K_CLOC_CONTROL;	



typedef enum {
	mcoHSI,
	mcoHSE,
	mcoSYS
} K_MCOSource;

typedef enum{
	cHSI,
	cHSE
} K_SOURCE;

extern uint32_t volatile K_EXT_OSC_FREQ;
extern uint32_t volatile K_INT_OSC_FREQ;
extern K_CLOC_CONTROL volatile K_CLOCK_CONTROL_STRUCT;



void K_updatePeriphFreq(void);

void K_SetExtClockPLL(char mult,	//mult - 4, 5, 6, 7, 8, 9, 65 as 6.5 
	char prediv1, // 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
	char prediv2, // 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
	char mult2);										//8, 9, 10, 11, 12, 13, 14, (not 15) 16, 20; if mult2 == 1 (not used) --> prediv2=1
								

void K_SetExtClockDirect(void);

void K_SetIntClockDirect(void);

void K_SetIntClockPLL(char freq);

void K_initMCOoutput(K_MCOSource source);

void K_setClock(K_SOURCE source, //если HSI, то presc1 = 2, presc2 и PLL2 не используются
	char presc1, //1-16; 
	char presc2, //1-16;
	char kPLL, //1, 4-9, 65 as 6.5 (если kPLL==1, то умножители PLL и PLL2 не используются, делители presc1, presc2 не используются, SW поздключается напрямую)
	char kPLL2, //1, 8-14, 16, 20 (если kPLL2==1, то умножитель PLL2 не используется, делитель presc2 не используется)
	char ahbpresc, // 1, 2, 4, 8, 16, (not 32) 64, 128, 256, 512
	char apb1presc, // 1, 2, 4, 8, 16
	char apb2presc, // 1, 2, 4, 8, 16
	char enable_mco); //0 - disable, !0 - enable (sysclock)


void K_setClockHSE(char freq);	//freq - 1, 2, 4, 8, 10, 48, 72 [MHz]

void K_setClockHSE_MCO(char freq); //freq - 1, 2, 4, 8, 10, 48, 72 [MHz] MCO activate
	
