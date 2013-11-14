/** @author Levente Kurusa <levex@linux.com> **/

#if !defined(__cplusplus)
#include <stdbool.h> /* C-ben alapbol nincsenek is bool-ok. */
#endif
#include <stddef.h>
#include <stdint.h>
 
#include "include/string.h"
#include "include/memory.h"
#include "include/display.h"
#include "display/textmode/dispi_textmode.h"
#include "include/x86/gdt.h"
#include "include/x86/idt.h"
#include "include/pit.h"
#include "include/pic.h"
#include "include/hal.h"
#include "include/tasking.h"
#include "include/levos.h"
#include "include/keyboard.h"
#include "include/device.h"
#include "include/rtc.h"
#include "include/x86/v86.h"
#include "include/floppy.h"
#include "include/ext2.h"
#include "include/proc.h"
#include "include/ata.h"
#include "include/vfs.h"
#include "include/proc.h"
#include "include/devfs.h"
#include "include/loader.h"
#include "include/module.h"
#include "include/elf.h"
#include "include/pci.h"

static DISPLAY* disp = 0;

MODULE("MAIN");

void _test();

int levex_id = 0;

extern uint32_t kernel_end;
extern uint32_t kernel_base;

static char* hostname = "levex-levos";
static char* username = "root";

void __login()
{
	uint8_t *buffer = (uint8_t*)malloc(64);
	int l = 0;
	char c = 0;
	kprintf("  _                 ____   _____ _  _   \n"
 			" | |               / __ \\ / ____| || |  \n"
 			" | |     _____   _| |  | | (___ | || |_  \n"
 			" | |    / _ \\ \\ / / |  | |\\___ \\|__   _|\n"
			" | |___|  __/\\ V /| |__| |____) |  | |  \n"
			" |______\\___| \\_/  \\____/|_____/   |_|  \n");
retry:
	memset(buffer, 0, 64);
	kprintf("\n%s login: ", hostname);
	c = 0;
	l = 0;
	while(1)
	{
		c = keyboard_get_key();
		if(!c) { schedule_noirq(); continue;}
		if(c == '\n')
		{
			if(strcmp((char *)buffer, "root") == 0)
			{
				break;
			}
			goto retry;
		}
		buffer[l++] = c;
		buffer[l] = 0;
		disp->putc(c);
	}
	memset(buffer, 0, 64);
	kprintf("\n%s@%s password: ", username, hostname);
	l = 0;
	c = 0;
	while(1)
	{
		c = keyboard_get_key();
		if(!c) { schedule_noirq(); continue;}
		if(c == '\n')
		{
			if(strcmp((char *)buffer, "toor") == 0)
			{
				disp->putc('\n');
				_kill();
			} else goto retry;
		}
		buffer[l] = c;
		buffer[l + 1] = 0;
		l++;
	}
}

void __cursor_updater()
{
	uint16_t lastpos = 0;
	while(1)
	{
		uint16_t pos = disp->con.cy*80 + disp->con.cx;
		if(pos == lastpos) { schedule_noirq(); continue; }
		lastpos = pos;
		outportb(0x3D4, 0x0F);
		outportb(0x3D5, (uint8_t)pos&0xFF);
		outportb(0x3D4, 0x0E);
		outportb(0x3D5, (uint8_t)(pos>>8)&0xFF);
		schedule_noirq();
	}
	_kill();
}

void test_device_read(uint8_t* buffer, uint32_t offset UNUSED, uint32_t len, device_t *dev UNUSED)
{
	len --;
	while(len--)
	{
		switch(len%5)
		{
			case 0:
				*buffer++ = 'X';
				break;
			case 1:
				*buffer++ = 'E';
				break;
			case 2:
				*buffer++ = 'V';
				break;
			case 3:
				*buffer++ = 'E';
				break;
			case 4:
				*buffer++ = 'L';
				break;
		}
	}
	*buffer = 0;
}

void create_test_device()
{
	device_t *testdev = 0;
	testdev = (device_t*)malloc(sizeof(device_t));
	testdev->name = "/dev/levex";
	testdev->unique_id = 0x1337;
	testdev->dev_type = DEVICE_CHAR;
	testdev->read = (uint8_t(*)(uint8_t*, uint32_t, uint32_t, device_t*))test_device_read;
	levex_id = device_add(testdev);
	_kill();
}

