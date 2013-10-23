all:
	echo -n "drivers/keyboard.o drivers/device.o drivers/rtc.o drivers/fdc.o drivers/dma.o " >> ../objs.txt
	i586-elf-gcc -c keyboard.c -o keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c device.c -o device.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c rtc.c -o rtc.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c fdc.c -o fdc.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c dma.c -o dma.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
