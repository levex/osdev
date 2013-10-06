#ifndef __MEMORY_H_
#define __MEMORY_H_

#include <stdint.h>
#include <stddef.h>

extern void mm_init();

extern void paging_init();
extern void paging_map_virtual_to_phys(uint32_t virt, uint32_t phys);

extern char* malloc(size_t size);

extern void* memcpy(const void* dest, const void* src, size_t num );
extern void* memset (void * ptr, int value, size_t num );
extern void* memset16 (void *ptr, uint16_t value, size_t num);

#endif
