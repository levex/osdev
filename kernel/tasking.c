/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/x86/gdt.h"
#include "../include/display.h"
#include "../include/pit.h"
#include "../include/tasking.h"
#include "../include/memory.h"

#include <stdint.h>

MODULE("TASK");

PROCESS* c = 0;
uint32_t lpid = 0;

void task1()
{
	while(1) kprintf("1");
}

void task2()
{
	while(1) kprintf("2");
}

void idle_thread()
{
	while(1);
}

void kill(uint32_t pid)
{
	if(pid == 1) panic("Idle can't be killed!\n");
}

PROCESS* createProcess(char* name, uint32_t addr)
{
	PROCESS* p = malloc(sizeof(PROCESS));
	memset(p, 0, sizeof(PROCESS));
	p->name = name;
	p->pid = ++lpid;
	p->eip = addr;
	p->esp = (uint32_t)malloc(4096);
	uint32_t* stack = p->esp + 4096;
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

void addProcess(PROCESS* p)
{
	set_task(0);
	p->next = c->next;
	p->next->prev = p;
	p->prev = c;
	c->next = p;
	set_task(1);
}

void schedule()
{
	//while(1);
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
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
	asm volatile("out %%al, %%dx": :"d"(0x20), "a"(0x20));
	asm volatile("iret");
}

void tasking_init()
{
	mprint("Creating idle process\n");
	c = createProcess("kidle", (uint32_t)idle_thread);
	c->next = c;
	c->prev = c;
	addProcess(createProcess("task1", (uint32_t)task1));
	addProcess(createProcess("task2", (uint32_t)task2));
	mprint("Tasking online!\n");
}
