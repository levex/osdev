/** @author Levente Kurusa <levex@linux.com> **/
#include <stdint.h>
#include "../include/display.h"
#include "../include/memory.h"
#include "../include/tasking.h"
#include "../include/pit.h"
#include "../include/levos.h"
#include "../include/module.h"
#include "../include/hal.h"
#include "../include/elf.h"
#include "../include/loader.h"

MODULE("ELF");


elf_priv_data *elf_probe(uint8_t *buffer)
{

	elf_header_t *header = (elf_header_t *)buffer;
	/* The first four bytes are 0x7f and 'ELF' */
	if(header->e_ident[0] == 0x7f && 
		header->e_ident[1] == 'E' && header->e_ident[2] == 'L' && header->e_ident[3] == 'F')
	{
		/* Valid ELF! */
		mprint("Buffer is valid ELF file!\n");
		return (void *)1;
	}
	return 0;
}

uint8_t elf_start(uint8_t *buf, elf_priv_data *priv UNUSED)
{
	elf_header_t *header = (elf_header_t *)buf;
	mprint("Type: %s%s%s\n",
		header->e_ident[4] == 1 ? "32bit ":"64 bit",
		header->e_ident[5] == 1 ? "Little Endian ":"Big endian ",
		header->e_ident[6] == 1 ? "True ELF ":"buggy ELF ");
	if(header->e_type != 2)
	{
		kprintf("File is not executable!\n");
		return 0;
	}
	/* Map the virtual space */
	uint32_t phys_loc = loader_get_unused_load_location();
	/* Find first program header and loop through them */
	elf_program_header_t *ph = (elf_program_header_t *)(buf + header->e_phoff);
	for(int i = 0; i < header->e_phnum; i++, ph++)
	{
		switch(ph->p_type)
		 {
		 	case 0: /* NULL */
		 		break;
		 	case 1: /* LOAD */
		 		mprint("LOAD: offset 0x%x vaddr 0x%x paddr 0x%x filesz 0x%x memsz 0x%x\n",
		 				ph->p_offset, ph->p_vaddr, ph->p_paddr, ph->p_filesz, ph->p_memsz);
		 		paging_map_virtual_to_phys(ph->p_vaddr, phys_loc);
		 		memcpy(ph->p_vaddr, buf + ph->p_offset, ph->p_filesz);
		 		break;
		 	default: /* @TODO add more */
		 	 kprintf("Unsupported p_type! Bail out!\n");
		 	 return 0;
		 }
	}
	/* Program loaded, jump to execution */
	return START("elf32", header->e_entry);
}

void elf_init()
{
	loader_t *elfloader = (loader_t *)malloc(sizeof(loader_t));
	elfloader->name = "ELF32";
	elfloader->probe = (void *)elf_probe;
	elfloader->start = (void *)elf_start;
	register_loader(elfloader);
	_kill();
}