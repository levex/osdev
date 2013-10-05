compile: assembly bkernel link
all: assembly bkernel link start

start:
	qemu -kernel levos.bin

assembly:
	i586-elf-as boot.s -o boot.o

bkernel:
	if [[ -e "objs.txt" ]]; then rm objs.txt; fi;
	i586-elf-gcc -c main.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	echo -n "boot.o kernel.o " >> objs.txt
	cd display && $(MAKE) $(MFLAGS)
	cd lib && $(MAKE) $(MFLAGS)
	cd memory && $(MAKE) $(MFLAGS)
	cd arch && $(MAKE) $(MFLAGS)
	cd kernel && $(MAKE) $(MFLAGS)

objs = `cat objs.txt`
link:
	i586-elf-gcc -T linker.ld -o levos.bin -ffreestanding -O2 -nostdlib $(objs)  -lgcc
	rm objs.txt

clean:
	rm objs.txt
