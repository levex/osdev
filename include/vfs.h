/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __VFS_H_
#define __VFS_H_

#include "device.h"

struct __device_t;

typedef struct __fs_t {
	char *name;
	uint8_t (*probe)(struct __device_t *);
	uint8_t (*read)(char *, char *, struct __device_t *, void *);
	uint8_t (*read_dir)(char *, char *, struct __device_t *, void *);
	uint8_t (*exist)(char *filename, struct __device_t *, void *);
	uint8_t (*mount)(struct __device_t *, void *);
	uint8_t *priv_data;
} filesystem_t;

typedef struct __mount_info_t {
	char *loc;
	struct __device_t *dev;
} mount_info_t;

extern uint8_t vfs_read(char *f, char* buf);
extern uint32_t vfs_ls(char *d, char *buf);
extern uint8_t vfs_exist_in_dir(char *wd, char* fn);

extern void vfs_init();

extern uint8_t list_mount();

extern uint8_t device_try_to_mount(struct __device_t *dev, char *location);

#endif
