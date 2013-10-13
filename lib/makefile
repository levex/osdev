export objs += string.o

all:
	echo -n "lib/string.o lib/buffer_util.o " >> ../objs.txt
	i586-elf-gcc -c string.c -o string.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c buffer_util.c -o buffer_util.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
