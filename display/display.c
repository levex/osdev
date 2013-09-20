#include <stdint.h>
#include <stddef.h>
#include "../include/display.h"

static DISPLAY dispis[DISPLAY_MAX_DISPIS];
static uint8_t _last_register = 1;
static uint8_t current = 0;


/* Registers the DISPLAY and returns its ID */
uint8_t display_register(DISPLAY d)
{
	dispis[_last_register] = d;
	dispis[_last_register].onregister();
	return _last_register++;
}
/* Sets it as current display, calling d->onset(id) to let the display
   set itself up */
uint8_t display_setcurrent(uint8_t id)
{
	if(current == id) return 0;
	current = id;
	dispis[current].onset(id);
	return 1;
}
/* returns current DISPLAY */
DISPLAY* display_getcurrent()
{
	return &dispis[current];
}

