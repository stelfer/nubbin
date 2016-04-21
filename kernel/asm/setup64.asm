;;;Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt
	
global start64

extern bsp_init
extern kernel_stack_paddr
extern serial_init
extern idt_init
extern cpu_init
	
bits 64
section .setup
start64:

	call idt_init
	
	mov rax, bsp_init
	call rax
	jmp $			; Sanity placeholder: this will be overwritten during init
