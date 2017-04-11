#define	true	1
#define false	0
#define	NB_SHOTS	5
#define STEP_MAX	1
#define true 1
typedef int bool;
typedef int type;
#define circleRad 69
#define cross 5
#define circle 6
#define height 64
#define lenght 128
#define crossLenght 14
#define horizontal 1
#define vertical 2
#define crossUP 3
#define crossDOWN 4
#define false 0
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
 struct Positions {
   int  crosses[50];
   int crossesIndex;
   int  circles[50];
   int circlesIndex;

};

void drawLine(int x0,int y0,int lenghtL,int dir)
{
	for(int i = 0;i<lenghtL;i++)
	{
		if(dir == horizontal)
		{
			y0++;
		}
		if(dir == vertical)
		{
			x0++;
		}
		if(dir == crossUP)
		{
			y0--;
			x0++;
		}
		if(dir == crossDOWN)
		{
			y0++;
			x0++;
		}
		lcd_pixel(x0,y0);
	}
}
void drawCross(int x,int y)
{
	drawLine(x,y,crossLenght,crossDOWN);
	drawLine(x,y+15,crossLenght,crossUP);
}
void drawGrid()
{
	int sectorY = height/3;	
	int sectorX = lenght/3;

	drawLine(0,sectorY,lenght,vertical);
	drawLine(0,sectorY*2,lenght,vertical);
	drawLine(sectorX,0,lenght,horizontal);
	drawLine(sectorX*2,0,lenght,horizontal);
}
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
void moveCursor(int x, int y,type _type)
{
	if (_type == cross)
	{
		drawCross(x,y);
	}
	else if(_type == circle)
	{
		drawCirc(x,y,circleRad);
	}
};

int main(void)
{
	// IO-port INIT
	DDRA = 0xE0;
 
	// LCD - Display Init
	lcd_init();
	
	lcd_clear();

	drawGrid();
	drawCross(50,43);
}
