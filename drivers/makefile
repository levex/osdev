all:
	echo -n "drivers/keyboard.o drivers/device.o drivers/rtc.o " >> ../objs.txt
	i586-elf-gcc -c keyboard.c -o keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c device.c -o device.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c rtc.c -o rtc.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
