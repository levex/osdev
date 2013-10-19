/** @author Levente Kurusa <levex@linux.com> **/
#ifndef __RTC_H_
#define __RTC_H_

#include <stdint.h>

extern void rtc_init();
extern void rtc_print_time();
extern void rtc_print_time_as_proc();

#endif