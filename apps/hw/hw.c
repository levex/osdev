void main()
{
	char *text = "Hello, world!\n";
	int file = -1;
	/* syscall 1: open() eax=1 ebx=stdout(1) RET: eax=file*/
	asm volatile("int $0x80":"=a"(file):"a"(1), "b"(1));
	/* syscall 2: write() eax=2 ebx=buffer ecx=file RET: none*/
	asm volatile("int $0x80": :"a"(2), "b"(text), "c"(file));
	/* syscall 0: exit() eax=0 RET: none */
	asm volatile("int $0x80": :"a"(0));
	return;
}
