;;; Multiboot 1 Boot sector
;;; 

MBALIGN     equ 1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ 1<<1                   ; provide memory map
FLAGS       equ MBALIGN | MEMINFO      ; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002             ; 'magic number' lets bootloader find the header
MB_MAGIC    equ 0x2BADB002	       ; eax should contain this after multiboot loader does it's thing
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot


align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM	
