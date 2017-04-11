/******************************************************************************
** lcd.c
**
** N�yt�n kirjastot
** 
** Target: ATMEGA128 :: AVR-GCC
**
** 
** 29.09 2008 Kajak.fi
** 23.10 2012 Lis�tty piirto funktioita MrK
******************************************************************************/
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

/* delay function */
#define lcd_delay() for(int i=-32000;i<32000;i++)

/* Command type sent to the lcd */
typedef enum { LCD_CMD  = 0, LCD_DATA = 1 } LcdCmdData;

/* Function prototypes */
void lcd_base_addr(void);
void lcd_send(unsigned char data, LcdCmdData cd);

/* The lcd cursor position */
int lcdCacheIdx;
int lcdCacheIdy;

int lcdCacheData[128][8];

/* Alphabet lookup */
unsigned char PROGMEM font5x7 [][5] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
	{ 0x32, 0x34, 0x08, 0x16, 0x26 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [ 60
    { 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
    { 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z 90
    { 0x00, 0x7F, 0x3E, 0x1C, 0x08 },   // > Filled
	{ 0x08, 0x1C, 0x3E, 0x7F, 0x00 }, 	// < Filled
	{ 0x08, 0x7C, 0x7E, 0x7C, 0x08 },   // Arrow up
	{ 0x10, 0x3E, 0x7E, 0x3E, 0x10 },   // Arrow down	
	{ 0x3E, 0x3E, 0x3E, 0x3E, 0x3E },   // Stop
	{ 0x00, 0x7F, 0x3E, 0x1C, 0x08 },	// Play

	{ 0x0E, 0x11, 0x11, 0x11, 0x0E },   // ball
	{ 0x7F, 0x00, 0x00, 0x00, 0x00 }	// laser beam
};

/************************************************************ 
N�yt�n alustus 
* 12.8.-14 MrK 
* Korjattu Batron COG2 n�yt�lle


Performs IO & LCD controller initialization 
*************************************************************/
void lcd_init(void)
{
	
	// Pull-up on reset pin
    LCD_PORT |= LCD_RST_PIN;	// Reset = 1
	
	// Set output bits on lcd port
	LCD_DDR |= LCD_RST_PIN | LCD_CE_PIN | LCD_DC_PIN | LCD_DATA_PIN | LCD_CLK_PIN;
	// LCD_DDR DDRC ei toimimmm

	// Wait after VCC high for reset (max 30ms)
    _delay_ms(15);
    
    // Toggle display reset pin
    LCD_PORT &= ~LCD_RST_PIN; // Reset = 0
	_delay_ms(15);
    LCD_PORT |= LCD_RST_PIN;	// Reset = 1

	_delay_ms(15);

    // Disable LCD controller
    LCD_PORT |= LCD_CE_PIN;

    lcd_send(0xEB, LCD_CMD);  	// LCD bias 
    lcd_send(0x23, LCD_CMD);  	// Set Lines >> 23 = 64
    lcd_send(0x81, LCD_CMD);	// Set Potentiometer
	lcd_send(0x64, LCD_CMD);	// 16 >> 64 (Tummuus)
	lcd_send(0xAF, LCD_CMD);  	// Set Display ON
    lcd_send(0xCC, LCD_CMD);  	// Set LCD to RAM mapping
	
	//lcd_send(0xFF, LCD_DATA);	// 
	//lcd_send(0x00, LCD_DATA);  	// Set Display ON
    //lcd_send(0xFF, LCD_DATA);
	
	
	
	//cd_send(0xA5, LCD_CMD);  // All pixel ON TESTI�
    
    // Clear lcd
    lcd_clear();
	
	// For using printf
	//fdevopen(lcd_chr, 0);
}

/*********************************************************************
Kontrastin s��t�


 
**********************************************************************/
void lcd_contrast(unsigned char contrast)
{
	lcd_send(0x21, LCD_CMD);				// LCD Extended Commands
    lcd_send(0x80 | contrast, LCD_CMD);		// Set LCD Vop(Contrast)
    lcd_send(0x20, LCD_CMD);				// LCD std cmds, hori addr mode
}

/*****************************************
N�yt�n tyhjennys


 
******************************************/
void lcd_clear(void)
{
int j,i;

	for(j=0;j<8;j++){
		lcd_send(0xB0|j, LCD_CMD);	// page j
		lcd_send(0x10, LCD_CMD);
		lcd_send(0x00, LCD_CMD);	// column 0
				

		for(i=0;i<128;i++){
			lcd_send(0x00, LCD_DATA);
			lcdCacheData[i][j]=0;		
			}			
	}    

	lcd_send(0xB0, LCD_CMD);	// page 0
	lcd_send(0x10, LCD_CMD);
	lcd_send(0x00, LCD_CMD);	// column 0

	lcdCacheIdx = 0;
	lcdCacheIdy = 0;
}

/**********************************************
Sets cursor location to exact xy pixel location on the lcd 

***********************************************/
void lcd_goto_xy(unsigned char x, unsigned char y)
{
    lcdCacheIdx = x*6;
	lcdCacheIdy = y;
	lcd_base_addr();
}

/**********************************************
Sets cursor location to exact xy pixel location on the lcd 

***********************************************/
void lcd_goto_xy_exact(unsigned char x, unsigned char y)
{

    lcdCacheIdx = x;
	lcdCacheIdy = y;
	lcd_base_addr();
}

/**********************************************
Displays a character at current cursor location 

**********************************************/
void lcd_chr(char chr)
{
	lcd_base_addr();

    // 5 pixel wide characters and add space
    for(unsigned char i=0;i<5;i++) {
		lcd_send(pgm_read_byte(&font5x7[chr-32][i]) << 1, LCD_DATA);
    }
	lcd_send(0, LCD_DATA);
	
	lcdCacheIdx += 6;
}


/**********************************************
pixel _ On 

**********************************************/
void lcd_pixel(int x,int y)
{
	//lcd_base_addr();

    // pixel wide characters and add space
    
	//lcd_send(1, LCD_DATA);
    //lcd_send(0, LCD_DATA);
	
	//lcdCacheIdx += 1;

	int j,i;
	int i_Page;
	int y_line = 0;
	int merkki = 1;

	i_Page = y/8; 	//page numero 0...7
	y_line = y%8;		//Rivinumero
	
	i = y_line;
	//for(j=0;j<7;j++){
	while(i > 0)
	{ // T�ss� virhe tekee yhen liikaa
		//merkki = merkki + 1;					
		
		merkki = merkki << 1;
		i--;
		
		}
	merkki=merkki|lcdCacheData[x][i_Page];
	//merkki = merkki << 1; // Korjaa yhde liika merkin

	/*
	for(j=i_Page;j<8;j++){
		lcd_send(0xB0|j, LCD_CMD);	// page j
		lcd_send(0x10, LCD_CMD);
		lcd_send(0x00, LCD_CMD);	// column 0

	}*/

	lcd_goto_xy_exact(x,i_Page);
	lcd_send(merkki, LCD_DATA);
	lcdCacheData[x][i_Page]=merkki;
}



/***********************************************

Displays string at current cursor location and increment cursor location 
***********************************************/
void lcd_str(char *str)
{
    while(*str) {
        lcd_chr(*str++);
    }
}

// Set the base address of the lcd
void lcd_base_addr(void) {

	lcd_send(0xB0 | lcdCacheIdy, LCD_CMD);	// page

	lcd_send(0x00 | (lcdCacheIdx & 0x0F), LCD_CMD);
	lcd_send(0x10 | ((lcdCacheIdx & 0xF0)>>4), LCD_CMD);	// column
	
}

/************************************************ 

Sends data to display controller 
cd data / controll
*************************************************/
void lcd_send(unsigned char data, LcdCmdData cd)
{
int j;
	// Data/DC are outputs for the lcd (all low)
	LCD_DDR |= LCD_DATA_PIN | LCD_DC_PIN;
	
    // Enable display controller (active low)
    LCD_PORT &= ~LCD_CE_PIN;

    // Either command or data
    if(cd == LCD_DATA) {
        LCD_PORT |= LCD_DC_PIN;
    } else {
        LCD_PORT &= ~LCD_DC_PIN;
    }
	
	for(unsigned char i=0;i<8;i++) {
	
		// Set the DATA pin value
		if((data>>(7-i)) & 0x01) {
			LCD_PORT |= LCD_DATA_PIN;
		} else {
			LCD_PORT &= ~LCD_DATA_PIN;
		}
		
		// Toggle the clock
		LCD_PORT |= LCD_CLK_PIN;
		for(j=0;j<4;j++); // lis�tty pient� viivett�
		LCD_PORT &= ~LCD_CLK_PIN;
	}

	// Disable display controller
	//LCD_PORT &= ~LCD_DC_PIN;
    LCD_PORT |= LCD_CE_PIN;
	
	// Data/DC can be used as button inputs when not sending to LCD (/w pullups)
	LCD_DDR &= ~(LCD_DATA_PIN | LCD_DC_PIN);
	LCD_PORT |= LCD_DATA_PIN | LCD_DC_PIN;
}

/************************************************ 
Vaaka viivan piirto 


Kajak/MarkkuK 23.10-12

Vaatii testausta proto!
*************************************************/
void lcd_Line_vert(int x_Start, int y_Start, int Lenght)
{
int j;
int i_Page;
int y_line = 0;

	i_Page = y_Start/7; 	//page numero 0...7
	y_line = y_Start%7;	//Rivinumero

	if(y_line == 0){
		y_line = 1;
	}
	else{
		j=1;
		j=j<<y_line;
		y_line = j;				
	}	
	//lcd_goto_xy_exact((unsigned char) x_Start-1,(unsigned char) i_Page);
	
	lcd_send(0xB0|i_Page, LCD_CMD);	// Sivu(page) = i_Page
	lcd_send(0x10, LCD_CMD);
 	lcd_send(x_Start, LCD_CMD);	// Sarake(column) 
	
	for(j=0;j<Lenght;j++)	
			lcd_send(y_line, LCD_DATA);
}

/************************************************ 
Pysty viivan piirto 
Kajak/MarkkuK 30.10-12

Ei toiminnassa Vaatii koodausta!
*************************************************/
void lcd_Line_Hor(int x_Start, int y_Start, int Lenght)
{
int j,i;
int i_Page;
int y_line = 0;
int merkki = 0;


	i_Page = y_Start/7; 	//page numero 0...7
	y_line = y_Start%7;		//Rivinumero
	
	i = y_line;
	for(j=0;j<7;j++){
			if(i >= 0 ){ // T�ss� virhe tekee yhen liikaa
				merkki = merkki + 1;					
			}
			merkki = merkki << 1;
			i--;
		
		}
	merkki = merkki << 1; // Korjaa yhde liika merkin

	while(Lenght != 0){
		for(j=i_Page;j<8;j++){
			lcd_send(0xB0|j, LCD_CMD);	// page j
			lcd_send(0x10, LCD_CMD);
			lcd_send(0x00, LCD_CMD);	// column 0

			for(i=0;i<128;i++){
				if(i == x_Start){
					lcd_send(merkki, LCD_DATA); // Pystyviiva
					i = 129;
					j = 10; // Viivatulostettu
					Lenght = Lenght - y_line;
					i_Page++; // Vaihda sivua
					merkki = 0xFF;
					
				}
				else lcd_send(0x00, LCD_DATA); // Plankkoviiva
			}
		}
	}
}
/*	while(Lenght!=0){				
		
		for(j=0;j<7;j++){
			if(y_line >= 0 ){ // T�ss� virhe tekee yhen liikaa
				merkki = merkki + 1;					
			}
			merkki = merkki << 1;
			y_line--;
		
		}	
		
		lcd_goto_xy('4', '0');
										
		lcd_send(0xB0|i_Page, LCD_CMD);	// Sivu(page) = i_Page
		lcd_send(0x10, LCD_CMD);
	 	lcd_send(0x00, LCD_CMD);	// Sarake(column) 
	
		lcd_send(0xF0, LCD_DATA);
		
		Lenght--;
	}*/


/*	for(j=0;j<8;j++){
		lcd_send(0xB0|j, LCD_CMD);	// page j
		lcd_send(0x10, LCD_CMD);
		lcd_send(0x00, LCD_CMD);	// column 0

		for(i=0;i<128;i++)	
			lcd_send(0x00, LCD_DATA);
*/
