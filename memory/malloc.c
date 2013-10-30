/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"
#include "../include/tasking.h"
#include "../include/pit.h"

MODULE("MMU");
#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

void mm_init(uint32_t kernel_end)
{
	last_alloc = kernel_end + 0x1000;
	heap_begin = last_alloc;
	pheap_end = 0x400000;
	pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
	heap_end = pheap_begin;
	memset((char *)heap_begin, 0, heap_end - heap_begin);
	pheap_desc = (uint8_t *)malloc(MAX_PAGE_ALIGNED_ALLOCS);
	mprint("Kernel heap starts at 0x%x\n", last_alloc);
}

void mm_print_out()
{
	kprintf("Memory used: %d bytes\n", memory_used);
	kprintf("Memory free: %d bytes\n", heap_end - heap_begin - memory_used);
	kprintf("Heap size: %d bytes\n", heap_end - heap_begin);
	kprintf("Heap start: 0x%x\n", heap_begin);
	kprintf("Heap end: 0x%x\n", heap_end);
	kprintf("PHeap start: 0x%x\nPHeap end: 0x%x\n", pheap_begin, pheap_end);
}

void free(void *mem)
{
	alloc_t *alloc = (mem - sizeof(alloc_t));
	memory_used -= alloc->size + sizeof(alloc_t);
	alloc->status = 0;
}

void pfree(void *mem)
{
	if(mem < pheap_begin || mem > pheap_end) return;
	/* Determine which page is it */
	uint32_t ad = (uint32_t)mem;
	ad -= pheap_begin;
	ad /= 4096;
	/* Now, ad has the id of the page */
	pheap_desc[ad] = 0;
	return;
}

char* pmalloc(size_t size)
{
	/* Loop through the avail_list */
	for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++)
	{
		if(pheap_desc[i]) continue;
		pheap_desc[i] = 1;
		mprint("PAllocated from 0x%x to 0x%x\n", pheap_begin + i*4096, pheap_begin + (i+1)*4096);
		return (char *)(pheap_begin + i*4096);
	}
	mprint("pmalloc: FATAL: failure!\n");
	return 0;
}

char* malloc(size_t size)
{
	if(!size) return 0;

	/* Loop through blocks and find a block sized the same or bigger */
	uint8_t *mem = (uint8_t *)heap_begin;
	while((uint32_t)mem < last_alloc)
	{
		alloc_t *a = (alloc_t *)mem;
		/* If the alloc has no size, we have reaced the end of allocation */
		//mprint("mem=0x%x a={.status=%d, .size=%d}\n", mem, a->status, a->size);
		if(!a->size)
			goto nalloc;
		/* If the alloc has a status of 1 (allocated), then add its size
		 * and the sizeof alloc_t to the memory and continue looking.
		 */
		if(a->status) {
			mem += a->size;
			mem += sizeof(alloc_t);
			mem += 4;
			continue;
		}
		/* If the is not allocated, and its size is bigger or equal to the
		 * requested size, then adjust its size, set status and return the location.
		 */
		if(a->size >= size)
		{
			/* Set to allocated */
			a->status = 1;

			mprint("RE:Allocated %d bytes from 0x%x to 0x%x\n", size, mem + sizeof(alloc_t), mem + sizeof(alloc_t) + size);
			memset(mem + sizeof(alloc_t), 0, size);
			memory_used += size + sizeof(alloc_t);
			return (char *)(mem + sizeof(alloc_t));
		}
		/* If it isn't allocated, but the size is not good, then
		 * add its size and the sizeof alloc_t to the pointer and
		 * continue;
		 */
		mem += a->size;
		mem += sizeof(alloc_t);
		mem += 4;
	}

	nalloc:;
	if(last_alloc+size+sizeof(alloc_t) >= heap_end)
	{
		set_task(0);
		panic("Cannot allocate %d bytes! Out of memory.\n", size);
	}
	alloc_t *alloc = (alloc_t *)last_alloc;
	alloc->status = 1;
	alloc->size = size;

	last_alloc += size;
	last_alloc += sizeof(alloc_t);
	last_alloc += 4;
	mprint("Allocated %d bytes from 0x%x to 0x%x\n", size, (uint32_t)alloc + sizeof(alloc_t), last_alloc);
	memory_used += size + 4 + sizeof(alloc_t);
	memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
	return (char *)((uint32_t)alloc + sizeof(alloc_t));
/*
	char* ret = (char*)last_alloc;
	last_alloc += size;
	if(last_alloc >= heap_end)
	{
		set_task(0);
		panic("Cannot allocate %d bytes! Out of memory.\n", size);
	}
	mprint("Allocated %d bytes from 0x%x to 0x%x\n", size, ret, last_alloc);
	return ret;*/
}
