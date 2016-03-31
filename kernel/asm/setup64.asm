bits 64
;; default rel

global start64

extern main
extern kernel_stack_paddr
extern kernel_vaddr
extern serial_init
extern serial_puts
	
section .setup
start64:
	
	mov rsp, kernel_stack_paddr
	mov rbp, rsp

	call install_idt
	lidt [idtr64]

	call serial_init

	mov rax, main
	call rax
	jmp $

int_handler:
	push rdi
	mov rdi, INTR_MSG
	call serial_puts
	pop rsi
	jmp $
	iretq

INTR_MSG db "HERE!", 0
	
install_idt:

	mov ecx, 0

.loop:
	mov edx, ecx
	mov eax, int_handler
	mov word [idt64 + ecx], ax
	mov word [idt64 + ecx + 2], 0x08
	mov word [idt64 + ecx + 4], 0x8e00
	shr ax, 16
	mov word [idt64 + ecx + 6], ax

	shr ax, 16
	mov dword [idt64 + ecx + 8], eax
	mov dword [idt64 + ecx + 12], 0

	
	add ecx, 16
	cmp ecx, 50*16
	jne .loop

	
.done:
	ret

	
%include "nubbin/kernel/asm/print_lm.asm"

align 8
	
idt64:
	times 50*2 dq 0
	
idtr64:
	dw (2*50*8)-1
	dd idt64

	
