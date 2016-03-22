global lm_start
extern main

extern bios_mmap
	
section .setup
bits 64
lm_start:

	mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax


	;; mov edx, dword [bios_mmap + 4]
	;; call print_hex_lm
	;; jmp $
	
	;; TODO: map main to higher half
	;; link main to higher half
	;; jump to higher half

	push bios_mmap
	
	call main
	jmp $

%include "nubbin/kernel/asm/print_lm.asm"
