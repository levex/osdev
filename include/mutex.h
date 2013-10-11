/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __MUTEX_H_
#define __MUTEX_H_

typedef struct {
	uint8_t locked;
} mutex;

#define DEFINE_MUTEX(name) static mutex name = {.locked=0};

extern void mutex_lock(mutex* m);
extern void mutex_unlock(mutex* m);

#endif
