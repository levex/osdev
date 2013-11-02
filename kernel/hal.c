/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/hal.h"
#include "../include/display.h"
#include "../include/pic.h"
#include "../include/tasking.h"
#include "../include/x86/idt.h"

#include <stdint.h>

MODULE("HAL");

void hal_init()
{
	mprint("Init done.\n");
}

void send_eoi(uint8_t irq)
{
	pic_send_eoi(irq);
}

void set_int(int i, uint32_t addr)
{
	mprint("Installing INT#%d to address: 0x%x\n", i, addr);
	idt_register_interrupt(i, addr);
}

uint8_t inportb(uint16_t portid)
{
	uint8_t ret;
	asm volatile("inb %%dx, %%al":"=a"(ret):"d"(portid));
	return ret;
}
uint16_t inportw(uint16_t portid)
{
	uint16_t ret;
	asm volatile("inw %%dx, %%ax":"=a"(ret):"d"(portid));
	return ret;
}
void outportb(uint16_t portid, uint8_t value)
{
	asm volatile("outb %%al, %%dx": :"d" (portid), "a" (value));
}
