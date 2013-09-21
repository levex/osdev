#if !defined(__cplusplus)
#include <stdbool.h> /* C-ben alapbol nincsenek is bool-ok. */
#endif
#include <stddef.h>
#include <stdint.h>
 
#include "include/string.h"
#include "include/display.h"
#include "display/textmode/dispi_textmode.h"

static DISPLAY* disp = 0;

#if defined(__cplusplus)
extern "C" /* ha C++ compiler, akkor ez a függvény legyen C99es linkage-ű. */
#endif
void kernel_main()
{
	/* We create a textmode display driver, register it, then set it as current */
	display_setcurrent(display_register(textmode_init()));
	disp = display_getcurrent();
	disp->puts("Hello, world! I am a kernel developer, \nand I think that newlines do work! Not sure, but hey!\n");
	kprintf("Testing printf\n%s %s\n%c %s\n%d %s\n0x%x %s", "it does work!", "<--- a string", 'D', "<-- a character", 1337, " <- a decimal value", 1337, " <-- hexadecimal value");

}
