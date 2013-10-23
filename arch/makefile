all:
	echo -n "arch/gdt.o arch/idt.o arch/v86c.o arch/exceptions.o " >> ../objs.txt
	i586-elf-gcc -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c idt.c -o idt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c v86.c -o v86c.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c exceptions.c -o exceptions.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
