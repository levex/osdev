/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/x86/idt.h"
#include "../include/display.h"
#include "../include/hal.h"
#include "../include/tasking.h"
#include "../include/keyboard.h"

MODULE("KBD");

void keyboard_irq();

uint8_t lastkey = 0;
uint8_t *keycache = 0;

enum KEYCODE {
	NULL_KEY = 0,
	Q_PRESSED = 0x10,
	Q_RELEASED = 0x90,
	W_PRESSED = 0x11,
	W_RELEASED = 0x91,
	E_PRESSED = 0x12,
	E_RELEASED = 0x92,
	R_PRESSED = 0x13,
	R_RELEASED = 0x93,
	T_PRESSED = 0x14,
	T_RELEASED = 0x94,
	Z_PRESSED = 0x15,
	Z_RELEASED = 0x95,
	U_PRESSED = 0x16,
	U_RELEASED = 0x96,
	I_PRESSED = 0x17,
	I_RELEASED = 0x97,
	O_PRESSED = 0x18,
	O_RELEASED = 0x98,
	P_PRESSED = 0x19,
	P_RELEASED = 0x99,
	A_PRESSED = 0x1E,
	A_RELEASED = 0x9E,
	S_PRESSED = 0x1F,
	S_RELEASED = 0x9F,
	D_PRESSED = 0x20,
	D_RELEASED = 0xA0,
	F_PRESSED = 0x21,
	F_RELEASED = 0xA1,
	G_PRESSED = 0x22,
	G_RELEASED = 0xA2,
	H_PRESSED = 0x23,
	H_RELEASED = 0xA3,
	J_PRESSED = 0x24,
	J_RELEASED = 0xA4,
	K_PRESSED = 0x25,
	K_RELEASED = 0xA5,
	L_PRESSED = 0x26,
	L_RELEASED = 0xA6,
	Y_PRESSED = 0x2C,
	Y_RELEASED = 0xAC,
	X_PRESSED = 0x2D,
	X_RELEASED = 0xAD,
	C_PRESSED = 0x2E,
	C_RELEASED = 0xAE,
	V_PRESSED = 0x2F,
	V_RELEASED = 0xAF,
	B_PRESSED = 0x30,
	B_RELEASED = 0xB0,
	N_PRESSED = 0x31,
	N_RELEASED = 0xB1,
	M_PRESSED = 0x32,
	M_RELEASED = 0xB2,

	ENTER_PRESSED = 0x1C,
	ENTER_RELEASED = 0x9C,

};

#define KBD_SEND(byt) outportb(0x64, byt);

/* This is a late init, must end with _kill() */
void keyboard_init()
{
	mprint("Driver init.\n");
	/* Install IRQ */
	set_int(33, keyboard_irq);
	_kill(); /* end me */
}

void keyboard_read_key()
{
	lastkey = 0;
	if(inportb(0x64) & 1)
		lastkey = inportb(0x60);
}

char keyboard_get_key()
{
	keyboard_read_key();
	if(lastkey == 0) return 0;
	char c = keyboard_to_ascii(lastkey);
	return c;
}
static char* _qwertzuiop = "qwertzuiop"; // 0x10-0x1c
static char* _asdfghjkl = "asdfghjkl";
static char* _yxcvbnm = "yxcvbnm";
uint8_t keyboard_to_ascii(uint8_t key)
{
	if(key == 0x1C) return '\n';
	if(key >= 0x10 && key <= 0x1C)
	{
		return _qwertzuiop[key - 0x10];
	} else if(key >= 0x1E && key <= 0x26)
	{
		return _asdfghjkl[key - 0x1E];
	} else if(key >= 0x2C && key <= 0x32)
	{
		return _yxcvbnm[key - 0x2C];
	}
	return 0;
}

void keyboard_irq()
{
	IRQ_START;
	send_eoi(1);
	IRQ_END;
}
