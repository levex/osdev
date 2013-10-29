/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"
#include "../include/tasking.h"
#include "../include/pit.h"
#include "../include/loader.h"
#include "../include/module.h"

MODULE("LOAD");

#define MAX_LOADERS 16

loader_t **loaders = 0;
uint8_t last_loader = 0;

uint32_t last_load_loc = 0x400000;

void loader_init()
{
	mprint("Setting up loader.\n");
	loaders = (loader_t**)malloc(MAX_LOADERS * sizeof(uint32_t));
	memset(loaders, 0, MAX_LOADERS * sizeof(uint32_t));
	_kill();
}

void register_loader(loader_t *load)
{
	if(last_loader + 1 > MAX_LOADERS) return;
	if(!load) return;
	loaders[last_loader] = load;
	last_loader ++;
	mprint("Registered %s loader as id %d\n", load->name, last_loader - 1);
}

uint32_t loader_get_unused_load_location()
{
	last_load_loc += 0x400000;
	return last_load_loc;
}

uint8_t exec_start(uint8_t *buf)
{
	for(int i = 0;i < MAX_LOADERS; i++)
	{
		if(!loaders[i]) break;
		void *priv = loaders[i]->probe(buf);
		if(priv)
		{
			return loaders[i]->start(buf, priv);
		}
	}
	return 0;
}