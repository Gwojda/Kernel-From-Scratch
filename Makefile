ARCH =		x86
KERNEL =	KKERNEL
ISO =		kkernel.iso

## Compilers
CC =		gcc
ASM =		nasm
LD =		ld

## Includes
INCS =	-I inc/

## Flags
CFLAGS =	-std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 $(INCS) \
 			-fno-builtin -fno-exception -fno-stack-protector -fno-rtti -nostdlib -nodefaultlibs
LDFLAG =	-melf_i386 -static --entry=_start -T $(LINKER)
ASMFLAGS =	-f elf -o 

include ./boot/$(ARCH)/Makefile
include ./kernel/Makefile
include ./lib/Makefile

TOTAL=		$(shell echo $(OBJS) | sed 's/ /\n/g' | wc -l)
CURRENT=	01

all: $(KERNEL)

$(KERNEL): $(OBJS)
	@echo "Linking kernel to $@..."
	@$(LD) $(LDFLAG) -o ISO/boot/$@ $^
	@echo "Compilation done for $(KERNEL)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[$(shell printf "%02d" $(CURRENT))/$(TOTAL)]\tCompiling (C) $@..."
	$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@$(ASM) $(ASMFLAGS) -c $< -o $@
	@echo "[$(shell printf "%02d" $(CURRENT))/$(TOTAL)]\tCompiling (ASM) $@..."
	$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))

install: $(KERNEL)
	@echo "Installing kernel image..."
	@sudo grub-mkrescue -o $(ISO) ISO/
	@echo "Launching KVM..."
	@sudo qemu-system-i386 -s -cdrom $(ISO) -curses

clean:
	@rm -f $(OBJS)
	@echo "Cleaning objects..."

fclean: clean
	@rm -f ISO/boot/$(KERNEL)
	@rm -f $(ISO)
	@echo "Cleaning Kernel..."
