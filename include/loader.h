#ifndef __LOADER_H_
#define __LOADER_H_

typedef struct {
	char *name;
	void* (*probe)(uint8_t *buf);
	uint8_t (*start)(uint8_t *buf, void *priv);
} loader_t;

extern void loader_init();
extern void register_loader(loader_t *load);
extern uint8_t exec_start(uint8_t *buf);

extern uint32_t loader_get_unused_load_location();

#endif