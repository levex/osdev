#ifndef __DISPI_TEXTMODE_H_
#define __DISPI_TEXTMODE_H_

#include "../../include/display.h"

#define COLOR_BLACK 0x00
#define COLOR_WHITE 0x0F

#define VGA_MEMORY 0xB8000
#define TEXTMODE_WIDTH 80
#define TEXTMODE_HEIGHT 26

extern DISPLAY textmode_init();
extern void textmode_puts(string s);
extern void textmode_putc(char c);
extern void textmode_clear();
#endif
