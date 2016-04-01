;;;Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt
	
global start64

extern main
extern kernel_stack_paddr
extern kernel_vaddr
extern serial_init
extern serial_puts
extern idt_init
	
bits 64
section .setup
start64:
	
	mov rsp, kernel_stack_paddr
	mov rbp, rsp

	call idt_init
	call serial_init

	mov rax, main
	call rax
	jmp $
