/** @author Levente Kurusa <levex@linux.com> **/
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

extern int addProcess(PROCESS* p);
extern PROCESS* createProcess(char* name, uint32_t addr);

extern int is_pid_running(int pid);

extern void _kill();
extern void schedule();
extern void schedule_noirq();
extern void tasking_init();

#endif
