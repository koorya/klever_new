//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
//  Конфигурация перефирийной шины I2C микроконтроллера STM32F10xxxx
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef I2C_H	// Блокирую повторное включение этого модуля
#define I2C_H

#define  EEPROM_I2C_24Cxx 0x50

#define I2C_MODE_READ	  1
#define I2C_MODE_WRITE	0
#define I2C_Set_Address(addr, mode)	( (addr << 1 ) | mode )

// ==================================================

// Костыльная инициализация выводов SDA и SCL для I2C (согласно Errata стр.26)
void I2C_GPIO_conf_with_Errata(void);

// Инициализация I2C (используем I2C2)
void I2C_Init(void);

// Чтение состояний битов в регистре SR1
uint16_t I2C_read_SR1(void);
	
// Чтение состояний битов в регистре SR1
uint16_t I2C_read_SR2(void);

// Запись одного полного слова (32бит, DWORD) в EEPROM на шине I2C
char EEPROM_write_DWord(uint8_t device,  uint8_t cell, uint32_t raw);

// Процедура чтения слова (страницs памяти, 16бит, WORD) из EEPROM на шине I2C
uint16_t EEPROM_read_word(uint8_t device,  uint8_t cell);

// Чтение одного полного слова (32бит, DWORD) из EEPROM на шине I2C
uint32_t EEPROM_read_DWord(uint8_t device, uint8_t cell);


#endif
//
//
