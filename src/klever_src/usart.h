#define USART_BUFF_SIZE 200


extern volatile uint16_t usart_buff_index; 
extern volatile const char * usart_buff;

void sendNextByte(void);

void USART2_IRQHandler(void);

void usart2_init(void);/*RS485*/

extern void usart3_init_keyboard(void);

void sendStrToUart2(char *str, char size);

