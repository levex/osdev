/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/display.h"
#include "../include/mutex.h"
#include "../include/pit.h"

#include <stdint.h>

void mutex_lock(mutex* m)
{
	/* if the lock is locked, wait and set its locked state */
	set_task(0);
	if(!m->locked) { m->locked = 1; return; }
	set_task(1);
	while(m->locked);
}

void mutex_unlock(mutex* m)
{
	/* this code can only be accessed by the holding thread, so unlock it */
	m->locked = 0;
	set_task(1);
}
