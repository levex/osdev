/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/x86/idt.h"
#include "../include/display.h"

MODULE("IDT");

static uint32_t idt_location = 0;
static uint32_t idtr_location = 0;
static uint16_t idt_size = 0x800;

static uint8_t test_success = 0;
static uint32_t test_timeout = 0x1000;

void __idt_default_handler();
void __idt_test_handler();

void idt_init()
{
	idt_location = 0x402000;
	mprint("Location: 0x%x\n", idt_location);
	idtr_location = 0x401F00;
	mprint("IDTR location: 0x%x\n", idtr_location);
	for(uint8_t i = 0; i < 255; i++)
	{
		idt_register_interrupt(i, __idt_default_handler);
	}
	idt_register_interrupt(0x2f, __idt_test_handler);
	mprint("Registered all interrupts to default handler.\n");
	/* create IDTR now */
	*(uint16_t*)idtr_location = idt_size - 1;
	*(uint32_t*)(idtr_location + 2) = idt_location;
	mprint("IDTR.size = 0x%x IDTR.offset = 0x%x\n", *(uint16_t*)idtr_location, *(uint32_t*)(idtr_location + 2));
	_set_idtr();
	mprint("IDTR set, testing link.\n");
	asm volatile("int $0x2f");
	while(test_timeout != 0)
	{
		if(test_success != 0)
		{
			mprint("Test succeeded, disabling INT#0x2F\n");
			idt_register_interrupt(0x2F, __idt_default_handler);
			break;
		}
	}
	if(!test_success)
		panic("Link test failed.");
	return;
}

void __idt_test_handler()
{
	INT_START;
	test_success = 1;
	INT_END;
}

void __idt_default_handler()
{
	panic("Unhandled interrupt!\n");
}

void idt_register_interrupt(uint8_t i, void(*callback)())
{
	idt_descriptor desc = {0};
	desc.offset_0_15 = (uint16_t)callback;
	desc.selector = 0x8;
	desc.zero = 0;
	desc.type_attr = 0 | IDT_32BIT_INTERRUPT_GATE | IDT_DPL_3 | IDT_PRESENT;
	desc.offset_16_31 = (uint16_t)((uint32_t)callback >> 16);
	/*mprint("Descriptor: id%d offset 0x%x, orig 0x%x\n", 
		i,
		desc.offset_16_31 << 16 | desc.offset_0_15,
		(uint32_t)callback);*/
	add_idt_descriptor(i, desc);
}

void add_idt_descriptor(uint8_t id, idt_descriptor desc)
{
	*(uint16_t*)(idt_location + sizeof(idt_descriptor)*id + 0) = desc.offset_0_15;
	*(uint16_t*)(idt_location + sizeof(idt_descriptor)*id + 2) = desc.selector;
	*(uint8_t*) (idt_location + sizeof(idt_descriptor)*id + 4) = desc.zero;
	*(uint8_t*) (idt_location + sizeof(idt_descriptor)*id + 5) = desc.type_attr;
	*(uint16_t*)(idt_location + sizeof(idt_descriptor)*id + 6) = desc.offset_16_31;
	return;
}
