/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/hal.h"
#include "../include/mutex.h"
#include "../include/tasking.h"
#include "../include/device.h"
#include "../include/vfs.h"
#include "../include/string.h"
#include "../include/ext2.h"

MODULE("VFS");
#define MAX_MOUNTS 16


uint8_t __init_vfs = 0;

mount_info_t **mount_points = 0;
int last_mount_id = 0;

device_t *check_mount(char *loc)
{
	for(int i = 0; i < last_mount_id; i++)
	{
		if(strcmp(loc, mount_points[i]->loc) == 0)
		{
			return mount_points[i]->dev;
		}
	}
	return 0;
}

uint8_t device_try_to_mount(device_t *dev, char *loc)
{
	if(!dev || !(dev->unique_id)) return 0;
	if(check_mount(loc)) return 0;
	if(ext2_probe(dev)) {
		if(ext2_mount(dev, dev->fs->priv_data))
		{
			mount_info_t *m = (mount_info_t *)malloc(sizeof(mount_info_t));
			m->loc = loc;
			m->dev = dev;
			last_mount_id++;
			mount_points[last_mount_id - 1] = m;
		}
		return 1;
	}
	return 0;
}

uint8_t vfs_read(char *filename, char *buffer)
{
	/* Correct algorithm to resolve mounts:
	 * In a loop remove until '/' and then look for match
	 * if no match, continue until last '/' and then we know
	 * it is on the root_device
	 */

	 /* For now, we don't support mounts outside root, so
	  * just pass it to the root mount */
	 if(!mount_points[0]) return 0;

	 mount_points[0]->dev->fs->read(filename, buffer,
	 	mount_points[0]->dev, mount_points[0]->dev->fs->priv_data);
	 return 1;
}

uint32_t vfs_ls(char *dir, char* buffer)
{
	mount_points[0]->dev->fs->read_dir(dir, buffer,
	 	mount_points[0]->dev, mount_points[0]->dev->fs->priv_data);
	return 1;
}

uint8_t vfs_exist_in_dir(char *wd, char *fn)
{
	char *filename = malloc(strlen(wd) + 2 + strlen(fn));
	memcpy(filename, wd, strlen(wd));
	memcpy(filename+strlen(wd), fn, strlen(fn));
	memset(filename+strlen(wd)+strlen(fn) + 1, '\0', 1);
	return mount_points[0]->dev->fs->exist(filename,
	 	mount_points[0]->dev, mount_points[0]->dev->fs->priv_data);
}

void vfs_init()
{
	mprint("Loading VFS\n");
	mount_points = (mount_info_t **)malloc(sizeof(uint32_t) * MAX_MOUNTS);
	__init_vfs = 1;
	_kill();
}