#include "stm32f10x.h"
#include "GLCD.h"
#include "font8x8.h"
#include <math.h>
#include <stdio.h>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

volatile uint8_t glcd_buff[1024]; // ����� ��� ������ � �������

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// ������������� ���������� SPI
void SPI_Init(void)
{

	// ���������� ������������ ������� SPI1 � ���� ������������
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	// ������������ SPI2. 
	// ������������� ��������� ������ SPI ����������, �������� ������� ������ (������� "�����")
	SPI1->CR1 = 0;
	SPI1->CR2 = 0;
	SPI1->DR = 0;
	
	// �������� ���������������: 
	// ������� �� ������� ������������ ������
	// CPOL = 1 ������� �� 1 � 0 (� ������� �� ���� ������� �������)
	// ���� = 1 �� ������� ������
	// MSTR = 1 �������� � ������ �������
	// BR[2:0] = 011 ������������� ������������ �� 16
	// BR[2:0] = 100 ������������� ������������ �� 32
	// BR[2:0] = 101 ������������� ������������ �� 64
	// BR[2:0] = 110 ������������� ������������ �� 128
	// BR[2:0] = 111 ������������� ������������ �� 256 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0 |
	SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_BR | SPI_CR1_CPHA | SPI_CR1_CPOL; // SPI_CR1_DFF | 
	
	// �������� ��������� SPI
  SPI1->CR1 |= SPI_CR1_SPE;

}
//

// �������� ������ � ����� SPI
char spi_Send(uint8_t raw)
{
	SPI1->DR = raw;
	uint32_t err_cnt = 100000000;
	while(SPI1->SR & SPI_SR_BSY && --err_cnt)
	{
			 // ���� ���� �� �������� ���� ��� (�� ��������� �������� ������)
	}
	if (!err_cnt)
		return 1;
	return 0;
	
}
//

// �������� ������/������� � GLCD ST7920
char glcd_Send(uint8_t raw, uint8_t cmd)
{
	uint8_t StartByte = 0;
	char err_cnt = 0;
	
	// ���������� CS � 1 (������ �������� ������/�������)
	GPIOA->BSRR = GPIO_BSRR_BS4;
  
	if (!cmd) { StartByte = 0xF8; } else { StartByte = 0xFA; }
  
	// ���������� ��������� ���
	err_cnt += spi_Send(StartByte);
	
	// ���������� ������� ������� ������
	err_cnt += spi_Send(raw & 0xF0);
	
	// ���������� ������� ������� ������
	err_cnt += spi_Send((raw & 0x0F) << 4);
	 
	// ���������� CS � 0 (��������� �������� ������/�������)
	GPIOA->BRR = GPIO_BRR_BR4;
	return err_cnt;
}
//

// �������� "���-�������"
void glcd_Delay(uint32_t tick)
{ 
	uint32_t wait = 0;
 	tick = tick * 8000;
	
	while(tick) 
 {
  __ASM("NOP"); 
	wait +=1; 
  tick--;
 } 
}
//

// ������������� ������������ ������
void GLCD_Init(void)
{
	// ������������� SPI1 ���������, � �������� ��������� �������
	// ��������� SPI1
	// LED       - ����� PA10 - ����� �������� ��� �����
	// MOSI (RW) - ����� PA7 - �������������� �������
	// Reset     - ����� PA0 - ����� �������� ��� �����
	// CLK (E)   - ����� PA5 - �������������� �������
	// CS (RS)   - ����� PA4 - ����� �������� ��� �����
	SPI_Init();
	glcd_Delay(50);
	
	// ������ ����� ����������� ������� � ��������� ��������� ������� (����� ������ :))
	GPIOA->BRR = ( GPIO_BRR_BR0 | GPIO_BRR_BR10 );
	glcd_Delay(50);
	GPIOA->BSRR = ( GPIO_BSRR_BS0 );
	glcd_Delay(2);
	
  glcd_Send(0x30, GLDC_Command);    //8 ���, ����������� ����� �������.
	glcd_Send(0x36, GLDC_Command);    //���. �������, ������ ��������, ������ ���������.
  glcd_Send(0x01, GLDC_Command);    //����� ����������� �����������������.
	
	glcd_Delay(150);
}
//

// ������� ������������ ������� (����������)
char glcd_Clear_HW(void)
{
  if(glcd_Send(0x01, GLDC_Command)) return 1;
  glcd_Delay(50);
  return 0;
}
//

// ������� ������������ ������� (�����������)
char glcd_Clear_FW(void)
{
  for (char Loop_ST = 0; Loop_ST<32; Loop_ST++)
  { 
    glcd_Send(0x80|Loop_ST, GLDC_Command);
    for (char Loop1 = 0; Loop1<64; Loop1++)
    { 
      if(glcd_Send(0, GLDC_Data))
    	  return 1;
    }
  }
  return 0;
}
//

