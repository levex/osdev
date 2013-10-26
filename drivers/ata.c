/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/ata.h"
#include "../include/display.h"
#include "../include/hal.h"
#include "../include/tasking.h"
#include "../include/device.h"
#include "../include/memory.h"

#include <stdint.h>

MODULE("ATA");

void ata_init()
{
	mprint("Checking for ATA drivers\n");
	_kill();
}