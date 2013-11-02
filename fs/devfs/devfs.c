/** @author Levente Kurusa <levex@linux.com> **/
#include "../../include/display.h"
#include "../../include/vfs.h"
#include "../../include/hal.h"
#include "../../include/tasking.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/levos.h"
#include "../../include/devfs.h"

#include <stdint.h>

MODULE("DEVFS");

uint8_t devfs_probe(device_t *dev)
{
	if(dev->unique_id == 11)
		return 1;
	return 0;
}

uint8_t devfs_read(char *f UNUSED,  char *buf UNUSED, device_t *dev UNUSED, void * a UNUSED)
{
	return 0;
}

uint8_t devfs_read_dir(char *f UNUSED, char *buf UNUSED, device_t *dev UNUSED, void * a UNUSED)
{
	kprintf(".\n..\n");
}

uint8_t devfs_exist(char *f, device_t *dev UNUSED, void *a UNUSED)
{
	if(strcmp(f, "/") == 0)
		return 1;
	return 0;
}

uint8_t devfs_mount(device_t *dev, void *a UNUSED)
{
	if(dev->unique_id == 11)
		return 1;
	return 0;
}

void devfs_init()
{
	filesystem_t *fs = (filesystem_t *)malloc(sizeof(filesystem_t));
	mprint("Mounting /dev filesystem.\n");
	fs->name = "DEVFS";
	fs->probe = devfs_probe;
	fs->read = devfs_read;
	fs->read_dir = devfs_read_dir;
	fs->exist = devfs_exist;
	fs->mount = devfs_mount;
	device_t *dev_dev = (device_t *)malloc(sizeof(device_t));
	dev_dev->name = "DEVDEV";
	dev_dev->unique_id = 11;
	dev_dev->dev_type = DEVICE_BLOCK;
	dev_dev->fs = fs;
	dev_dev->read = 0;
	dev_dev->write = 0;
	device_add(dev_dev);
	dev_dev = device_get_by_id(11);
	device_try_to_mount(dev_dev, "/dev/");
	_kill();
}