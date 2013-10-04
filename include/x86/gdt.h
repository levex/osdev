/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __GDT_H_
#define __GDT_H_

#include <stdint.h>

#define GDT_ACCESSED	0x01
#define GDT_READWRITE	0x02
#define GDT_DIRECTION	0x04
#define GDT_EXEC	0x08
#define GDT_STATIC	0x10
#define GDT_PRESENT	0x20


extern void gdt_init();
extern int gdt_set_descriptor();
extern int gdt_add_descriptor(uint8_t id, uint64_t desc);
extern uint64_t gdt_create_descriptor(uint32_t base, uint32_t limit, uint16_t flag);

#endif
