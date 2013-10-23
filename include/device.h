/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __DEVICE_H_
#define __DEVICE_H_

typedef enum __device_type {
	DEVICE_UNKNOWN = 0,
	DEVICE_CHAR = 1,
	DEVICE_BLOCK = 2,
} device_type;

struct __fs_t;

typedef struct {
	char *name;
	uint32_t unique_id;
	device_type dev_type;
	struct __fs_t *fs;
	uint8_t (*read)(uint8_t* buffer, uint32_t offset , uint32_t len);
} device_t;

typedef struct __fs_t {
	char *name;
	uint8_t (*probe)(device_t *);
	uint8_t *priv_data;
} filesystem_t;

extern void device_init();
extern int device_add(device_t* dev);
extern device_t *device_get(int id);
device_t *device_get_by_id(int id);
extern int device_getnumber();

#endif