void __read()
{
	uint8_t* buffer = (uint8_t*)malloc(32);
	device_t *testdev = device_get(levex_id);
	testdev->read(buffer, 0, 32, testdev);
	kprintf("%s\n", buffer);
	_kill();
}

void __malloc_bug()
{
	malloc(0x1FFFFFFF);
	_kill();
}

void __ps()
{
	tasking_print_all();
	_kill();
}

void sig_test()
{
	asm volatile("movl $0, %eax\n"
				"idiv %eax");
}

#if defined(__cplusplus)
extern "C" /* ha C++ compiler, akkor ez a függvény legyen C99es linkage-ű. */
#endif
void kernel_main()
{
	int eax;
	asm("movl %%eax, %0": "=m"(eax));
	/* We create a textmode display driver, register it, then set it as current */
	display_setcurrent(display_register(textmode_init()));
	disp = display_getcurrent();
	mprint("Welcome to LevOS 4.0, very unstable!\n");
	mprint("Kernel base is 0x%x,  end is 0x%x\n", &kernel_base, &kernel_end);
	if(eax == 0x1337) { mprint("Assembly link established.\n"); }
	else panic("Couldn't establish assembly link!");
	/* Setup memory manager */
	mm_init(&kernel_end);
	/* So far good, we have a display running,
	 * but we are *very* early, set up arch-specific
	 * tables, timers and memory to continue to tasking.
	 */
	gdt_init();
	/* Now, we have the GDT setup, let's load the IDT as well */
	idt_init();
	exceptions_init();
	/* Next step, setup PIT. */
	hal_init();
	pic_init();
	pit_init();
	/* Enabling interrupts early... */
	asm volatile("sti");
	mprint("Interrupts were enabled, beware.\n");
	/* Setup paging. */
	paging_init();
	/* Good job ladies and gentleman, we are now alive.
	 * Safe and sound, on the way to tasking!
	 * Let's roll.
	 */
	syscall_init();
	tasking_init();
	/* Tasking should have started late_init(),
	 * if not, we are screwed as no peripherials are in
	 * an operating state. Tell the user that we ran away
	 * in terror.
	 */
	panic("Reached end of main(), but tasking was not started.");
	for(;;);
}

void __test()
{
	kprintf("Module system test.\n");
}
/* This function is ought to setup peripherials and such,
 * while also starting somekind of /bin/sh
 */
 int cu_pid = 0;
