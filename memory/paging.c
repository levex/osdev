/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"

MODULE("PAGE");

static uint32_t* page_directory = 0;
static uint32_t page_dir_loc = 0;
static uint32_t* last_page = 0;

/* Paging now will be really simple
 * we reserve 0-8MB for kernel stuff
 * heap will be from approx 1mb to 4mb
 * and paging stuff will be from 4mb
 */

void paging_init()
{
	mprint("Setting up paging\n");
	page_directory = (uint32_t*)0x400000;
	page_dir_loc = page_directory;
	last_page = 0x404000;
	for(int i = 0; i < 1024; i++)
	{
		page_directory[i] = 0 | 2;
	}
	paging_map_virtual_to_phys(0, 0);
	paging_map_virtual_to_phys(0x400000, 0x400000);
	paging_enable();
	mprint("Paging was successfully enabled!\n");
}

void paging_enable()
{
	asm volatile("mov %%eax, %%cr3": :"a"(page_dir_loc));	
	asm volatile("mov %cr0, %eax");
	asm volatile("orl $0x80000000, %eax");
	asm volatile("mov %eax, %cr0");
}

void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys)
{
	uint16_t id = virt >> 22;
	for(int i = 0; i < 1024; i++)
	{
		last_page[i] = phys | 3;
		phys += 4096;
	}
	page_directory[id] = ((uint32_t)last_page) | 3;
	last_page = (((uint32_t)last_page) + 4096);
	mprint("Mapping 0x%x (%d) to 0x%x\n", virt, id, phys);
}
