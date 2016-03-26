#Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt


TARGET_CCWARN 		+= -Wno-unused-function -Wno-unused-variable -Wno-macro-redefined
TARGET_CCFLAGS		+= -ffreestanding

QEMU			:= /usr/local/bin/qemu-system-x86_64
#QEMU_NOGRAPHIC		:= -nographic
#QEMU_DEBUG		:= -s -S
QEMU_SERIAL		:= -serial mon:stdio
QEMU_IMAGE_ARGS		:= $(QEMU_DEBUG) $(QEMU_SERIAL) $(QEMU_NOGRAPHIC) -m 1G
#-boot a -fda
QEMU_KERNEL_ARGS	:= -kernel

KERNEL_OBJS 		:= 					\
			build/nubbin/kernel/kernel.o		\
			build/nubbin/kernel/io.o 		\
			build/nubbin/kernel/asm/boot.o		\
			build/nubbin/kernel/asm/setup16.o	\
			build/nubbin/kernel/asm/setup32.o	\
			build/nubbin/kernel/asm/ata.o		\
			build/nubbin/kernel/asm/setup64.o	\
			build/nubbin/kernel/console.o		\
			build/nubbin/kernel/serial.o		\
			build/nubbin/kernel/string.o

OS_IMAGE		:= build/nubbin/os-image

nubbin-clean:
	rm -f $(OS_IMAGE)
	rm -f $(KERNEL_OBJS)
	rm -f $(patsubst build/%.o,build/deps/%.d,$(KERNEL_OBJS))
	rm -f $(patsubst build/%.bin,build/deps/%.d,$(PM_START_BIN) $(RM_START)) 

TARGET_CCFLAGS		+= -mcmodel=large

TESTS		:= 

$(OS_IMAGE): $(KERNEL_OBJS)
	$(TARGET_LD) -m $(TARGET_LDEMU) -Tnubbin/kernel/asm/os-image.$(TARGET_LDEMU).ld $^ $(KERNEL_LD_OPTS) -o $@.elf64
	$(TARGET_OBJCOPY) -S -O binary $@.elf64 $@

.PHONY: run-image
run-image: $(OS_IMAGE)
	$(QEMU) $(QEMU_IMAGE_ARGS) $<


