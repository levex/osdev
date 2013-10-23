compile: assembly bkernel link
all: assembly bkernel link start

mount:
	echo "Mounting fda.img as /mnt/floppy..."
	sudo mount -o loop fda.img /mnt/floppy
	echo "Done."

start:
	qemu -kernel levos.bin -fda fda.img -monitor /dev/stdout
	reset

assembly:
	i586-elf-as boot.s -o boot.o
	i586-elf-as v86.s -o v86.o

bkernel:
	if [[ -e "objs.txt" ]]; then rm objs.txt; fi;
	i586-elf-gcc -c main.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	echo -n "boot.o kernel.o v86.o " >> objs.txt
	cd display && $(MAKE) $(MFLAGS)
	cd lib && $(MAKE) $(MFLAGS)
	cd memory && $(MAKE) $(MFLAGS)
	cd arch && $(MAKE) $(MFLAGS)
	cd kernel && $(MAKE) $(MFLAGS)
	cd drivers && $(MAKE) $(MFLAGS)
	cd fs && $(MAKE) $(MFLAGS)

objs = `cat objs.txt`
link:
	i586-elf-gcc -T linker.ld -o levos.bin -ffreestanding -O2 -nostdlib $(objs)  -lgcc
	rm objs.txt

clean:
	rm objs.txt
