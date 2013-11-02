#ifndef __MODULE_H_
#define __MODULE_H_

 #include <stdint.h>

#define MODULE_NAME(name) static char *__MODULE_NAME __attribute((unused)) = name;
#define MODULE_INIT(func) static void *__module_init_pointer __attribute((unused)) = &func;
#define MODULE_EXIT(func) static void *__module_exit_pointer __attribute((unused)) = &func;

#define EXPORT_SYMBOL(FUNC) __module_add_list(#FUNC, FUNC);

extern void __test();

extern void module_init();
extern void module_call_func(char *fn);
extern void __module_add_list(char *fn, uint32_t addr);

#endif