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
	int edx = 0;
	asm volatile("movl %%eax, %0":"=m"(eax));
	asm volatile("movl %%ebx, %0":"=m"(ebx));
	asm volatile("movl %%ecx, %0":"=m"(ecx));
	asm volatile("movl %%edx, %0":"=m"(edx));
	//kprintf("SYSCALL! a: 0x%x b: 0x%x c:0x%x ", eax, ebx, ecx);
	//kprintf("d: %d\n", edx);
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
			switch(ebx)
			{
				case 1: /* stdout */
					break;
				case 0: /* stdin */
					for(int i = 0;i < edx; i++)
					{
						char c = 0;
						while(!(c = keyboard_get_key()));
						*((uint8_t *)(ecx + i)) = c;
						kprintf("%c", c);
						//kprintf("(0x%x) %c\n", ((uint8_t *)(ecx + i)), *((uint8_t *)(ecx + i)));
						if(c == '\n')
						{
							edx = i;
							break;
						}
					}
					break;
				case 2: /* stderr */
					break;
				default:
					break;
			}
			asm volatile("movl %%eax, %%edx": :"a"(edx*4));
			break;
		case 4: /* syscall 4: write() eax=4 ebx=file ecx=buffer edx=size RET: ecx=size*/
			switch(ebx)
			{
				case 0: /* STDOUT */
					;
					char *str = malloc(edx + 1);
					memcpy(str, ecx, edx);
					str[edx + 1] = 0;
					kprintf("%s", str);
					free(str);
					break;
				case 1: /* stdin */
					/* Can't write to stdin! */
					asm volatile("movl $0, %ecx");
					break;
				case 2: /* stderr */
					kprintf("ERR: %s", ecx);
					break;
				default:
					/* To find descriptor substracr 4 from id */
					asm volatile("movl $0, %ecx");
					break;
			}
			break;
		case 5: /* syscall 5: open() eax=5 ebx=file ecx=flags  edx=mode RET: eax=file */
			//asm volatile("movl $0, %eax");
			;
			uint32_t desc = 0xffffffff;
			switch(ebx)
			{
				case 0: /* stdout */
					desc = 0;
					break;
				case 1: /* stdin */
					desc = 1;
					break;
				case 2: /* stderr */
					desc = 2;
					break;
			}
			/* Add to open list */
			PROCESS *c = p_proc();
			if(c->num_open_files >= MAX_OPEN_FILES)
			{
				desc = 0xffffffff; // @temp @todo
			} else {
				if(desc == 0xffffffff)
				{
					if(vfs_exist_in_dir("", (char *)ebx))
					{
						char *openbuf = (char *)malloc(strlen((char *)ebx) + 1);
						memcpy(openbuf, (char *) ebx, strlen((char *)ebx) + 1);
						c->open_files[c->num_open_files] = openbuf;
						desc = c->num_open_files + 4;
						c->num_open_files ++;
					} else {
						desc = 0xffffffff;
					}
				}
			}
			asm volatile("movl %%ebx, %%eax": :"b"(desc));
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