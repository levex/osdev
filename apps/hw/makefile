apps:
	i586-elf-gcc -c hw.c -o hw.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -T link.ld -o hw -ffreestanding -O2 -nostdlib hw.o  -lgcc
