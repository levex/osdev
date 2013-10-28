/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/x86/gdt.h"
#include "../include/display.h"
#include "../include/pit.h"
#include "../include/tasking.h"
#include "../include/memory.h"
#include "../include/signal.h"

#include <stdint.h>

MODULE("TASK");

PROCESS* c = 0;
uint32_t lpid = 0;
uint8_t __enabled = 0;

void task1()
{
	mprint("Tasking online.\n");
	_kill();
//	while(1) schedule_noirq();kprintf("[1] Hello, I am task one!\n");
}

void task2()
{
	while(1) kprintf("[2] Hello, I am task two! :)\n");
}
void task3()
{
	while(1) kprintf("[3] Mutex locking memcpy and kprintf! Awesome!\n");
}

void idle_thread()
{
	enable_task();
	__enabled = 1;
	late_init();
}

void kill(uint32_t pid)
{
	if(pid == 1) panic("Idle can't be killed!\n");
	if(pid == c->pid) _kill();
	PROCESS* orig = c;
	PROCESS* p = orig;
	while(1)
	{
		if(p->pid == pid) {
			mprint("Process %s (%d) was set to ZOMBIE.\n", p->name, pid);
			p->state = PROCESS_STATE_ZOMBIE;
			break;
		}
		p = p->next;
		if(p == orig) break;
	}
}

void send_sig(int sig)
{
	c->notify(sig);
}

int is_tasking()
{
	return __enabled;
}

char* p_name()
{
	return c->name;
}

int p_pid()
{
	return c->pid;
}

void _kill()
{
	if(c->pid == 1) { set_task(0); panic("Idle can't be killed!"); }
	mprint("Killing process %s (%d)\n", c->name, c->pid);
	set_task(0);
	free(c->stacktop);
	free(c);
	c->prev->next = c->next;
	c->next->prev = c->prev;
	set_task(1);
	schedule_noirq();
}

/* Let me introduce you to Jack, the ripper.
 * His job is to hunt down and kill zombie processes.
 * He is powerful, don't mess with him! :-)
 */

void jack_the_ripper()
{
	reset:;
	PROCESS *orig = c;
	PROCESS *p = orig;
	while(1)
	{
		p = p->next;
		if(p == c) { continue;}
		if(p->state == PROCESS_STATE_ZOMBIE)
		{
			set_task(0);
			p->prev->next = p->next;
			p->next->prev = p->prev;
			free(p->stacktop);
			free(p);
			set_task(1);
			mprint("Jack killed %s (%d). One less zombie.\n", p->name, p->pid);
		}
		if(p == orig) goto reset;
		schedule_noirq();
	}
}

void tasking_print_all()
{
	PROCESS *orig = c;
	PROCESS *p = orig;
	while(1)
	{
		kprintf("Process: %s (%d) %s\n", p->name, p->pid,
			p->state == PROCESS_STATE_ZOMBIE?"ZOMBIE":
					p->state==PROCESS_STATE_ALIVE?"ALIVE":"DEAD");
		p = p->next;
		if(p == orig) break;
	}
}

void __notified(int sig)
{

	switch(sig)
	{
		case SIG_ILL:
			pidprint("Received SIGILL, terminating!\n");
			_kill();
			break;
		case SIG_TERM:
			pidprint("Received SIGTERM, terminating!\n");
			_kill();
		case SIG_SEGV:
			pidprint("Received SIGSEGV, terminating!\n");
			_kill();
		default:
			pidprint("Received unknown SIG!\n");
			return;
	}
}

int is_pid_running(int pid)
{
	set_task(0);
	PROCESS* p = c;
	PROCESS* orig = c;
	int ret = 0;
	while(1)
	{
		if(p->pid == pid)  { ret = 1; break; }
		p = p->next;
		if(p == orig) break;
	}
	set_task(1);
	return ret;
}

PROCESS* createProcess(char* name, uint32_t addr)
{
	PROCESS* p = malloc(sizeof(PROCESS));
	memset(p, 0, sizeof(PROCESS));
	p->name = name;
	p->pid = ++lpid;
	p->eip = addr;
	p->state = PROCESS_STATE_ALIVE;
	p->notify = __notified;
	p->esp = (uint32_t)malloc(4096);
	uint32_t* stack = p->esp + 4096;
	p->stacktop = p->esp;
	*--stack = 0x00000202; // eflags
	*--stack = 0x8; // cs
	*--stack = (uint32_t)addr; // eip
	*--stack = 0; // eax
	*--stack = 0; // ebx
	*--stack = 0; // ecx;
	*--stack = 0; //edx
	*--stack = 0; //esi
	*--stack = 0; //edi
	*--stack = p->esp + 4096; //ebp
	*--stack = 0x10; // ds
	*--stack = 0x10; // fs
	*--stack = 0x10; // es
	*--stack = 0x10; // gs
	p->esp = (uint32_t)stack;
	mprint("Created task %s with esp=0x%x eip=0x%x\n", p->name, p->esp, p->eip);
	return p;
}

/* add process but take care of others also! */
int addProcess(PROCESS* p)
{
	set_task(0);
	__addProcess(p);
	set_task(1);
	return p->pid;
}

/* This adds a process while no others are running! */
void __addProcess(PROCESS* p)
{
	p->next = c->next;
	p->next->prev = p;
	p->prev = c;
	c->next = p;
}

/* starts tasking */
void __exec()
{
	asm volatile("mov %%eax, %%esp": :"a"(c->esp));
	asm volatile("pop %gs");
	asm volatile("pop %fs");
	asm volatile("pop %es");
	asm volatile("pop %ds");
	asm volatile("pop %ebp");
	asm volatile("pop %edi");
	asm volatile("pop %esi");
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
	asm volatile("iret");
}

void schedule_noirq()
{
	if(!__enabled) return;
	asm volatile("int $0x2e");
	return;
}

void schedule()
{
	//asm volatile("add $0xc, %esp");
	asm volatile("push %eax");
	asm volatile("push %ebx");
	asm volatile("push %ecx");
	asm volatile("push %edx");
	asm volatile("push %esi");
	asm volatile("push %edi");
	asm volatile("push %ebp");
	asm volatile("push %ds");
	asm volatile("push %es");
	asm volatile("push %fs");
	asm volatile("push %gs");
	asm volatile("mov %%esp, %%eax":"=a"(c->esp));
	c = c->next;
	asm volatile("mov %%eax, %%esp": :"a"(c->esp));
	asm volatile("pop %gs");
	asm volatile("pop %fs");
	asm volatile("pop %es");
	asm volatile("pop %ds");
	asm volatile("pop %ebp");
	asm volatile("pop %edi");
	asm volatile("pop %esi");
	asm volatile("out %%al, %%dx": :"d"(0x20), "a"(0x20)); // send EoI to master PIC
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
	asm volatile("iret");
}

void tasking_init()
{
	mprint("Creating idle process\n");
	c = createProcess("kidle", (uint32_t)idle_thread);
	c->next = c;
	c->prev = c;
	__addProcess(createProcess("task1", (uint32_t)task1));
	__addProcess(createProcess("Jack", (uint32_t)jack_the_ripper));
	/*__addProcess(createProcess("task2", (uint32_t)task2));
	__addProcess(createProcess("task3", (uint32_t)task3));*/
	__exec();
	panic("Failed to start tasking!");
}
