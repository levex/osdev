#include "../include/module.h"
#include "../include/display.h"

MODULE("TEST");

void mymod_init()
{
	kprintf("Look at me loading modules!\n");
}

void mymod_exit()
{
	kprintf("Au revoir!\n");
}


MODULE_INIT(mymod_init);
MODULE_EXIT(mymod_exit);