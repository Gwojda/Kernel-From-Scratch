all :
	nasm -f elf64 isofiles/boot/multiboot_header.asm
	nasm -f elf64 isofiles/boot/boot.asm
	ld -n -o kernel.bin -T isofiles/boot/linker.ld multiboot_header.o boot.o
	grub-mkrescue -o os.iso isofiles
