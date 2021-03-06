/** @author Levente Kurusa <levex@linux.com> **/
#include "../../include/display.h"
#include "dispi_textmode.h"
#include "../../include/string.h"
#include "../../include/mutex.h"
#include "../../include/memory.h"

static DISPLAY d = {0}; // we don't have a mm yet, must reserve static BSS space for it.

inline uint8_t __textmode_make_color(uint8_t fg, uint8_t bg)
{
	return fg | bg << 4;
}

inline uint16_t __textmode_create_entry(char c, uint8_t color)
{
	uint16_t _char = (uint16_t)c;
	uint16_t _color = (uint16_t)color;
	return _char | _color << 8;
}

DEFINE_MUTEX(m_scroll);
static inline void __textmode_scrollup()
{
	mutex_lock(&m_scroll);
	for(int y = 0; y < TEXTMODE_HEIGHT; y++)
	{
		memcpy((char *)(VGA_MEMORY + y*TEXTMODE_WIDTH*2) ,
			(char *)(VGA_MEMORY + (y+1)*TEXTMODE_WIDTH*2),
			TEXTMODE_WIDTH*2);
	}
	uint8_t entity = __textmode_make_color(d.con.fgcol, d.con.bgcol);
	memset16((char *)(VGA_MEMORY + TEXTMODE_HEIGHT*TEXTMODE_WIDTH*2), __textmode_create_entry(' ', entity), TEXTMODE_WIDTH*2);
	mutex_unlock(&m_scroll);
}

void __textmode_onregister()
{
	/* textmode requires no setup */
	return;
}

void __textmode_onset(uint8_t id)
{
	/* When set, clear the screen and output init message */
	d.clear();
	/* no kprintf() yet, so *hackish* methods beware! */
	d.puts("Textmode driver initialized as display ");
	d.putc(id + 48); // convert to ASCII *hackish*
	d.putc('\n');
	return;
}

DISPLAY* textmode_init()
{
	/* create the DISPLAY variable */
	d.width = TEXTMODE_WIDTH;
	d.height = TEXTMODE_HEIGHT;
	d.con.cx = 0;
	d.con.cy = 0;
	d.con.fgcol = COLOR_WHITE;
	d.con.bgcol = COLOR_BLACK;
	d.puts = textmode_puts;
	d.putc = textmode_putc;
	d.clear = textmode_clear;
	d.onregister = __textmode_onregister;
	d.onset = __textmode_onset;
	return &d;
}

void textmode_clear()
{
	/* Clear the screen, by printing spaces everywhere. */
	uint8_t entity = __textmode_make_color(d.con.fgcol, d.con.bgcol);
	for(size_t y = 0; y < TEXTMODE_HEIGHT; y++)
		for(size_t x = 0; x < TEXTMODE_WIDTH; x++)
		{
			const size_t index = y * 2 * TEXTMODE_WIDTH + x * 2;
			*(uint16_t*)(VGA_MEMORY + index) = __textmode_create_entry(' ', entity);
		}
	/* Reset the console coordinates */
	d.con.cx = 0;
	d.con.cy = 0;
}

void textmode_puts(string s)
{
	/* loop thru the bytes, and print it! */
	while(*s != 0) textmode_putc(*s++);
}
void textmode_putc(char c)
{
	if(!c) return;
	/* check for line wrapping and LF character */
	if(d.con.cx >= TEXTMODE_WIDTH || c == '\n')
	{
		d.con.cx = 0;
		d.con.cy ++;
	}
	if(d.con.cy >= TEXTMODE_HEIGHT-1)
	{
		__textmode_scrollup();
		d.con.cy --;
	}
	/* list of special characters */
	if(c == '\n') return;
	/* write it to video memory */
	const size_t index = d.con.cy * 2 * TEXTMODE_WIDTH + d.con.cx * 2;
	*(uint16_t*)(VGA_MEMORY + index) = __textmode_create_entry(c, __textmode_make_color(d.con.fgcol, d.con.bgcol));
	/* increment current x position */
	d.con.cx ++;
}
