#Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt


TARGET_CCWARN 		+= -Wno-unused-function -Wno-unused-variable -Wno-macro-redefined
TARGET_CCFLAGS		+= -nostdlib -fno-builtin -fno-stack-protector -ffreestanding

QEMU			:= /usr/local/bin/qemu-system-i386
QEMU_ARGS		:= -boot order=a -fda 

KERNEL_OBJS 		:= build/nubbin/kernel/gdt.o build/nubbin/kernel/kernel.o build/nubbin/kernel/low_level.o build/nubbin/kernel/mem.o

nubbin-clean:
	echo "CLEAN"

build/%.$(TARGET_ARCH): build/%.$(TARGET_ARCH).o $(KERNEL_OBJS) | %.elf_i386.ld $(DEPDIR)/%.d
	$(TARGET_LD) -m elf_i386 -T$(*).elf_i386.ld $^ $(KERNEL_LD_OPTS) -o $@ --oformat binary

build/nubbin/os-image : build/nubbin/kernel/asm/boot.bin build/nubbin/kernel/asm/kernel.$(TARGET_ARCH)
	cat $^ > $@

run-image: build/nubbin/os-image
	$(QEMU) $(QEMU_ARGS) $<

