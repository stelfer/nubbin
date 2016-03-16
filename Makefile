#Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt


TARGET_CCWARN 		+= -Wno-unused-function -Wno-unused-variable -Wno-macro-redefined
TARGET_CCFLAGS		+= -nostdlib -fno-builtin -fno-stack-protector -ffreestanding

QEMU			:= /usr/local/bin/qemu-system-x86_64
QEMU_IMAGE_ARGS		:= -boot order=a -fda
QEMU_KERNEL_ARGS	:= -kernel

KERNEL_OBJS 		:= 				\
			build/nubbin/kernel/gdt.o	\
			build/nubbin/kernel/kernel.o	\
			build/nubbin/kernel/low_level.o	\
			build/nubbin/kernel/mem.o

OS_IMAGE		:= build/nubbin/os-image

KERNEL			:= build/nubbin/kernel/asm/kernel.$(TARGET_FORMAT)
KERNEL_BIN		:= build/nubbin/kernel/asm/kernel.bin
BOOT_LOADER		:= build/nubbin/kernel/asm/boot.bin

nubbin-clean:
	rm -f $(OS_IMAGE) $(KERNEL_OBJS) $(patsubst %.o,%.d,$(KERNEL_OBJS)) $(KERNEL) $(BOOT_LOADER) $(KERNEL_BIN)

TESTS		:= 

$(KERNEL): $(KERNEL_OBJS)

$(KERNEL_BIN) : $(KERNEL)
	$(TARGET_OBJCOPY) -O binary $< $@

$(OS_IMAGE) : $(BOOT_LOADER) $(KERNEL_BIN)
	cat $^ > $@


.PHONY: run-image
run-image: $(OS_IMAGE)
	$(QEMU) $(QEMU_IMAGE_ARGS) $<

.PHONY: run-kernel
run-kernel: $(KERNEL)
	$(QEMU) $(QEMU_KERNEL_ARGS) $<

