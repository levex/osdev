/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"

MODULE("MMU");

uint32_t last_alloc = 0;

void mm_init(uint32_t kernel_end)
{
	last_alloc = kernel_end + 0x1000;
	mprint("Kernel heap starts at 0x%x\n", last_alloc);
}

char* malloc(size_t size)
{
	char* ret = (char*)last_alloc;
	last_alloc += size;
	mprint("Allocated %d bytes from 0x%x to 0x%x\n", size, ret, last_alloc);
	return ret;
}