// �������� ����������� ����� ������
void glcd_Buffer_Clear(void)
{
  for (int Loop = 0; Loop<1024; Loop++)
  {
    glcd_buff[Loop]=0;
  }
}
//


void show_float(char * ret, float val){

	char *tmpSign = (val < 0) ? "-" : "";
	float tmpVal = (val < 0) ? -val : val;

	int tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = round(tmpFrac * 10000);  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.

	sprintf (ret, "%s%4d.%04d", tmpSign, tmpInt1, tmpInt2);
}

// ����� ������������ ������ ������ �� �������
char glcd_Show(void)
{
	char err_cnt = 0;
  int LoopDataLCD = 0;                       //� ���� ���������� �������� ������� ������. �� 0 �� 1024).
  for (char Stroki = 0; Stroki<64; Stroki++) // ���������� ������ ������ ����.
  {
    if (Stroki<32)                          // ���� ����� ������ ������ 32-� (��������).
    { 
    	err_cnt += glcd_Send(0x80|Stroki, GLDC_Command);      // ������ ��������� � ������ ������ 0..31.
    	err_cnt += glcd_Send(0x80, GLDC_Command);
    }
    else 
    {
    	err_cnt += glcd_Send((0x80|Stroki)-32, GLDC_Command);     // ���� 2-� �������� ������ (������ 31..63.)
    	err_cnt += glcd_Send(0x80+8, GLDC_Command);            // �� ��������� �� 8-� ������ ������.
    }                                        // � �� ������ ������ -32. �.�. ����� �� ����� ���� ����� 32, �� ��� �� �������� ����������.
    for (char Stolb = 0; Stolb<16; Stolb++)  // � ������ ������ ������ �� 16 �����. 
    {      
    	err_cnt += glcd_Send(glcd_buff[LoopDataLCD], GLDC_Data);
    	LoopDataLCD++;
    }
    if(err_cnt) return 1;
  };  
  return 0;
}
//

// ���������� ������ �� ����������� �(�������) � Y(������), ����� �����������
void glcd_Char(uint8_t Symbol, uint8_t X, uint8_t Y, uint8_t type_font)
{
  for (unsigned char Strok_Loop = 0; Strok_Loop < 8; Strok_Loop++)
  { 
    for (unsigned char Stilb = 0; Stilb < 8; Stilb++)
    {
      switch  (type_font) //����� ������ �������� ������? � define ���� ��������.
      {
        case FonOFF_InversOFF: glcd_buff[X+16*Stilb+(Y*16*8)] |=   Font8x8Table[Symbol] [Stilb];  break;
        case FonOFF_InversON:  glcd_buff[X+16*Stilb+(Y*16*8)] &= ~(Font8x8Table[Symbol] [Stilb]); break;
        case FonON_InversOFF:  glcd_buff[X+16*Stilb+(Y*16*8)] =    Font8x8Table[Symbol] [Stilb];  break;
        case FonON_InversON:   glcd_buff[X+16*Stilb+(Y*16*8)] |= ~(Font8x8Table[Symbol] [Stilb]); break;
      }
    }    
  }
}
//

// �������� ������ � ����� ������� �� �������� �����������.
void glcd_String(char String[], uint8_t X, uint8_t Y, uint8_t type_font)
{
  unsigned char Loop_SM = 0;                         //����� �������.
  while (String[Loop_SM] != 0x00)           //���� �� ��������� ���������.
  {   
    switch  (type_font) //����� ������ ������ ��������? � define ���� ��������.
    {
      case FonOFF_InversOFF: glcd_Char( String[Loop_SM], X, Y, FonOFF_InversOFF ); break;
      case FonOFF_InversON:  glcd_Char( String[Loop_SM], X, Y, FonOFF_InversON ); break;
      case FonON_InversOFF:  glcd_Char( String[Loop_SM], X, Y, FonON_InversOFF ); break;
      case FonON_InversON:   glcd_Char( String[Loop_SM], X, Y, FonON_InversON ); break;
    }
    
    X++;                        //�������� �� ����������� ������ ���������.
    if (X > 15)                 //���� �������� �� ����� ������ - �� ��������� ������.
    {
      X = 0; Y++;
      if (Y == 9) {Y=0;};       //���� �������� ��������� ������ - � 0-�.
    }
    Loop_SM++;                  //��������� ������.
  }
}
//

// ���������� ������� � ����������� ������ ������ �(������) � Y(������) �� ������ �������� ����
void glcd_Pixel(int16_t X, int16_t Y, uint8_t State)
{
  // ���������, ��������� �� ����� � ���� ��������� �������
  if ((X >= glcd_Width) || (Y >= glcd_Height) || (X < 0) || (Y < 0))
  return;
  
  uint16_t ByteIdx = Y >> 3;
  uint8_t BitIdx = Y - (ByteIdx << 3); // ������ ������������ ������ ���� (0<=Y<=7)
  ByteIdx *= glcd_Width;  
  ByteIdx += X;
  
  if (State)
    glcd_buff[ByteIdx] |= (1 << BitIdx);
  else
    glcd_buff[ByteIdx] &= ~(1 << BitIdx);
}
//

