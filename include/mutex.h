#ifndef __MUTEX_H_
#define __MUTEX_H_

typedef struct {
	uint8_t locked;
} mutex;

extern void mutex_lock(mutex* m);
extern void mutex_unlock(mutex* m);

#endif
