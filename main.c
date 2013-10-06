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

static DISPLAY* disp = 0;

MODULE("MAIN");

extern void kernel_end;
extern void kernel_base;

#if defined(__cplusplus)
extern "C" /* ha C++ compiler, akkor ez a függvény legyen C99es linkage-ű. */
#endif
void kernel_main()
{
	int eax;
	asm("movl %%eax, %0": "=a"(eax));
	/* We create a textmode display driver, register it, then set it as current */
	display_setcurrent(display_register(textmode_init()));
	mprint("Welcome to LevOS 4.0, very unstable!\n");
	mprint("Kernel base is 0x%x,  end is 0x%x\n", &kernel_base, &kernel_end);
	if(eax == 0x1337) mprint("Assembly link established.\n");
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
	/* Enable tasking. */
	tasking_init();
	#ifdef __cplusplus
	mprint("C++ version, may cause malfunction!\n");
	#endif
	panic("Reached end of main(), but no init was started.");
	for(;;);
}

void _test()
{
	mprint("Unhandled IRQ\n");
}
