ARCH =		x86
KERNEL =	KKERNEL

## Compilers
CC =		gcc
ASM =		nasm
LD =		ld

## Includes
INCS =	-I inc/

## Flags
CFLAGS =	-std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32 $(INCS)
LDFLAG =	-melf_i386 -static --entry=_start -o -T isofiles/boot/linker.ld
ASMFLAGS =	-f elf -o

include ./boot/$(ARCH)/Makefile
include ./kernel/Makefile
include ./lib/Makefile

TOTAL=		$(shell echo $(OBJS) | sed 's/ /\n/g' | wc -l)
CURRENT=	01

all: $(KERNEL)

$(KERNEL): $(OBJS)
	@echo "Linking kernel to $@..."
	@$(LD) $(LDFLAG) -o boot/$@ $^
	@echo "Compilation done for $(KERNEL)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "[$(shell printf "%02d" $(CURRENT))/$(TOTAL)]\tCompiling (C++) $@..."
	$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	@$(ASM) $(ASMFLAGS) -c $< -o $@
	@echo "[$(shell printf "%02d" $(CURRENT))/$(TOTAL)]\tCompiling (ASM) $@..."
	$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))

install: $(KERNEL)
	@echo "Installing kernel image..."
	@sudo grub-mkrescue -o os.iso .
	@echo "Launching KVM..."
	@sudo qemu-system-x86_64 -cdrom os.iso -curses

clean:
	@rm -f $(OBJS)
	@echo "Cleaning objects..."

fclean: clean
	@rm -f $(KERNEL)
	@echo "Cleaning Kernel..."