void late_init()
{
	/* From now, we are preemptible. Setup peripherials */
	int pid = 0;	
	pid = START("kbd_init", keyboard_init);
	pid = START("vfs_init", vfs_init);
	pid = START("loader_init", loader_init);
	cu_pid = START("cursor_update", __cursor_updater);
	pid = START("devicemm", device_init);
	while(is_pid_running(pid))schedule_noirq();
	pid = START("testdev", create_test_device);
	pid = START("elf_init", elf_init);
	pid = START("rtc_init", rtc_init);
	START_AND_WAIT("fdc_init", fdc_init);
	START_AND_WAIT("ata_init", ata_init);
	pid = START("mod_init", module_init);
	pid = START("pci_init", pci_init);


	/* We now wait till all the late_inits have finished */
	while(is_pid_running(pid))schedule_noirq();
	EXPORT_SYMBOL(__test);
	/* Once they are done, pass control to the kernel terminal,
	 * so that it will (eventually) start a /init or /bin/sh
	 */
	START_AND_WAIT("login", __login);
	/* We cannot die as we are the idle thread.
	 * schedule away so that we don't starve others
	 */
	while(1) {
		START_AND_WAIT("_test", _test);
		kerror("\n\nThe terminal has crashed. Restarting it...\n\n");
	}
	panic("Reached end of late_init()\n");
}
static char c = 0;
static char* buffer = 0;
static uint16_t loc = 0;
static char *wd = 0;
static int root_mounted = 0;
static char *ls_buffer = 0;
static uint8_t *file_buf = 0;
void _test()
{
	buffer = (char*)malloc(256);
	ls_buffer = (char *)malloc(1024);
	file_buf = (uint8_t *)malloc(524288);
	if(!file_buf) panic("Kernel terminal has no memory available!\n");
	uint8_t *write_buf = (uint8_t *)malloc(512);
	char* prompt = "(kernel) $ ";
	uint8_t prompt_size = strlen(prompt);
	kprintf("Welcome to LevOS 4.0\nThis is a very basic terminal.\nDon't do anything stupid.\n");
prompt:
	if(!root_mounted)
		kprintf(prompt);
	else
		kprintf("%s@%s:%s$ ", username, hostname, wd);
	memset(buffer, 0, 256);
	while(1) {
		if(!keyboard_enabled()){ schedule_noirq(); continue; }
		c = keyboard_get_key();
		if(!c) continue;
		if(c == '\r')
		{
			if(disp->con.cx <= prompt_size) continue;
			disp->con.cx --;
			disp->putc(' ');
			disp->con.cx --;
			buffer[loc--] = 0;
			continue;
		}
		if(c == '\n')
		{
			disp->putc(c);
			buffer[loc] = 0;
			loc = 0;
			uint32_t n = strsplit(buffer, ' ');
			if(strcmp(buffer, "help") == 0)
			{
				kprintf("LevOS4.0\nThis is the kernel terminal.\nDon't do anything stupid.\n");
				kprintf("Commands available: help; reboot; read; malloc; ps; fl; cd; ls\n"
						"clear; reset; time; v; kill\n");
				goto prompt;
			}
			if(strcmp(buffer, "reboot") == 0)
			{
				outportb(0x64, 0xFE);
				goto prompt;
			}
			if(strcmp(buffer, "read") == 0)
			{
				START_AND_WAIT("read", __read);
				goto prompt;
			}
			if(strcmp(buffer, "malloc") == 0)
			{
				uint8_t *mem = (uint8_t *)malloc(1337);
				free(mem);
				mem = (uint8_t *)malloc(1337);
				goto prompt;
			}
			if(strcmp(buffer, "mount") == 0)
			{
				list_mount();
				goto prompt;
				//kprintf("wd=0x%x", wd);
			}
			if(strcmp(buffer, "uname") == 0)
			{
				if(!root_mounted)
					goto prompt;
				vfs_read("/proc/os/full", file_buf);
				kprintf("%s\n", file_buf);
				memset(file_buf, 0, 512);
				goto prompt;
			}
			if(strcmp(buffer, "cat") == 0)
			{
				if(!n || n == 1)
				{
					kprintf("FATAL: no parameter.\n");
					goto prompt;
				}
				char *arg = (char *)(buffer + strlen(buffer) + 1);
				memset(file_buf, 0, 512);
				if(vfs_read(arg, file_buf))
				{
					kprintf("%s\n", file_buf);
				} else kprintf("File not found.\n");
				goto prompt;
			}
			if(strcmp(buffer, "ps") == 0)
			{
				START_AND_WAIT("ps", __ps);
				goto prompt;
			}
			if(strcmp(buffer, "time") == 0)
			{
				START_AND_WAIT("time", rtc_print_time_as_proc);
				goto prompt;
			}
			if(strcmp(buffer, "clear") == 0 || strcmp(buffer, "reset") == 0)
			{
				disp->clear();
				goto prompt;
			}
			if(strcmp(buffer, "v") == 0)
			{
				module_call_func("__test");
				goto prompt;
			}
			if(strcmp(buffer, "kill") == 0)
			{
				kill(cu_pid);
				while(is_pid_running(cu_pid)) {schedule_noirq(); continue;}
				goto prompt;
			}
			if(strcmp(buffer, "ls") == 0)
			{
				if(!root_mounted)
				{
					kprintf("FATAL: No root directory. Mount with 'fl'.\n");
					goto prompt;
				}
				if(!vfs_ls(wd, ls_buffer))
				{
					kprintf("Error.\n");
					goto prompt;
				}
				goto prompt;
			}
			if(strcmp(buffer, "mem") == 0)
			{
				mm_print_out();
				goto prompt;
			}
			if(strcmp(buffer, "cd") == 0)
			{
				if(!root_mounted)
				{
					kprintf("FATAL: No root directory. Mount with 'fl'.\n");
					goto prompt;
				}
				if(!n || n == 1)
				{
					kprintf("FATAL: no parameter.\n");
					goto prompt;
				}
				char *arg = (char *)(buffer + strlen(buffer) + 1);
				if(strcmp(arg, ".") == 0) goto prompt;
				if(strcmp(arg, "..") == 0)
				{
					if(strlen(wd) == 1)
					{
						goto prompt;
					} else {
						str_backspace(wd, '/');
						prompt_size = strlen(username) + strlen(hostname) + 4 + strlen(wd);
						goto prompt;
					}

				}
				if(vfs_exist_in_dir(wd, arg))
				{
					size_t size = strlen(wd) + strlen(arg) + 2;
					char *_w = (char *)malloc(size);
					memset(_w, 0, size);
					memcpy(_w, wd, strlen(wd));
					memcpy(_w + strlen(wd), arg, strlen(arg));
					memcpy(_w + strlen(_w), "/\0", 2);
					memcpy(wd, _w, strlen(_w) + 1);
					free(_w);
					prompt_size = strlen(username) + strlen(hostname) + 4 + strlen(wd);
				}
				goto prompt;
			}
			if(strcmp(buffer, "devinfo") == 0)
			{
				device_print_out();
				goto prompt;
			}
			if(strcmp(buffer, "fl") == 0)
			{
				if(!n || n == 1)
				{
					kprintf("FATAL: No id parameter, select root with devinfo\n");
					goto prompt;
				}
				char *arg = (char *)(buffer + strlen(buffer) + 1);
				int devid = 0;
				atoi(arg, &devid);
				kprintf("arg: %d\n", devid);
				device_t *dev = device_get(devid);
				if(!dev) {
					kprintf("Unable to locate medium!\n");
					goto prompt;
				}
				if(device_try_to_mount(dev, "/")) {
					kprintf("Mounted / on %s (%d) with %s\n", dev->name, dev->unique_id, dev->fs->name);
					root_mounted = 1;
					wd = (char *)malloc(512);
					memcpy(wd, "/", 2);
					prompt_size = strlen(username) + strlen(hostname) + 4 + strlen(wd);
					//goto prompt;
					device_try_to_mount(device_get_by_id(19), "/mnt/");
					START_AND_WAIT("proc_init", proc_init);
					START_AND_WAIT("devfs_init", devfs_init);
				}
				else kprintf("Unable to mount / on %s (%d)!\n", dev->name, dev->unique_id);
				goto prompt;

			}
			if(strcmp(buffer, "write") == 0)
			{
				if(!n || n == 1)
				{
					kprintf("FATAL: No argument supplied.\n");
					goto prompt;
				}
				device_t *dev = device_get_by_id(19);
				char *arg = (char *)(buffer + strlen(buffer) + 1);
				char *f = (char *)malloc(strlen(wd) + strlen(arg) + 1);
				memcpy(f, wd, strlen(wd));
				memcpy(f + strlen(wd), arg, strlen(arg) + 1);
				f[strlen(wd) + strlen(arg) + 1] = 0;
				kprintf("Writing 'levex is epic' to %s\n", f);
				char *my_buf = "levex is epic";
				dev->fs->writefile(f, my_buf, strlen(my_buf) + 1,  dev, dev->fs->priv_data);
				free(f);
				goto prompt;
			}
			if(strcmp(buffer, "lev") == 0)
			{
				if(vfs_read("/bin/hw", file_buf)) {
					int pid = 0;
					if(pid = exec_start(file_buf))
					{
						while(is_pid_running(pid)) schedule_noirq();
					}
				} else {
					kprintf("Unable to read /bin/hw\n");
				}
				goto prompt;
			}
			if(strcmp(buffer, "touch") == 0)
			{
				if(!n || n == 1)
				{
					kprintf("FATAL: No argument supplied.\n");
					goto prompt;
				}
				device_t *dev = device_get_by_id(19);
				char *arg = (char *)(buffer + strlen(buffer) + 1);
				char *f = (char *)malloc(strlen(wd) + strlen(arg) + 1);
				memcpy(f, wd, strlen(wd));
				memcpy(f + strlen(wd), arg, strlen(arg) + 1);
				f[strlen(wd) + strlen(arg) + 1] = 0;
				kprintf("Touching %s\n", f);
				dev->fs->touch(f, dev, dev->fs->priv_data);
				free(f);
				goto prompt;
			}
			if(!*buffer) goto prompt;
			if(vfs_read(buffer, file_buf))
			{
				int pid = 0;
				if(pid = exec_start(file_buf))
				{
					while(is_pid_running(pid)) schedule_noirq();
				}
			}
			goto prompt;
		}
		buffer[loc++] = c;
		buffer[loc] = 0;
		disp->putc(c);
	}
}
