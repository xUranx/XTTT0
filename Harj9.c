#define	true	1
#define false	0
#define	NB_SHOTS	5
#define STEP_MAX	1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "lcd.h"


//typedef char bool;
/********************************
 main 

 *******************************/
void drawBox(int x0, int y0, int x1, int y1)
{
	int p;
	int tempVal;
	
	if (x0 > x1)
	{
		tempVal = x0;
		x0 = x1;
		x1 = tempVal;
	}
	
	if (y0 > y1)
	{
		tempVal = y0;
		y0 = y1;
		y1 = tempVal;
	}
	
	for (p = x0; p <= x1; p++)
	{
		lcd_pixel(p,y0);
		lcd_pixel(p,y1);
	}
	for (p = y0; p <= y1; p++)
	{
		lcd_pixel(x0,p);
		lcd_pixel(x1,p);
	}
}

void drawCirc(int x0, int y0, int radius)
{
	int f = 1 - radius;
	  int ddF_x = 1;
	  int ddF_y = -2 * radius;
	  int x = 0;
	  int y = radius;
	 
	  lcd_pixel(x0, y0 + radius);
	  lcd_pixel(x0, y0 - radius);
	  lcd_pixel(x0 + radius, y0);
	  lcd_pixel(x0 - radius, y0);
	 
	  while(x < y)
	  {
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if(f >= 0) 
		{
		  y--;
		  ddF_y += 2;
		  f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;    
		lcd_pixel(x0 + x, y0 + y);
		lcd_pixel(x0 - x, y0 + y);
		lcd_pixel(x0 + x, y0 - y);
		lcd_pixel(x0 - x, y0 - y);
		lcd_pixel(x0 + y, y0 + x);
		lcd_pixel(x0 - y, y0 + x);
		lcd_pixel(x0 + y, y0 - x);
		lcd_pixel(x0 - y, y0 - x);
	  }
}

int main(void)
{
	// IO-port INIT
	DDRA = 0xE0;
 
	// LCD - Display Init
	lcd_init();
	
	lcd_clear();
	
	// Piirtää näytölle neliön pikseli kerrallaan
	lcd_pixel(20,10);
	lcd_pixel(20,11);
			
	lcd_pixel(20,18);
	
	lcd_pixel(30,10);
	lcd_pixel(30,11);
		
	lcd_pixel(30,18);
	
	
	// Piirtää näytölle ympyrän 

	drawCirc(40, 40, 10);
 	drawBox(10,10,20,20);

	
	// Testataan viivan piirtoa
	//lcd_Line_vert(40, 40, 50); // proto Fungtio 
	//lcd_Line_Hor(5, 5, 20);
}
