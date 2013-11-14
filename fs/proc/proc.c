/** @author Levente Kurusa <levex@linux.com> **/
#include "../../include/display.h"
#include "../../include/vfs.h"
#include "../../include/hal.h"
#include "../../include/tasking.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/levos.h"
#include "../../include/pci.h"

#include <stdint.h>

MODULE("PROC");

uint8_t procfs_read(char *fn, char *buffer, device_t *dev UNUSED, void* priv UNUSED)
{
	if(strcmp(fn, "/os/full") == 0)
	{
		char *name = "LevOS levex-pc 4.0-rc1 i386";
		memcpy(buffer, name, strlen(name) + 1);
		return 1; 
	}
	if(strcmp(fn, "/os/arch") == 0)
	{
		char *name = "i386";
		memcpy(buffer, name, strlen(name) + 1);
		return 1; 
	}
	if(strcmp(fn, "/system/pci") == 0)
	{
		pci_proc_dump(buffer);
		return 1;
	}
	return 0;
}

uint8_t procfs_read_dir(char *dn, char *buffer UNUSED, device_t *dev UNUSED, void* priv UNUSED)
{
	if(!dn)return 0;
	if(strcmp(dn, "/") == 0)
	{
			kprintf(".\n..\nos\nsystem\n");
	}
	if(strcmp(dn, "/os/") == 0)
	{
		kprintf(".\n..\nfull\narch\n");
	}
	if(strcmp(dn, "/system/") == 0)
	{
		kprintf(".\n..\npci\n");
	}
	return 1;
}

uint8_t procfs_exist(char *fn, device_t *dev UNUSED, void *priv UNUSED)
{
	if(strcmp(fn, "/") == 0)
		return 1;
	if(strcmp(fn, "/os/") == 0)
		return 1;
	if(strcmp(fn, "/os/full") == 0)
		return 1;
	if(strcmp(fn, "/os/arch") == 0)
		return 1;
	if(strcmp(fn, "/system/") == 0)
		return 1;
	if(strcmp(fn, "/system/pci") == 0)
		return 1;
	return 0;
}

uint8_t procfs_mount(device_t *dev, void *priv UNUSED)
{
	if(dev->unique_id == 13)
		return 1;
	return 0;
}

uint8_t procfs_probe(device_t *dev)
{
	if(dev->unique_id == 13)
		return 1;
	return 0;
}

void proc_init()
{
	mprint("Mounting /proc filesystem.\n");
	filesystem_t *fs = (filesystem_t *)malloc(sizeof(filesystem_t));
	fs->name = "PROCFS";
	fs->probe = procfs_probe;
	fs->read = procfs_read;
	fs->read_dir = procfs_read_dir;
	fs->exist = procfs_exist;
	fs->mount = procfs_mount;
	device_t *proc_dev = (device_t *)malloc(sizeof(device_t));
	proc_dev->name = "PROCDEV";
	proc_dev->unique_id = 13;
	proc_dev->dev_type = DEVICE_BLOCK;
	proc_dev->fs = fs;
	proc_dev->read = 0;
	proc_dev->write = 0;
	device_add(proc_dev);
	proc_dev = device_get_by_id(13);
	device_try_to_mount(proc_dev, "/proc/");
	_kill();
}