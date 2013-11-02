all: module
	echo -n "exec/module.o exec/elfloader.o exec/loader.o exec/syscalls.o " >> ../objs.txt
	i586-elf-gcc -c module.c -o module.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c elfloader.c -o elfloader.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c loader.c -o loader.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c syscalls.c -o syscalls.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
module:
	i586-elf-gcc -c testmod.c -o test.ko -g -std=gnu99 -ffreestanding -O2 -Wall -Wextra
