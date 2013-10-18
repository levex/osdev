/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __VFS_H_
#define __VFS_H_

#include "device.h"

typedef struct {
	char *name;
	void (*probe)(device_t);
} filesystem_t;

extern void vfs_init();

extern int vfs_register_fs(filesystem_t fs);

#endif
