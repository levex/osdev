# Ezek a multiboot fejlec beallitasai, kesobb reszletesen foglalkozunk veluk
.set ALIGN,    1<<0             # modulok betoltese page(4k) aligned legyen
.set MEMINFO,  1<<1             # kerunk memory map-et
.set FLAGS,    ALIGN | MEMINFO  # most osszepreseljuk egy flags objektumba
.set MAGIC,    0x1BADB002       # 'magic number', ezzel fog megtalalni a GRUB
.set CHECKSUM, -(MAGIC + FLAGS) # checksum, ellenorzeskent


# Egy uj szekcio, amit a linker script majd jo elore tesz, elvileg.
.section .multiboot
.align 4 # 4byte boundary-n legyunk!
.long MAGIC # elsonek a MAGIC number
.long FLAGS # masodiknak a flags
.long CHECKSUM # majd vegul a checksum

# foglalunk a stack-nek 16KiB-ot
.section .bootstrap_stack
stack_bottom:
.skip 32768 # 16 KiB
stack_top:

# az elf-ben a .text szekcio tartalmazza a kodot amit futtatni fogunk
.section .text
.global _set_gdtr
.type _set_gdtr, @function
_set_gdtr:
	push %ebp
	movl %esp, %ebp

	lgdt 0x400000

	movl %ebp, %esp
	pop %ebp
	ret

.global _set_idtr
.type _set_idtr, @function
_set_idtr:
	push %ebp
	movl %esp, %ebp

	lidt 0x401F00

	movl %ebp, %esp
	pop %ebp
	ret

.global _reload_segments
.type _reload_segments, @function
_reload_segments:
	push %ebp
	movl %esp, %ebp

	push %eax
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	pop %eax

	ljmp $0x8, $me
me:
	movl %ebp, %esp
	pop %ebp
	ret

.global _start # ez exportalja a _start label-t
.type _start, @function # a _start label mostmar egy funkcio!
_start: # definialjuk a _start -ot

	movl $stack_top, %esp # gyors allitsuk be a stacket!
	mov $0x1337, %eax

	call kernel_main # es hivjuk meg a C99 resz! :)

	cli # kikapcsoljuk az interruptokat
	hlt # halt a kovetkezo interruptig
	# az elozo ket sor, orokre lefagyasztja a gepet, innen marcsak a reboot segit!
.Lhang:  # ha GPF vagy PF lenne, akkor sem hagyjuk a gepet elfutni a rossz memoriaba!
	jmp .Lhang # inkabb ugraljon orokre itt (~spinlock)


.size _start, . - _start

