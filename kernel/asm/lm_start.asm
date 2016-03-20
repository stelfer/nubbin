global lm_start
extern main
	
section .text
bits 64
lm_start:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
	call main
	jmp $

	
section .rodata
global video
video dd 0x000b8000
