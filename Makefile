QEMU=/usr/local/bin/qemu-system-i386
QEMU_ARGS=-boot order=a -fda 

TOOLCHAIN=toolchain
TOOLCHAIN_BIN=$(TOOLCHAIN)/bin

NASM=$(TOOLCHAIN_BIN)/nasm
NDIASM=$(TOOLCHAIN_BIN) -b16 -o7c00h -a -s7c3eh

CC=$(TOOLCHAIN_BIN)/i686-elf-gcc
LD=$(TOOLCHAIN_BIN)/i686-elf-ld
STRIP=$(TOOLCHAIN_BIN)/i686-elf-strip

ASM_SOURCES = 					\
	kernel/print_string.asm \
	kernel/print_hex.asm \
	kernel/disk_load.asm \
	kernel/print_string_pm.asm\
	kernel/gdt.asm\
	kernel/switch_to_pm.asm

NASM +=  -Ikernel/

C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)
OBJS = ${C_SOURCES:.c=.o}

ALL=os-image

all: $(ALL)

.PHONY: run
run: $(ALL)
	$(QEMU) $(QEMU_ARGS) $<

.PHONY: dis
dis: kernel/boot_sect.bin
	$(NDIASM) $<

kernel/boot_sect.bin: kernel/boot_sect.asm $(ASM) 
	$(NASM) $< -f bin -o $@

kernel/kernel.bin: kernel/kernel_entry.o $(OBJS)
	$(LD) -m elf_i386 -o $@ -Tkernel/link.ld $^ --oformat binary 

kernel/kernel_entry.o: kernel/kernel_entry.asm
	$(NASM) -f elf32 -o $@ $<

os-image: kernel/boot_sect.bin kernel/kernel.bin
	cat $^ > $@

%.o : %.c $(HEADERS)
	$(CC) -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -ffreestanding -c $< -o $@

clean:
	rm -f *.bin *~ *.o $(ALL)
	rm -f $(OBJS) kernel/*.bin

