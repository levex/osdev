/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __DEVICE_H_
#define __DEVICE_H_

typedef struct {
	char *name;
	uint32_t unique_id;
	uint8_t (*read)(uint8_t* buffer, uint32_t offset , uint32_t len);
} device_t;

extern void device_init();
extern int device_add(device_t* dev);
extern device_t *device_get(int id);
extern int device_getnumber();

#endif
