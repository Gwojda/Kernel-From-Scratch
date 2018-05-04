all :
	nasm -f elf64 isofiles/boot/multiboot_header.asm
	nasm -f elf64 isofiles/boot/boot.asm
	i686-elf-gcc -c isofiles/boot/kernel.c -o isofiles/boot/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
	ld -n -o isofiles/boot/kernel.bin -T isofiles/boot/linker.ld isofiles/boot/multiboot_header.o isofiles/boot/boot.o
	sudo grub-mkrescue -o os.iso isofiles

run:
	sudo qemu-system-x86_64 -cdrom os.iso -curses
