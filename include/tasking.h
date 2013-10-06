#ifndef __TASKING_H_
#define __TASKING_H_

#include <stdint.h>

struct _process;

typedef struct _process {
	struct _process* prev;
	char* name;
	uint32_t pid;
	uint32_t esp;
	uint32_t eip;
	struct _process* next;
} PROCESS;

extern void schedule();
extern void tasking_init();

#endif
