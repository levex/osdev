#ifndef __DISPLAY_H_
#define __DISPLAY_H_
#include "../include/string.h"
#include "../include/console.h"
#define DISPLAY_MAX_DISPIS 8

typedef struct {
	/* basic props */
	uint32_t width;
	uint32_t height;
	CONSOLE con;
	/* events */
	void (*onregister)();
	void (*onset)(uint8_t id);
	/* libc crap */
	void (*puts)(string);
	void (*putc)(char);
	void (*clear)();
} DISPLAY;

extern uint8_t display_register(DISPLAY d);
extern uint8_t display_setcurrent(uint8_t id);
extern DISPLAY* display_getcurrent();

#endif
