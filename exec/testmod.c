#include "../include/module.h"
#include "../include/display.h"

MODULE_NAME("TEST");

void mymod_init()
{
	__test();
}

void mymod_exit()
{
	return;
}


MODULE_INIT(mymod_init);
MODULE_EXIT(mymod_exit);