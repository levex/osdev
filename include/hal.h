/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __HAL_H_
#define __HAL_H_

#include <stdint.h>

#define INT_START asm volatile("pusha");
#define INT_END asm volatile("popa"); \
	asm volatile("iret");

#define IRQ_START asm volatile("add $12, %esp");\
	asm volatile("pushal");

#define IRQ_END asm volatile("popal"); \
	asm volatile("iretl");
extern void hal_init();

extern void send_eoi(uint8_t irq);

extern void set_int(int i, uint32_t addr);

extern uint8_t inportb(uint16_t portid);
extern void outportb(uint16_t portid, uint8_t value);
#endif
