/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/hal.h"
#include "../include/display.h"
#include "../include/pic.h"
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
	asm volatile("push %eax");
	asm volatile("push %edx");
	asm volatile("movw %0, %%dx":"=a"(portid));
	asm volatile("in %dx, %al");
	asm volatile("movb %%al, %0":"=a"(ret));
	asm volatile("pop %edx");
	asm volatile("pop %eax");
	return ret;
}
void outportb(uint16_t portid, uint8_t value)
{
	asm volatile("push %eax");
	asm volatile("push %edx");
	asm volatile("mov %0, %%al":"=a"(value));
	asm volatile("mov %0, %%dx":"=a"(portid));
	asm volatile("out %al, %dx");
	asm volatile("pop %edx");
	asm volatile("pop %eax");
}
