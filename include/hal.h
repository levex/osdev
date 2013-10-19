/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __HAL_H_
#define __HAL_H_

#include <stdint.h>

#define INT_START asm volatile("pusha");
#define INT_END asm volatile("popa"); \
	asm volatile("iret");



#define IRQ_START asm volatile("add $0x1c, %esp"); \
		asm volatile("pusha");

#define IRQ_END asm volatile("popa"); \
	asm volatile("iret");


#define DEFINE_IRQ(irqno, name, functionname, functocall) void functionname() { \
	IRQ_START; \
	send_eoi(irqno); \
	addProcess(createProcess(name, functocall)); IRQ_END;}

#define START(name, addr) addProcess(createProcess(name, addr));

#define START_AND_WAIT(name, addr) int __pid_name = START(name, addr); \
				while(is_pid_running(__pid_name))schedule_noirq();

extern void hal_init();

extern void send_eoi(uint8_t irq);

extern void set_int(int i, uint32_t addr);

extern uint8_t inportb(uint16_t portid);
extern void outportb(uint16_t portid, uint8_t value);
#endif
