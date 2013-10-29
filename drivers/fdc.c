/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/floppy.h"
#include "../include/display.h"
#include "../include/hal.h"
#include "../include/tasking.h"
#include "../include/dma.h"
#include "../include/pit.h"
#include "../include/device.h"
#include "../include/memory.h"

#include <stdint.h>

MODULE("FDC");

#define FDC_DMA_CHANNEL 2
#define FLPY_SECTORS_PER_TRACK 18
#define DMA_BUFFER 0x1000

uint8_t primary_avail = 0;
uint8_t secondary_avail = 0;


uint8_t irq_h = 0;


inline void __parse_cmos(uint8_t primary, uint8_t secondary)
{
	if(primary != 0)
	{
		primary_avail = 1;
		mprint("Primary FDC channel available.\n");
	}
	if(secondary != 0)
	{
		secondary_avail = 1;
		mprint("Secondary FDC channel available.\n");
	}

}

inline uint8_t __read_status()
{
	return inportb(FLPYDSK_MSR);
}

void __write_dor(uint8_t cmd)
{
	outportb(FLPYDSK_DOR, cmd);
}

void __write_cmd(uint8_t cmd)
{
	uint8_t timeout = 0xff;
	while(--timeout) {
		if(__read_status() & FLPYDSK_MSR_MASK_DATAREG) {
			outportb(FLPYDSK_FIFO, cmd);
			return;
		}
	} 
	mprint("FATAL: timeout durint %s\n", __func__);
}

uint8_t __read_data()
{
	for(int i = 0; i < 500; i++)
		if(__read_status() & FLPYDSK_MSR_MASK_DATAREG)
			return inportb(FLPYDSK_FIFO);
	return 0;
}

void __write_ccr(uint8_t cmd)
{
	outportb(FLPYDSK_CTRL, cmd);
}

void fdc_disable_controller()
{
	__write_dor(0);
}

void fdc_enable_controller()
{
	__write_dor(FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA);
}

void fdc_dma_init(uint8_t* buffer, uint32_t length)
{
	union {
		uint8_t byte[4];
		unsigned long l;
	}a, c;
	a.l = (unsigned) buffer;
	c.l = (unsigned) length -1;

	dma_reset (1);
	dma_mask_channel( FDC_DMA_CHANNEL );//Mask channel 2
	dma_reset_flipflop ( 1 );//Flipflop reset on DMA 1

	dma_set_address( FDC_DMA_CHANNEL, a.byte[0],a.byte[1]);//Buffer address
	dma_reset_flipflop( 1 );//Flipflop reset on DMA 1

	dma_set_count( FDC_DMA_CHANNEL, c.byte[0],c.byte[1]);//Set count
	dma_set_read ( FDC_DMA_CHANNEL );

	dma_unmask_all( 1 );//Unmask channel 2
}

void fdc_check_int(uint32_t* st0, uint32_t* cy1)
{
	__write_cmd(FDC_CMD_CHECK_INT);
	*st0 = __read_data();
	*cy1 = __read_data();
}

uint8_t __wait_for_irq()
{
	uint32_t timeout = 10;
	uint8_t ret = 0;
	while(!irq_h)
	{
		if(!timeout) break;
		timeout--;
		schedule_noirq();
	}
	if(irq_h) ret = 1;
	irq_h = 0;
	return ret;
}

void fdc_set_motor(uint8_t drive, uint8_t status)
{
	if(drive > 3)
	{
		BUG("Invalid drive selected.\n");
		return;
	}
	uint8_t motor = 0;
	switch(drive)
	{
		case 0:
			motor = FLPYDSK_DOR_MASK_DRIVE0_MOTOR;
			break;
		case 1:
			motor = FLPYDSK_DOR_MASK_DRIVE1_MOTOR;
			break;
		case 2:
			motor = FLPYDSK_DOR_MASK_DRIVE2_MOTOR;
			break;
		case 3:
			motor = FLPYDSK_DOR_MASK_DRIVE3_MOTOR;
			break;
	}
	if(status) {
		__write_dor(drive | motor | FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA);
	}
	else {
		__write_dor(FLPYDSK_DOR_MASK_RESET);
	}
}

