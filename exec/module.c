/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"
#include "../include/tasking.h"
#include "../include/pit.h"
#include "../include/module.h"

MODULE("MMU");

void module_load(uint8_t *buffer, uint32_t size)
{
	kprintf("Loading module (@0x%x size: %d bytes)\n", buffer, size);
	_kill();
}