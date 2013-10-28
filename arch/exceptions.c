/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/x86/idt.h"
#include "../include/display.h"
#include "../include/tasking.h"
#include "../include/signal.h"
#include "../include/pit.h"
#include <stdint.h>

MODULE("EXCE");

void exc_divide_by_zero()
{
	//kerror("Divide by zero at [0x%x:0x%x] EFLAGS: 0x%x\n", cs, eip, eflags);
	kprintf("Divide by zero!\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGILL\n", p_name(), p_pid());
		send_sig(SIG_ILL);
	}
}

void exc_debug()
{
	kprintf("Debug!\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
}

void exc_nmi()
{
	kprintf("NMI\n");
	/*if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}*/
	return;
}

void exc_brp()
{
	kprintf("Breakpoint!\n");
	return;
}

void exc_overflow()
{
	kprintf("Overflow!\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_bound()
{
	kprintf("Bound range exceeded.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_invopcode()
{
	kprintf("Invalid opcode.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_device_not_avail()
{
	kprintf("Device not available.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_double_fault()
{
	kprintf("Double fault, halting.\n");
	set_task(0);
	for(;;);
}

void exc_coproc()
{
	kprintf("Coprocessor fault, halting.\n");
	set_task(0);
	for(;;);
	return;
}

void exc_invtss()
{
	kprintf("TSS invalid.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_segment_not_present()
{
	kprintf("Segment not present.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGSEGV\n", p_name(), p_pid());
		send_sig(SIG_SEGV);
	}
	return;
}

void exc_ssf()
{
	kprintf("Stacksegment faulted.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_gpf()
{
	kprintf("General protection fault.\n");
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_pf()
{
	kprintf("Page fault in %s (%d)\n",p_name(), p_pid());
	if(is_tasking()) {
		kprintf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		send_sig(SIG_TERM);
	}
	return;
}

void exc_reserved()
{
	kprintf("This shouldn't happen. Halted.\n");
	set_task(0);
	for(;;);
	return;
}

void exceptions_init()
{
	mprint("Installing exceptions handlers\n");
	idt_register_interrupt(0, (uint32_t)exc_divide_by_zero);
	idt_register_interrupt(1, (uint32_t)exc_debug);
	idt_register_interrupt(2, (uint32_t)exc_nmi);
	idt_register_interrupt(3, (uint32_t)exc_brp);
	idt_register_interrupt(4, (uint32_t)exc_overflow);
	idt_register_interrupt(5, (uint32_t)exc_bound);
	idt_register_interrupt(6, (uint32_t)exc_invopcode);
	idt_register_interrupt(7, (uint32_t)exc_device_not_avail);
	idt_register_interrupt(8, (uint32_t)exc_double_fault);
	idt_register_interrupt(9, (uint32_t)exc_coproc);
	idt_register_interrupt(10, (uint32_t)exc_invtss);
	idt_register_interrupt(11, (uint32_t)exc_segment_not_present);
	idt_register_interrupt(12, (uint32_t)exc_ssf);
	idt_register_interrupt(13, (uint32_t)exc_gpf);
	idt_register_interrupt(14, (uint32_t)exc_pf);
	idt_register_interrupt(15, (uint32_t)exc_reserved);
	return;
}
