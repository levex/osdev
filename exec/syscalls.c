/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/display.h"
#include "../include/vfs.h"
#include "../include/hal.h"
#include "../include/tasking.h"
#include "../include/device.h"
#include "../include/memory.h"
#include "../include/levos.h"

#include <stdint.h>

MODULE("SYS");

void syscall()
{
	asm volatile("add $0x2c, %esp");
	asm volatile("pusha");
	int eax = 0;
	int ebx = 0;
	int ecx = 0;
	asm volatile("movl %%eax, %0":"=m"(eax));
	asm volatile("movl %%ebx, %0":"=m"(ebx));
	asm volatile("movl %%ecx, %0":"=m"(ecx));
	//kprintf("SYSCALL! a: 0x%x b: 0x%x c:0x%x\n", eax, ebx, ecx);
	switch(eax)
	{
		case 0: /* syscall 0: exit() eax=0 RET: none */
			_kill();
			break;
		case 1: /* syscall 1: open() eax=1 ebx=stdout(1) RET: eax=file*/
			asm volatile("movl $1, %eax");
			break;
		case 2: /* syscall 2: write() eax=2 ebx=buffer ecx=file RET: none*/
			switch(ecx)
			{
				case 0:
					kprintf("%s\n", ebx);
					break;
			}
			break;
		default:
			break;
	}
	INT_END;
}

void syscall_init()
{
	mprint("Registering syscall interface\n");
	set_int(0x80, (uint32_t)syscall);
}