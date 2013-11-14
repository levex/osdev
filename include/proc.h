#ifndef __PROC_H_
#define __PROC_H_

extern void proc_init();
extern uint8_t procfs_probe(device_t *dev);
extern uint8_t procfs_mount(device_t *dev, void *priv);

#endif