// ���������� ����� � ����������� ������ ������
void glcd_Line(uint8_t Napr, uint8_t IniLin, uint8_t Dl, uint8_t st)
{ 
  if (Napr == GLDC_HORIZONTAL_LINE)                                   //���� ����� ��������������, ��.
  {//��� �����������: Horiz, �� ��� � ����� ��������, ������ ������, �� ��� y ���������.
    char LoopXn = 0;                                   //������ ������. 
    while (IniLin>7) {IniLin=IniLin-8; LoopXn++;};     //���������, ������� ������ ������ �� ������� ������� �����.      
    if ((8-IniLin) >= Dl)                              //���� ������ ����� ������ ��� ����� ����������� ��������� ����� ������ �������� � 1 �����, �� ������ ��������� ����������. 
    {
      glcd_buff[LoopXn+st*16] |= (0xFF>>IniLin) & (0xFF<<(8-Dl));//���������� ����� ������ 1 �����, ����������� � ���� ������.
        //������: ����� 2          0011 1111 
                                 //1111 1100  ������ 2.                              
    }                           //&0011 1100  "�"
    else
    {                                                 //����� �������.
      if (Dl <=((8-LoopXn)+8)  )                      //���� ����� ������� (�� �������) � 2 ����� �����, �� ���������� ��.
      {
        glcd_buff[LoopXn+st*16]   |= 0xFF>>IniLin;     //���������� ��������� ������ ���� (���� ���� �����.).
        glcd_buff[LoopXn+st*16+1] |= 0xFF<<(8 - (Dl-(8-IniLin)));  //������ ����������, ����� ����� ���� ������.
      }
      else //���� ����� �������� 3 � ����� �����, ��... 
      {    
        glcd_buff[LoopXn+st*16] |= 0xFF>>IniLin;           //������ �� ����� ����� ����� (�� ������� 8) �����.
        Dl = Dl - (8-IniLin);                             //���������� ��� >2� ���.
        char LoopXb = 0;                                  //������� ������.
        while (Dl>7) {LoopXb++; Dl=Dl-8;};                //��������� ����������� ������ ��������.
        char ZPDLSM = LoopXb;
        while (LoopXb!=0x00)                              //�������� ��������� ����������� �����.
        {
          glcd_buff[LoopXn+st*16+LoopXb] = 0xFF;
          LoopXb--;
        }
        if (Dl != 0) glcd_buff[LoopXn+st*16+ZPDLSM+1] |= 0xFF<<(8-Dl);  //������ �� �� ����� ����������� �����.
      };
    }; 
  }
  else    //���� ���������: Vert, ������ �� ��� Y ��������, ������ ����, ��������� �). 
  {
    char LoopXn = 0;                                   //������ ������ ����� �� ������� �� ��������� ����� (���� 00010000).
    while (st>7) {st=st-8; LoopXn++;};                 //���������, ������� ������ ������ �� ������� ������� ����� �� ��� �.
    char Line_const = (1<<7)>>st;                      //������� �����.
    for (char Ris = 0; Ris != Dl; Ris++)
    {
      glcd_buff[LoopXn+IniLin*16+Ris*16] |= Line_const;   
    }
  }
}
//

// ����������� ������� ���� �� ������
void Menu_Cursor(uint8_t menu)
{
	uint8_t cursor_x = 0, cursor_y = 0;

	switch(menu)
		  {
				case 0: { cursor_x = 15;
					        cursor_y = 1;
					        break;
					      }
				
				case 1: { cursor_x = 15;
					        cursor_y = 5;
					        break;
					      }
				
				case 2: { cursor_x = 15;
					        cursor_y = 2;
					        break;
					      }
				
				case 7: { cursor_x = 15;
					        cursor_y = 2;
					        break;
					      }
				default: break;
			}
			glcd_Char(17, cursor_x, cursor_y, FonON_InversOFF);
}
//

// ����� "��������" ����� �� ����� (�������� 2 �����).
void glcd_Float(uint32_t dig, char temp[])
{
	if (dig/10000) { temp[0] = (dig/10000)+48; } else { temp[0] = ' '; }
	if((dig/10000)||(dig>=1000)) { temp[1] = ((dig%10000)/1000)+48; } else { temp[1] = ' '; } 
  temp[2] = ((dig%1000)/100)+48;
  temp[3] = '.';
	temp[4] = ((dig%100)/10)+48;
	temp[5] = (dig%10)+48;
}
//
