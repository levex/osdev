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
		case 0: /* syscall 0: restart() */
			break;
		case 1: /* syscall 1: exit() eax=1 ebx=err */
			_kill();
			for(;;);
			break;
		case 2: /* syscall 2: fork() eax = 2 ebx=pt_regs (?) */
			break;
		case 3: /* syscall 3: read() eax=3 ebx=file ecx=buf edx=size */
			break;
		case 4: /* syscall 4: write() eax=4 ebx=file ecx=buffer edx=size RET: ecx=size*/
			switch(ebx)
			{
				case 0:
					kprintf("%s", ecx);
					break;
			}
			break;
		case 5: /* syscall 5: open() eax=5 ebx=file ecx=flags  edx=mode RET: eax=file */
			asm volatile("movl $1, %eax");
			break;
		case 0x14: /* syscall 0x14: getpid() eax=0x14 RET: eax=pid */
			asm volatile("movl %%ebx, %%eax": :"b"(p_pid()));
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