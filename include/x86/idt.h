/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __IDT_H_
#define __IDT_H_

typedef struct {
	uint16_t offset_0_15;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_16_31;
} __attribute__((packed)) idt_descriptor;

#define IDT_32BIT_INTERRUPT_GATE	0xE
#define IDT_STORAGE_SEGMENT		0x20
#define IDT_DPL_3			0x60
#define IDT_PRESENT			0x80

#define INT_START asm volatile("pusha");
#define INT_END asm volatile("popa"); \
	asm volatile("iret");

extern void idt_init();
extern void idt_register_interrupt(uint8_t i, uint32_t addr);
extern void add_idt_descriptor(uint8_t id, idt_descriptor desc);

#endif
