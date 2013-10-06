/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/pic.h"
#include "../include/display.h"
#include "../include/hal.h"
#include <stdint.h>

MODULE("PIC");

void pic_init()
{
	/* We have to remap the IRQs to be able to process
	 * hardware-related interrupt requests and to service
	 * exceptions as well.
	 */
	

	/* First step is to save current masks, set by BIOS */
	uint8_t m1 = inportb(PIC_MASTER_DATA), m2 = inportb(PIC_SLAVE_DATA);
	mprint("Existing masks: Master: 0x%x Slave: 0x%x\n", m1, m2);
	mprint("Beginning initialization\n");
	/* set up cascading mode */
	outportb(PIC_MASTER_CMD, 0x10 + 0x01);
	outportb(PIC_SLAVE_CMD,  0x10 + 0x01);
	/* Setup master's vector offset */
	outportb(PIC_MASTER_DATA, 0x20);
	/* Tell the slave its vector offset */
	outportb(PIC_SLAVE_DATA, 0x28);
	/* Tell the master that he has a slave */
	outportb(PIC_MASTER_DATA, 4);
	outportb(PIC_SLAVE_DATA, 2);
	/* Enabled 8086 mode */
	outportb(PIC_MASTER_DATA, 0x01);
	outportb(PIC_SLAVE_DATA, 0x01);

	mprint("Resetting masks\n");
	outportb(PIC_MASTER_DATA, 0);
	outportb(PIC_SLAVE_DATA, 0);
	mprint("Init done.\n");
}

void pic_send_eoi(uint8_t irq)
{
	if(irq >= 8)
		outportb(PIC_SLAVE_CMD, PIC_CMD_EOI);
	outportb(PIC_MASTER_CMD, PIC_CMD_EOI);
}
