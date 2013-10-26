/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/hal.h"
#include "../include/mutex.h"
#include "../include/tasking.h"
#include "../include/device.h"

MODULE("DEV");

device_t *devices = 0;
uint8_t lastid = 0;

void device_init()
{
	devices = (device_t*)malloc(64*sizeof(device_t));
	memset(devices, 0, 64*sizeof(device_t));
	lastid = 0;
	mprint("Device Manager initialized.\n");
	_kill();
}

int device_add(device_t* dev)
{
	devices[++lastid] = *dev;
	mprint("Registered Device %s (%d) as Device#%d\n", dev->name, dev->unique_id, lastid);
	return lastid;
}

device_t *device_get_by_id(int id)
{
	for(int i = 0;i < 64; i++)
	{
		if(devices[i].unique_id == id) return &devices[i];
	}
}

int device_getnumber()
{
	return lastid;
}

device_t *device_get(int id)
{
	return &devices[id];
}
