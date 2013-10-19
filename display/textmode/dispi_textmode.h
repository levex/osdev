#ifndef __DISPI_TEXTMODE_H_
#define __DISPI_TEXTMODE_H_

#include "../../include/display.h"

#define	COLOR_BLACK 0
#define	COLOR_BLUE 1
#define	COLOR_GREEN 2
#define	COLOR_CYAN 3
#define	COLOR_RED 4
#define	COLOR_MAGENTA 5
#define	COLOR_BROWN 6
#define	COLOR_LIGHT_GREY 7
#define	COLOR_DARK_GREY 8
#define	COLOR_LIGHT_BLUE 9
#define	COLOR_LIGHT_GREEN 10
#define	COLOR_LIGHT_CYAN 11
#define	COLOR_LIGHT_RED 12
#define	COLOR_LIGHT_MAGENTA 13
#define	COLOR_LIGHT_BROWN 14
#define	COLOR_WHITE 15

#define VGA_MEMORY 0xB8000
#define TEXTMODE_WIDTH 80
#define TEXTMODE_HEIGHT 26

extern DISPLAY *textmode_init();
extern void textmode_puts(string s);
extern void textmode_putc(char c);
extern void textmode_clear();
#endif
