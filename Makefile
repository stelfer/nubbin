#Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt


TARGET_CCWARN 		+= -Wno-unused-function -Wno-unused-variable -Wno-macro-redefined
TARGET_CCFLAGS		+= -nostdlib -fno-builtin -fno-stack-protector -ffreestanding

QEMU			:= /usr/local/bin/qemu-system-x86_64
QEMU_IMAGE_ARGS		:= -m 1G  -nographic -boot order=a -fda
QEMU_KERNEL_ARGS	:= -kernel

KERNEL_OBJS 		:= 					\
			build/nubbin/kernel/kernel.o		\
			build/nubbin/kernel/io.o 		\
			build/nubbin/kernel/asm/lm_start.o	\
			build/nubbin/kernel/console.o		\
			build/nubbin/kernel/serial.o		\
			build/nubbin/kernel/string.o

OS_IMAGE		:= build/nubbin/os-image

PM_START		:= build/nubbin/kernel/asm/pm_start.$(TARGET_FORMAT)
PM_START_BIN		:= build/nubbin/kernel/asm/pm_start.bin
RM_START		:= build/nubbin/kernel/asm/rm_start.bin

nubbin-clean:
	rm -f $(OS_IMAGE) $(PM_START) $(PM_START).o $(RM_START) $(PM_START_BIN)
	rm -f $(KERNEL_OBJS)
	rm -f $(patsubst build/%.o,build/deps/%.d,$(KERNEL_OBJS))
	rm -f $(patsubst build/%.bin,build/deps/%.d,$(PM_START_BIN) $(RM_START)) 

TARGET_CCFLAGS		+= -mcmodel=large

TESTS		:= 

$(PM_START): $(KERNEL_OBJS)

$(PM_START_BIN) : $(PM_START)
	$(TARGET_OBJCOPY) -O binary $< $@

$(OS_IMAGE) : $(RM_START) $(PM_START_BIN)
	cat $^ > $@


.PHONY: run-image
run-image: $(OS_IMAGE)
	$(QEMU) $(QEMU_IMAGE_ARGS) $<


