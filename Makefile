#Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt


TARGET_CCWARN 		+= -Wno-unused-function -Wno-unused-variable -Wno-macro-redefined
TARGET_CCFLAGS		+= -ffreestanding

QEMU			:= /usr/local/bin/qemu-system-x86_64
QEMU_CPU		:= -cpu kvm64,+ssse3,+sse4.1,+sse4.2,-x2apic,-vme
QEMU_NUMA		:= -numa node,nodeid=0,cpus=0-3,mem=512M -numa node,nodeid=1,cpus=4-7,mem=512M
QEMU_SMP		:= -smp sockets=2,cores=2,threads=2
QEMU_MACHINE		:= $(QEMU_CPU) $(QEMU_NUMA) $(QEMU_SMP)
QEMU_NOGRAPHIC		:= -nographic
# QEMU_DEBUG		:= -s -S
QEMU_SERIAL		:= -serial mon:stdio --no-reboot -d int #int,exec,cpu,mmu,pcall,guest_errors
QEMU_IMAGE_ARGS		:= $(QEMU_MACHINE) $(QEMU_DEBUG) $(QEMU_SERIAL) $(QEMU_NOGRAPHIC) -m 1G
#-boot a -fda
QEMU_KERNEL_ARGS	:= -kernel

KERNEL_OBJS 		:= 					\
			build/nubbin/kernel/bsp_init.ko		\
			build/nubbin/kernel/io.ko 		\
			build/nubbin/kernel/asm/boot.o		\
			build/nubbin/kernel/asm/setup16.o	\
			build/nubbin/kernel/asm/setup32.o	\
			build/nubbin/kernel/asm/ata.o		\
			build/nubbin/kernel/asm/serial.ko	\
			build/nubbin/kernel/asm/string.ko	\
			build/nubbin/kernel/asm/memory.o	\
			build/nubbin/kernel/asm/interrupt.o	\
			build/nubbin/kernel/asm/setup64.o	\
			build/nubbin/kernel/asm/task.ko		\
			build/nubbin/kernel/asm/cpu.ko		\
			build/nubbin/kernel/asm/isr.ko		\
			build/nubbin/kernel/asm/apic.ko		\
			build/nubbin/kernel/console.ko		\
			build/nubbin/kernel/string.ko		\
			build/nubbin/kernel/cpu.ko		\
			build/nubbin/kernel/memory.ko		\
			build/nubbin/kernel/acpi.ko		\
			build/nubbin/kernel/apic.ko		\
			build/nubbin/kernel/kdata.ko		\
			build/nubbin/kernel/interrupt.ko	\
			build/nubbin/kernel/user.o

OS_IMAGE		:= build/nubbin/os-image

nubbin-clean:
	rm -f $(OS_IMAGE)
	rm -f $(KERNEL_OBJS)
	rm -f $(patsubst build/%.o,build/deps/%.d,$(KERNEL_OBJS))
	rm -f $(patsubst build/%.bin,build/deps/%.d,$(PM_START_BIN) $(RM_START)) 

KERNEL_VERSION		!= cd $(PROJECT) && git rev-parse --short HEAD
TARGET_CCFLAGS		+= -mcmodel=large -DVERSION=\"$(KERNEL_VERSION)\"

TESTS		:= 

$(BUILD)/%.ko : %.c | $(DEPDIR)/%.d
	$(PRECOMPILE_DEP)
	$(call PRECOMPILE_CMD,$(TARGET_COMPILE_CC),$<)
	$(TARGET_COMPILE_CC) $(DEPFLAGS) -o $@ -c $<
	$(POSTCOMPILE_CMD)
	$(POSTCOMPILE_DEP)

$(BUILD)/%.ko : %.asm | $(DEPDIR)/%.d
	$(call NASM_BUILD,$(TARGET_FORMAT))

$(OS_IMAGE): $(KERNEL_OBJS)
	$(TARGET_LD) -m $(TARGET_LDEMU) -Tnubbin/kernel/asm/os-image.$(TARGET_LDEMU).ld $^ $(KERNEL_LD_OPTS) -o $@.elf64
	$(TARGET_OBJCOPY) -S -O binary $@.elf64 $@

.PHONY: run-image
run-image: $(OS_IMAGE)
	$(QEMU) $(QEMU_IMAGE_ARGS) $<


