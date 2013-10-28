all:
	echo -n "fs/vfs.o " >> ../objs.txt
	i586-elf-gcc -c vfs.c -o vfs.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	cd ext2 && $(MAKE) $(MFLAGS)
	cd proc && $(MAKE) $(MFLAGS)
