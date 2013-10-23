#include "../include/x86/v86.h"
#include "../include/display.h"
#include "../include/tasking.h"


MODULE("V86");

void v86_test()
{
	mprint("Starting v86 code!\n");
	v86_test_code();
	mprint("Returned.\n");
	_kill();
}