/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"
#include "../include/tasking.h"

MODULE("MMU");

uint32_t last_alloc = 0;
uint32_t heap_end = 0;

void mm_init(uint32_t kernel_end)
{
	last_alloc = kernel_end + 0x1000;
	heap_end = 0x400000;
	mprint("Kernel heap starts at 0x%x\n", last_alloc);
}

char* malloc(size_t size)
{
	char* ret = (char*)last_alloc;
	last_alloc += size;
	if(last_alloc >= heap_end)
	{
		set_task(0);
		panic("Cannot allocate %d bytes! Out of memory.\n", size);
	}
	mprint("Allocated %d bytes from 0x%x to 0x%x\n", size, ret, last_alloc);
	return ret;
}
