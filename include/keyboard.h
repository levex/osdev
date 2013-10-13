/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include <stdint.h>

extern void keyboard_init();
extern uint8_t keyboard_enabled();
extern char keyboard_get_key();
extern uint8_t keyboard_to_ascii(uint8_t key);

#endif