void fdc_drive_set(uint8_t step, uint8_t loadt, uint8_t unloadt, uint8_t dma)
{
	mprint("%s: steprate: %dms, load: %dms, unload: %dms, dma:%d\n", __func__,
			step, loadt, unloadt, dma);
	__write_cmd(FDC_CMD_SPECIFY);
	uint8_t data = 0;
	data = ((step&0xf)<<4) | (unloadt & 0xf);
	__write_cmd(data);
	data = ((loadt << 1) | (dma?0:1));
	__write_cmd(data);
}

void fdc_calibrate(uint8_t drive)
{
	uint32_t st0, cy1;
	
	fdc_set_motor(drive, 1);
	for(int i = 0; i < 10; i++)
	{
		__write_cmd(FDC_CMD_CALIBRATE);
		__write_cmd(drive);
		if(!__wait_for_irq())
		{
			goto fail;
		}
		fdc_check_int(&st0, &cy1);
		if(!cy1)
		{
			fdc_set_motor(drive, 0);
			mprint("Calibration successful!\n");
			return;
		}
	}
fail: fdc_set_motor(drive, 0);
	mprint("Unable to calibrate!\n");
}

void flpy_irq()
{
	//asm volatile("sub $0x1c, %esp");
	IRQ_START;
	irq_h = 1;
	send_eoi(6);
	IRQ_END;
}

uint8_t fdc_seek(uint8_t cyl, uint8_t head)
{
	uint32_t st0, cy10;

	for(int i = 0;i < 10; i++)
	{
		__write_cmd(FDC_CMD_SEEK);
		__write_cmd((head<<2) | 0);
		__write_cmd(cyl);

		if(!__wait_for_irq())
		{
			mprint("FATAL: IRQ link offline! %s\n", __func__);
			return 1;
		}
		fdc_check_int(&st0, &cy10);
		if(cy10 == cyl)
			return 0;
	}
	return 1;
}

uint8_t flpy_read_sector(uint8_t head, uint8_t track, uint8_t sector)
{
		uint32_t st0, cy1;

		fdc_dma_init((uint8_t *)DMA_BUFFER, 512);
		dma_set_read(FDC_DMA_CHANNEL);
		__write_cmd(FDC_CMD_READ_SECT|FDC_CMD_EXT_MULTITRACK|FDC_CMD_EXT_SKIP|FDC_CMD_EXT_DENSITY);
		__write_cmd((head << 2)| 0);
		__write_cmd(track);
		__write_cmd(head);
		__write_cmd(sector);
		__write_cmd(FLPYDSK_SECTOR_DTL_512);
		__write_cmd( ((sector+1)>=FLPY_SECTORS_PER_TRACK)?FLPY_SECTORS_PER_TRACK:(sector+1));
		__write_cmd(FLPYDSK_GAP3_LENGTH_3_5);
		__write_cmd(0xff);

		if(!__wait_for_irq())
		{
			mprint("FATAL: IRQ link offline! %s\n", __func__);
			return 1;
		}
		for(int j = 0; j < 7; j++)
			__read_data();

		fdc_check_int(&st0, &cy1);
		return 1;
}

void __lba_to_chs(int lba, int *head, int *track, int *sector)
{
	*head = (lba % (FLPY_SECTORS_PER_TRACK * 2) ) / FLPY_SECTORS_PER_TRACK;
	*track = lba / (FLPY_SECTORS_PER_TRACK * 2);
	*sector = lba % FLPY_SECTORS_PER_TRACK + 1;
}

uint8_t* flpy_read_lba(int lba)
{
	int head = 0, track = 0, sector = 1;
	int rc = 0;
	__lba_to_chs(lba,  &head, &track, &sector);

	fdc_set_motor(0, 1);
	rc = fdc_seek(track, head);
	if(rc) {
		BUG("Failed to seek!\n");
		return 0;
	}

	flpy_read_sector(head, track, sector);
	fdc_set_motor(0, 0);

	return (uint8_t *)DMA_BUFFER;

}

