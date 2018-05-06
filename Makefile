all :
	nasm -f elf isofiles/boot/multiboot_header.asm
	nasm -f elf isofiles/boot/boot.asm
	gcc -c isofiles/boot/kernel.c -o isofiles/boot/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32
	ld -melf_i386 -static --entry=_start -o isofiles/boot/kernel.bin -T isofiles/boot/linker.ld isofiles/boot/multiboot_header.o isofiles/boot/boot.o isofiles/boot/kernel.o
	sudo grub-mkrescue -o os.iso isofiles

run:
	sudo qemu-system-x86_64 -cdrom os.iso -curses
