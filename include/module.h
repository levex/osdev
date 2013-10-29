#ifndef __MODULE_H_
#define __MODULE_H_

#define MODULE_NAME(name) static char *__MODULE_NAME __attribute((unused)) = name;
#define MODULE_INIT(func) static void *__module_init_pointer __attribute((unused)) = &func;
#define MODULE_EXIT(func) static void *__module_exit_pointer __attribute((unused)) = &func;

#endif