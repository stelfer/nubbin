QEMU			= /usr/local/bin/qemu-system-i386
QEMU_ARGS		= -boot order=a -fda 

TOOLCHAIN		= toolchain
TOOLCHAIN_BIN	= $(TOOLCHAIN)/bin

NASM			= $(TOOLCHAIN_BIN)/nasm -Ikernel/asm/ -w+error 
NDIASM			= $(TOOLCHAIN_BIN) -b16 -o7c00h -a -s7c3eh

NASM_MAKEDEPEND	= $(NASM) -M -MG -MT '$(patsubst %.asm,%.o,$<)'

HOST_CC			= /usr/local/bin/gcc-4.9

CC				= $(TOOLCHAIN_BIN)/i686-elf-gcc
LD				= $(TOOLCHAIN_BIN)/i686-elf-ld
STRIP			= $(TOOLCHAIN_BIN)/i686-elf-strip
MAKEDEPEND 		= $(CC) $(CFLAGS) -MM -MG -MT '$(patsubst %.c,%.o,$<)'

ASM_SOURCES 	= 					\
	kernel/asm/print_string.asm 		\
	kernel/asm/print_hex.asm 			\
	kernel/asm/disk_load.asm 			\
	kernel/asm/print_string_pm.asm		\
	kernel/asm/gdt.asm					\
	kernel/asm/switch_to_pm.asm

BIN_OBJS 		= kernel/asm/boot.o
BIN_IMAGES 		= kernel/asm/kernel.bin

INCLUDES 		= -Iinclude
WARN_FLAGS		= -Wall -Werror
CFLAGS 			= \
	$(INCLUDES) \
	$(WARN_FLAGS) \
	-m32 \
	-nostdlib \
	-fno-builtin \
	-fno-stack-protector -ffreestanding

HOST_CFLAGS		= $(INCLUDES) $(WARN_FLAGS) -ffreestanding

KERNEL_SOURCES 	= $(wildcard kernel/*.c drivers/*.c)
KERNEL_OBJS 	= $(KERNEL_SOURCES:.c=.o)

TEST_SOURCES 	= $(wildcard kernel/test/*.c)
TEST_OBJS 		= $(TEST_SOURCES:.c=.o)
TESTS 			= $(addsuffix .test,$(TEST_SOURCES:.c=))

SOURCES 		= $(KERNEL_SOURCES) $(TEST_SOURCES)
DEPFILES 		= $(SOURCES:.c=.d) $(ASM_SOURCES:.asm=.d) $(BIN_OBJS:.o=.d) $(BIN_IMAGES:.bin=.d)

ALL				= os-image

all: $(ALL)

.PHONY: run
run: $(ALL)
	$(QEMU) $(QEMU_ARGS) $<

#os-image : $(TESTS)

kernel/asm/boot.o: kernel/asm/boot.asm
	$(NASM) $< -f bin -o $@

kernel/asm/kernel.bin: kernel/asm/kernel.o $(KERNEL_OBJS)
	$(LD) -m elf_i386 -o $@ -Tkernel/link.ld $^ --oformat binary 

kernel/asm/kernel.o: kernel/asm/kernel.asm
	$(NASM) -f elf32 -o $@ $<

os-image: kernel/asm/boot.o kernel/asm/kernel.bin
	cat $(filter %.bin %.o, $^) > $@

kernel/test/%.o : kernel/test/%.c kernel/test/%.d
	$(HOST_CC) $(HOST_CFLAGS) -c $< -o $@

kernel/test/%.test : kernel/test/%.o
	@rm -f $@ $@.failed
	$(HOST_CC) -o $@.failed $^
	$@.failed && mv $@.failed $@

kernel/%.o : kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/%.d : kernel/%.c
	@$(MAKEDEPEND) $< > $@

kernel/asm/%.d : kernel/asm/%.asm
	@$(NASM_MAKEDEPEND) $< > $@

kernel/test/%.host.o : kernel/%.c
	$(HOST_CC) $(HOST_CFLAGS) -c -o $@ $<

clean:
	@rm -f *.bin **/*~ **/*.o $(ALL) $(DEPFILES)
	@rm -f $(OBJS) kernel/*.bin 
	@rm -f $(TEST_OBJS) $(TESTS) kernel/test/*.failed kernel/test/*.o

kernel/test/printf.test : kernel/test/printf.host.o kernel/test/ctype.host.o kernel/test/strtox.host.o
kernel/test/strtox.test : kernel/test/ctype.host.o kernel/test/strtox.host.o


-include $(DEPFILES)
