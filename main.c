/** @author Levente Kurusa <levex@linux.com> **/

#if !defined(__cplusplus)
#include <stdbool.h> /* C-ben alapbol nincsenek is bool-ok. */
#endif
#include <stddef.h>
#include <stdint.h>
 
#include "include/string.h"
#include "include/display.h"
#include "display/textmode/dispi_textmode.h"
#include "include/x86/gdt.h"
#include "include/x86/idt.h"
#include "include/pit.h"
#include "include/pic.h"
#include "include/hal.h"
#include "include/tasking.h"
#include "include/levos.h"
#include "include/keyboard.h"

static DISPLAY* disp = 0;

MODULE("MAIN");

void _test();

extern void kernel_end;
extern void kernel_base;

#if defined(__cplusplus)
extern "C" /* ha C++ compiler, akkor ez a függvény legyen C99es linkage-ű. */
#endif
void kernel_main()
{
	int eax;
	asm("movl %%eax, %0": "=m"(eax));
	/* We create a textmode display driver, register it, then set it as current */
	display_setcurrent(display_register(textmode_init()));
	disp = display_getcurrent();
	mprint("Welcome to LevOS 4.0, very unstable!\n");
	mprint("Kernel base is 0x%x,  end is 0x%x\n", &kernel_base, &kernel_end);
	if(eax == 0x1337) { mprint("Assembly link established.\n"); }
	else panic("Couldn't establish assembly link!");
	/* Setup memory manager */
	mm_init(&kernel_end);
	/* So far good, we have a display running,
	 * but we are *very* early, set up arch-specific
	 * tables, timers and memory to continue to tasking.
	 */
	gdt_init();
	/* Now, we have the GDT setup, let's load the IDT as well */
	idt_init();
	/* Next step, setup PIT. */
	hal_init();
	pic_init();
	pit_init();
	/* Enabling interrupts early... */
	asm volatile("sti");
	mprint("Interrupts were enabled, beware.\n");
	/* Setup paging. */
	paging_init();
	/* Good job ladies and gentleman, we are now alive.
	 * Safe and sound, on the way to tasking!
	 * Let's roll.
	 */
	tasking_init();
	/* Tasking should have started late_init(),
	 * if not, we are screwed as no peripherials are in
	 * an operating state. Tell the user that we ran away
	 * in terror.
	 */
	panic("Reached end of main(), but tasking was not started.");
	for(;;);
}

/* This function is ought to setup peripherials and such,
 * while also starting somekind of /bin/sh
 */
void late_init()
{
	/* From now, we are preemptible. Setup peripherials */
	START("kbd_init", keyboard_init);
	START("_test", _test);
	/* We cannot die as we are the idle thread.
	 * schedule away so that we don't starve others
	 */
	while(1) schedule_noirq();
	panic("Reached end of late_init()\n");
}
static char c = 0;
static char* buffer = 0;
static uint16_t loc = 0;
void _test()
{
	buffer = (char*)malloc(256);
	kprintf("Welcome to LevOS 4.0\nThis is a very basic terminal.\nDon't do anything stupid.\n");
prompt:
	kprintf("\n(kernel) $ ");
	while(1) {
		if(!keyboard_enabled()){ schedule_noirq(); continue; }
		c = keyboard_get_key();
		if(!c) continue;
		if(c == '\n')
		{
			buffer[loc] = 0;
			loc = 0;
			if(strcmp(buffer, "help") == 0)
			{
				kprintf("\nLevOS4.0\nThis is the kernel terminal.\nDon't do anything stupid.");
				kprintf("\nCommands available: help; reboot");
			}
			if(strcmp(buffer, "reboot") == 0)
			{
				outportb(0x64, 0xFE);
			}
			goto prompt;
		}
		buffer[loc++] = c;
		buffer[loc] = 0;
		disp->putc(c);
	}
}
