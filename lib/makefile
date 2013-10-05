export objs += string.o

all:
	echo -n "lib/string.o " >> ../objs.txt
	i586-elf-gcc -c string.c -o string.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
