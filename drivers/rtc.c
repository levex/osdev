/** @author Levente Kurusa <levex@linux.com> **/
#include "../include/rtc.h"
#include "../include/display.h"
#include "../include/hal.h"
#include "../include/tasking.h"

MODULE("RTC");


void rtc_init()
{
	mprint("Loading RTC\n");
	rtc_print_time();
	mprint("RTC functioning properly.\n");
	_kill();
}

uint8_t rtc_get_year()
{
	outportb(0x70, 0x09);
	return inportb(0x71);
}

uint8_t rtc_get_month()
{
	outportb(0x70, 0x08);
	return inportb(0x71);
}

uint8_t rtc_get_day()
{
	outportb(0x70, 0x07);
	return inportb(0x71);
}

uint8_t rtc_get_weekday()
{
	outportb(0x70, 0x06);
	return inportb(0x71);
}

uint8_t rtc_get_hour()
{
	outportb(0x70, 0x04);
	return inportb(0x71);
}

uint8_t rtc_get_minute()
{
	outportb(0x70, 0x02);
	return inportb(0x71);
}

uint8_t rtc_get_second()
{
	outportb(0x70, 0x00);
	return inportb(0x71);
}

void rtc_print_time()
{
	kprintf("20%x, %x. %x. %x:%x:%x\n",
			rtc_get_year(), rtc_get_month(), rtc_get_day(),
			rtc_get_hour(), rtc_get_minute(), rtc_get_second());
}

void rtc_print_time_as_proc()
{
	rtc_print_time();
	_kill();
}
