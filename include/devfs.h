#ifndef __DEVFS_H_
#define __DEVFS_H_

extern void devfs_init();
extern uint8_t devfs_probe(device_t *dev);
extern uint8_t devfs_mount(device_t *dev, void *priv);

#endif