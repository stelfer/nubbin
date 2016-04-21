global idt_init
global interrupt_write_gate
	
extern idt_paddr
	
IDT_PRESENT 		equ (1 << 15)
IDT_DPL_RING0 		equ (0 << 13)
IDT_DPL_RING1 		equ (1 << 13)
IDT_DPL_RING2 		equ (2 << 13)
IDT_DPL_RING3 		equ (3 << 13)
IDT_TYPE_CALL_GATE 	equ (0xc << 8)
IDT_TYPE_INTR_GATE 	equ (0xe << 8)
IDT_TYPE_TRAP_GATE 	equ (0xf << 8)

%macro install_gate 1
	extern isr_int%1
	mov rsi, isr_int%1
	xor rdx, rdx
	mov dx, IDT_PRESENT | IDT_TYPE_INTR_GATE
	mov rdi, %1
	call interrupt_write_gate
%endmacro

%macro install_trap 1
	extern isr_int%1
	mov rsi, isr_int%1
	xor rdx, rdx
	mov dx, IDT_PRESENT | IDT_TYPE_TRAP_GATE
	mov rdi, %1
	call interrupt_write_gate
%endmacro
	
bits 64
section .setup

;;; In: RDI -> descriptor index, RSI-> obj, DX-> TYPE
interrupt_write_gate:
	push rdi
	shl rdi, 4
	lea rdi, [idt_paddr + rdi]
	mov rax, rsi
	mov word [rdi], ax 		; off 0-15
	mov word [rdi + 2], 0x08	; segment
	mov word [rdi + 4], dx		; type
	shr rax, 16
	mov word [rdi + 6], ax		; off 16-31
	shr rax, 16
	mov dword [rdi + 8], eax 	; off 32-63
	mov dword [rdi + 12], 0		; reserved
	pop rdi
	ret
	
idt_init:
	;; zero the memory
	xor eax, eax
	mov ecx, 0x400
	lea edi, [idt_paddr]
	rep stosd

	;; Install gates for the reserved interrupts
	install_gate 000h
	install_trap 001h
	install_gate 002h
	install_trap 003h
	install_trap 004h
	install_gate 005h
	install_gate 006h
	install_gate 007h
	install_trap 008h
	install_gate 00ah
	install_gate 00bh
	install_gate 00ch
	install_gate 00dh
	install_gate 00eh
	install_gate 010h
	install_gate 011h
	install_trap 012h
	install_gate 013h
	install_gate 014h
	install_gate 01eh

	install_gate 020h
	install_gate 076h
	
	mov rax, idt_paddr
	push rax
	mov ax, word (256*16)-1
	push ax
	lidt [rsp]
	add rsp, 10
	ret
