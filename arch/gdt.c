/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/x86/gdt.h"
#include "../include/display.h"

#include <stdint.h>

MODULE("GDT");

extern void _set_gdtr();
extern void _reload_segments();

static uint32_t gdt_pointer = 0;
static uint32_t  gdt_size = 0;
static uint32_t gdtr_loc = 0;

static uint32_t highpart = 0;
static uint32_t lowpart = 0;

void gdt_init()
{
	gdt_pointer = 0x806; // start GDT data at 4MB
	mprint("location of GDT: 0x%x\n", gdt_pointer);
	gdtr_loc =    0x800;
	mprint("location of GDTR: 0x%x\n", gdtr_loc);
	gdt_add_descriptor(0, 0);
	gdt_add_descriptor(1, 0x00CF9A000000FFFF);
	gdt_add_descriptor(2, 0x00CF92000000FFFF);
	gdt_add_descriptor(3, 0x008FFA000000FFFF); // 16bit code pl3
	gdt_set_descriptor(4, 0x008FF2000000FFFF); // 16bit data pl3
	mprint("Global Descriptor Table is alive.\n");
}

int gdt_set_descriptor()
{
	/* GDTR
	 * 0-1 = SIZE - 1
	 * 2-5 = OFFSET
	 */
	*(uint16_t*)gdtr_loc = (gdt_size - 1) & 0x0000FFFF;
	gdtr_loc += 2;
	*(uint32_t*)gdtr_loc = gdt_pointer;
	_set_gdtr();
	mprint("GDTR was set. gdtr.size=%d gdtr.offset=0x%x\n", 
		*(uint16_t*)(gdtr_loc-2) + 1, 
		*(uint32_t*)gdtr_loc);
	_reload_segments();
	mprint("Segments reloaded.\n");
	return 0;
}

int gdt_add_descriptor(uint8_t id, uint64_t desc)
{
	uint32_t loc = gdt_pointer + sizeof(uint64_t)*id;
	*(uint64_t*)loc = desc;
	mprint("Added entry %d = 0x%x << 32 | 0x%x\n", id, (*(uint64_t*)loc) >> 32, *(uint32_t*)loc+4);
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
