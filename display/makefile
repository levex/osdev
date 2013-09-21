all:
	echo -n "display/display.o " >> ../objs.txt
	i586-elf-gcc -c display.c -o display.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	cd textmode && $(MAKE) $(MFLAGS) 
