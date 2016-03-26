extern main
extern bios_mmap

global start64
	
section .setup
bits 64
start64:

	mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

	;; TODO: map main to higher half
	;; link main to higher half
	;; jump to higher half

	push bios_mmap
	
	call main
	jmp $

	
%include "nubbin/kernel/asm/print_lm.asm"
