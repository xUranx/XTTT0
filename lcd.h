/*********************************************************
** lcd.h
**
** LCD driver
** Target: ATMEGA :: AVR-GCC
**
** 
** Kajak.fi
***********************************************************/
#ifndef _NOKIALCD_H_
#define _NOKIALCD_H_

/******************* Macros & define *******************/
#define F_CPU 1000000UL

/**************** Functions prototypes *****************/
//short ReadADC(unsigned char);



/* Lcd screen size */
#define LCD_X_RES 84
#define LCD_Y_RES 48
#define LCD_CACHE_SIZE ((LCD_X_RES * LCD_Y_RES) / 8)

/* Pinout for LCD */
#define LCD_CLK_PIN 	(1<<PC4)
#define LCD_DATA_PIN 	(1<<PC3)
#define LCD_DC_PIN 		(1<<PC2)
#define LCD_CE_PIN 		(1<<PC1)
#define LCD_RST_PIN 	(1<<PC0)
#define LCD_PORT		PORTC
#define LCD_DDR			DDRC

/* Special Chars */
#define ARROW_RIGHT	ICON(0)
#define ARROW_LEFT 	ICON(1)
#define ARROW_UP 	ICON(2)
#define ARROW_DOWN 	ICON(3)
#define STOP 		ICON(4)
#define PLAY	 	ICON(5)

/* Function for my special characters */
#define	ICON(x)		'z'+1+x

void lcd_init(void);
void lcd_contrast(unsigned char contrast);
void lcd_clear(void);
void lcd_clear_line(unsigned char line);
void lcd_goto_xy(unsigned char x, unsigned char y);
void lcd_goto_xy_exact(unsigned char x, unsigned char y);
void lcd_chr(char chr);
void lcd_str(char* str);
void lcd_pixel(int x,int y);
void lcd_Line_vert(int x_Start, int y_Start, int Lenght);
void lcd_Line_Hor(int x_Start, int y_Start, int Lenght);
void lcd_clear_obj(uint8_t *tex);
void lcd_clear_area(int x, int y, int x0, int y0);
#endif



