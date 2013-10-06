all:
	echo -n "memory/memutils.o memory/malloc.o memory/paging.o " >> ../objs.txt
	i586-elf-gcc -c memutils.c -o memutils.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c malloc.c -o malloc.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c paging.c -o paging.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
