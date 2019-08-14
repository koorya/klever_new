#include <stdint.h>
#include "usart.h"
#include "modbus.h"

	volatile char init_atv12_flag = 0;
#define FC1_DEV_ADDR 0x0b
#define WRITE_MODBUS_FUNCT 0x06
#define CONTROL_WORD_ADDR 0x2135 //8501 dec
#define FREQUENCY_SETPOINT_ADDR 0x2136 //8502 dec

volatile char modbus_command[] = {0x0b, 0x06, 0x21, 0x36, 0x00, 0x0a, 0x00, 0x00};
volatile uint16_t crc_res = 0;


uint16_t calculateCRC(char *str, uint8_t size){
	uint16_t CRC = 0xffff;
	uint8_t i, j;
	for(i = 0; i<size; i++){
		CRC ^= str[i];
		for(j = 0; j < 8; j++){
			if(CRC & 0x0001){
				CRC >>= 1;
				CRC ^= 0xa001;
			}else{
				CRC >>= 1;
			}
		}
	}
	return CRC;
}
uint16_t byteSwap(uint16_t word){
	return (word>>8) + (word<<8);
}
uint16_t calculateCRCBSw(char *str, uint8_t size){
	return byteSwap(calculateCRC(str, size));
}

void delay_ms(uint32_t tick);
void initAtv12(uint16_t freq){
	modbus_command[0] = FC1_DEV_ADDR;
	modbus_command[1] = WRITE_MODBUS_FUNCT;
	modbus_command[2] = CONTROL_WORD_ADDR >> 8;
	modbus_command[3] = CONTROL_WORD_ADDR & 0x00ff;
	modbus_command[4] = 0x00;
	modbus_command[5] = 0x06;
	uint16_t crc = calculateCRC((char*)modbus_command, 6);
	modbus_command[6] = crc & 0x00ff;
	modbus_command[7] = crc >> 8;
	
	sendStrToUart2((char*)modbus_command, 8);
	delay_ms(200);
	setFreqAtv12(freq);//0.1Hz
}


void setFreqAtv12(uint16_t freq){
	if(freq>500)
		freq = 500;
	modbus_command[0] = FC1_DEV_ADDR;
	modbus_command[1] = WRITE_MODBUS_FUNCT;
	modbus_command[2] = FREQUENCY_SETPOINT_ADDR >> 8;
	modbus_command[3] = FREQUENCY_SETPOINT_ADDR & 0x00ff;
	modbus_command[4] = freq >> 8;
	modbus_command[5] = freq & 0x00ff;
	uint16_t crc = calculateCRC((char*)modbus_command, 6);
	modbus_command[6] = crc & 0x00ff;
	modbus_command[7] = crc >> 8;
	
	sendStrToUart2((char*)modbus_command, 8);
}
void stopAtv12(void){
	modbus_command[0] = FC1_DEV_ADDR;
	modbus_command[1] = WRITE_MODBUS_FUNCT;
	modbus_command[2] = CONTROL_WORD_ADDR >> 8;
	modbus_command[3] = CONTROL_WORD_ADDR & 0x00ff;
	modbus_command[4] = 0x00;
	modbus_command[5] = 0x07;
	uint16_t crc = calculateCRC((char*)modbus_command, 6);
	modbus_command[6] = crc & 0x00ff;
	modbus_command[7] = crc >> 8;
	
	sendStrToUart2((char*)modbus_command, 8);
}
void runAtv12(void){
		modbus_command[0] = FC1_DEV_ADDR;
	modbus_command[1] = WRITE_MODBUS_FUNCT;
	modbus_command[2] = CONTROL_WORD_ADDR >> 8;
	modbus_command[3] = CONTROL_WORD_ADDR & 0x00ff;
	modbus_command[4] = 0x00;
	modbus_command[5] = 0x0f;
	uint16_t crc = calculateCRC((char*)modbus_command, 6);
	modbus_command[6] = crc & 0x00ff;
	modbus_command[7] = crc >> 8;
	
	sendStrToUart2((char*)modbus_command, 8);
}