uint8_t flpy_read(uint8_t* buffer, uint32_t lba, uint32_t sectors)
{
	if(!sectors) return 1;
	uint32_t sectors_read = 0;
	while(sectors_read != sectors)
	{
		uint8_t *buf = flpy_read_lba(lba + sectors_read);
		if(!buf) return 1;
		memcpy(buffer + sectors_read*512, buf, 512);
		sectors_read++;
	}
	return 0;
}

uint8_t flpy_write_lba(uint8_t *buf, uint32_t lba)
{
	int head = 0, track = 0, sector = 1;
	__lba_to_chs(lba, &head, &track, &sector);
	fdc_set_motor(0, 1);
	int rc = fdc_seek(track, head);
	if(rc)
	{
		mprint("Failed to seek for write!\n");
		return 0;
	}

	memcpy((uint8_t *)DMA_BUFFER, buf, 512);

	uint32_t st0, cy1;

	fdc_dma_init((uint8_t *)DMA_BUFFER, 512);
	dma_set_write(FDC_DMA_CHANNEL);
	__write_cmd(FDC_CMD_WRITE_SECT|FDC_CMD_EXT_MULTITRACK|FDC_CMD_EXT_SKIP|FDC_CMD_EXT_DENSITY);
	__write_cmd((head << 2)| 0);
	__write_cmd(track);
	__write_cmd(head);
	__write_cmd(sector);
	__write_cmd(FLPYDSK_SECTOR_DTL_512);
	__write_cmd( ((sector+1)>=FLPY_SECTORS_PER_TRACK)?FLPY_SECTORS_PER_TRACK:(sector+1));
	__write_cmd(FLPYDSK_GAP3_LENGTH_3_5);
	__write_cmd(0xff);

	if(!__wait_for_irq())
	{
		mprint("FATAL: IRQ link offline! %s\n", __func__);
		return 1;
	}
	for(int j = 0; j < 7; j++)
		__read_data();

	fdc_check_int(&st0, &cy1);
	return 1;

}

uint8_t flpy_write(uint8_t *buffer, uint32_t lba, uint32_t sectors)
{
	if(!sectors) return 0;
	uint32_t sectors_wrote = 0;
	while(sectors_wrote != sectors)
	{
		if(!flpy_write_lba(buffer + sectors_wrote * 512, lba + sectors_wrote)) return 1;
		sectors_wrote++;
	}
	return 1;
}

void fdc_reset()
{
	fdc_disable_controller();
	fdc_enable_controller();
	if(!__wait_for_irq())
	{
		mprint("FATAL: IRQ link offline. %s\n", __func__);
		return;
	}
	mprint("IRQ link is online.\n");
	uint32_t st0, cy1;
	for(int i = 0; i < 4; i++)
		fdc_check_int(&st0, &cy1);

	mprint("Setting transfer speed to 500Kb/s\n");
	__write_ccr(0);

	fdc_calibrate(0);

	fdc_drive_set(3, 16, 240, 1);

	device_t *floppy = (device_t *)malloc(sizeof(floppy));
	floppy->name = "FLOPPY";
	floppy->unique_id = 0x13;
	floppy->dev_type = DEVICE_BLOCK;
	floppy->read = flpy_read;
	floppy->write = flpy_write;
	device_add(floppy);
}

void fdc_init()
{
	mprint("Looking for floppy devices\n");
	outportb(0x70, 0x10);
	uint8_t cmos = inportb(0x71);
	__parse_cmos((cmos&0xf0) >> 4, cmos&0x0f);
	if(!primary_avail) goto exit;
	mprint("Registering FDC IRQ\n");
	set_int(38, (uint32_t)flpy_irq);
	mprint("Resetting controller.\n");
	fdc_reset();
	mprint("Floppy is now usable.\n");
exit:	_kill();
}
