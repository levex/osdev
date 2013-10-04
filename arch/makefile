all:
	echo -n "arch/gdt.o " >> ../objs.txt
	i586-elf-gcc -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
