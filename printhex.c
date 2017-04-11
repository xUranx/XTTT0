#include "lcd.h"
#include <stdio.h>
#include <avr/io.h>
#include "printhex.h"

void CalcHex(uint8_t value, int *bint)
{
	if (value - 0x80 >= 0x00)
	{
		bint[0] = 1;
		value -= 0x80;
	}
	if (value - 0x40 >= 0x00)
	{
		bint[1] = 1;
		value -= 0x40;
	}
	if (value - 0x20 >= 0x00)
	{
		bint[2] = 1;
		value -= 0x20;
	}
	if (value - 0x10 >= 0x00)
	{
		bint[3] = 1;
		value -= 0x10;
	}
	if (value - 0x08 >= 0x00)
	{
		bint[4] = 1;
		value -= 0x08;
	}
	if (value - 0x04 >= 0x00)
	{
		bint[5] = 1;
		value -= 0x04;
	}
	if (value - 0x02 >= 0x00)
	{
		bint[6] = 1;
		value -= 0x02;
	}
	if (value - 0x01 >= 0x00)
	{
		bint[7] = 1;
		value -= 0x01;
	}
}
