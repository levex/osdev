/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"
#include "../include/tasking.h"
#include "../include/pit.h"
#include "../include/module.h"

MODULE("MOD");

#define MAX_EXPORTS 256

char **fn = 0;
uint32_t *addrs = 0;
uint32_t r = 0;

void __module_add_list(char *f, uint32_t addr)
{
	mprint("Registering function %s to address 0x%x\n", f, addr);
	fn[r] = f;
	addrs[r] = addr;
	r ++;
}

void module_call_func(char *f)
{
	for(int i = 0; i < r; i++)
	{
		if(strcmp(f, fn[i]) == 0)
		{
			asm volatile("call %%eax": :"a"(addrs[i]));
			return;
		}
	}
}

void module_init()
{
	mprint("Init.\n");
	fn = (char **)malloc(MAX_EXPORTS * sizeof(uint32_t));
	addrs = (uint32_t *)malloc(MAX_EXPORTS * sizeof(uint32_t));
	_kill();
}

void module_load(uint8_t *buffer, uint32_t size)
{
	kprintf("Loading module (@0x%x size: %d bytes)\n", buffer, size);
	_kill();
}