
extern	volatile char init_atv12_flag;

uint16_t calculateCRCBSw(char *str, uint8_t size);
extern volatile char modbus_command[];
extern volatile uint16_t crc_res;

void initAtv12(uint16_t freq);
void setFreqAtv12(uint16_t freq);
void stopAtv12(void);
void runAtv12(void);


