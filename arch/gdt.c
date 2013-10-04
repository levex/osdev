/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/x86/gdt.h"
#include "../include/display.h"

#include <stdint.h>

MODULE("GDT");

extern void _set_gdtr();
extern void _reload_segments();

static uint64_t* gdt_pointer = 0;
static uint32_t  gdt_size = 0;
static uint64_t* gdtr_loc = 0;

static uint32_t highpart = 0;
static uint32_t lowpart = 0;

void gdt_init()
{
	gdt_pointer = 0x401000; // start GDT data at 4MB
	mprint("location of GDT: 0x%x\n", gdt_pointer);
	gdtr_loc =    0x400000;
	mprint("location of GDTR: 0x%x\n", gdtr_loc);
	gdt_add_descriptor(0, 0);
	gdt_add_descriptor(1, 0x00CF9A000000FFFF);
	gdt_add_descriptor(2, 0x00CF92000000FFFF);
	//panic("failure to set GDT");
	gdt_set_descriptor();
	mprint("Global Descriptor Table is alive.\n");
}

int gdt_set_descriptor()
{
	*gdtr_loc = (gdt_size - 1) & 0x0000FFFF;
	gdtr_loc = (uint32_t*)((uint32_t)gdtr_loc + 2);
	*gdtr_loc = gdt_pointer;
	_set_gdtr();
	mprint("GDTR was set. gdtr.size=%d gdtr.offset=0x%x\n", (*(uint32_t*)((uint32_t)gdtr_loc - 2) & 0x0000FFFF) + 1, *gdtr_loc);
	panic("Unable to continue.");	
	_reload_segments();
	mprint("Segments reloaded.");
	return 0;
}

int gdt_add_descriptor(uint8_t id, uint64_t desc)
{
	gdt_pointer[id] = desc;
	mprint("Added entry %d = 0x%x << 32 | 0x%x\n", id, gdt_pointer[id] >> 32, (uint32_t)(gdt_pointer[id]));
	gdt_size += sizeof(desc);
	return 0;
}

uint64_t gdt_create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t desc = 0;
	highpart = 0;
	lowpart = 0;
	desc = limit 		& 0x000F0000;
	desc |= (flag << 8) 	& 0x00F0FF00;
	desc |= (base >> 16) 	& 0x000000FF;
	desc |= base		& 0xFF000000;
	
	highpart = desc;
	desc <<= 32;

	desc |= base << 16;
	desc |= limit		& 0x0000FFFF;
	lowpart = (uint32_t)desc;
	return desc;
}
