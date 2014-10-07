QEMU			= /usr/local/bin/qemu-system-i386
QEMU_ARGS		= -boot order=a -fda 

TOOLCHAIN		= toolchain
TOOLCHAIN_BIN	= $(TOOLCHAIN)/bin

NASM			= $(TOOLCHAIN_BIN)/nasm
NDIASM			= $(TOOLCHAIN_BIN) -b16 -o7c00h -a -s7c3eh

HOST_CC			= /usr/local/bin/gcc-4.9

CC				= $(TOOLCHAIN_BIN)/i686-elf-gcc
LD				= $(TOOLCHAIN_BIN)/i686-elf-ld
STRIP			= $(TOOLCHAIN_BIN)/i686-elf-strip
MAKEDEPEND 		= $(CC) $(CFLAGS) -MM -MG -MT '$(patsubst %.c,%.o,$<)'

ASM_SOURCES 	= 					\
	kernel/print_string.asm 		\
	kernel/print_hex.asm 			\
	kernel/disk_load.asm 			\
	kernel/print_string_pm.asm		\
	kernel/gdt.asm					\
	kernel/switch_to_pm.asm

NASM 			+=  -Ikernel/

INCLUDES 		= -Iinclude
CFLAGS 			= $(INCLUDES) -m32 -nostdlib -fno-builtin -fno-stack-protector -ffreestanding

KERNEL_SOURCES 	= $(wildcard kernel/*.c drivers/*.c)
KERNEL_OBJS 	= $(KERNEL_SOURCES:.c=.o)

TEST_SOURCES 	= $(wildcard kernel/test/*.c)
TEST_OBJS 		= $(TEST_SOURCES:.c=.o)
TESTS 			= $(addsuffix .test,$(TEST_SOURCES:.c=))

SOURCES 		= $(KERNEL_SOURCES) $(TEST_SOURCES)
DEPFILES 		= $(SOURCES:.c=.d)

ALL				= os-image

all: $(ALL)

.PHONY: run
run: $(ALL)
	$(QEMU) $(QEMU_ARGS) $<

.PHONY: dis
dis: kernel/boot_sect.bin
	$(NDIASM) $<

os-image : $(TESTS)

kernel/boot_sect.bin: kernel/boot_sect.asm $(ASM) 
	$(NASM) $< -f bin -o $@

kernel/kernel.bin: kernel/kernel_entry.o $(KERNEL_OBJS)
	$(LD) -m elf_i386 -o $@ -Tkernel/link.ld $^ --oformat binary 

kernel/kernel_entry.o: kernel/kernel_entry.asm
	$(NASM) -f elf32 -o $@ $<

os-image: kernel/boot_sect.bin kernel/kernel.bin
	cat $(filter %.bin, $^) > $@

kernel/test/%.o : kernel/test/%.c kernel/test/%.d
	$(HOST_CC) $(INCLUDES) -ffreestanding -c $< -o $@

kernel/test/%.test : kernel/test/%.o
	@rm -f $@ $@.failed
	$(HOST_CC) -o $@.failed $<
	$@.failed && mv $@.failed $@

kernel/%.o : kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/%.d : kernel/%.c
	@$(MAKEDEPEND) $< > $@

clean:
	rm -f *.bin **/*~ *.o $(ALL) $(DEPFILES)
	rm -f $(OBJS) kernel/*.bin 
	rm -f $(TEST_OBJS) $(TESTS) kernel/test/*.failed

-include $(DEPFILES)
