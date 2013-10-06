all:
	echo -n "kernel/pic.o kernel/pit.o kernel/hal.o kernel/tasking.o kernel/mutex.o " >> ../objs.txt
	i586-elf-gcc -c pic.c -o pic.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c pit.c -o pit.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c hal.c -o hal.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c tasking.c -o tasking.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	i586-elf-gcc -c mutex.c -o mutex.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
