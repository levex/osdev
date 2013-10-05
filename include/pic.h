/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __PIC_H_
#define __PIC_H_

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CMD 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_CMD_EOI 0x20

#include <stdint.h>

extern void pic_init();
extern void pic_send_eoi(uint8_t irq);

#endif
