global idt_init
global interrupt_write_gate
	
extern idt_paddr
extern serial_puts
extern isr_trap

IDT_PRESENT 		equ (1 << 15)
IDT_DPL_RING0 		equ (0 << 13)
IDT_DPL_RING1 		equ (1 << 13)
IDT_DPL_RING2 		equ (2 << 13)
IDT_DPL_RING3 		equ (3 << 13)
IDT_TYPE_CALL_GATE 	equ (0xc << 8)
IDT_TYPE_INTR_GATE 	equ (0xe << 8)
IDT_TYPE_TRAP_GATE 	equ (0xf << 8)
	
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

	mov rsi, isr_trap
	xor rdx, rdx
	mov dx, IDT_PRESENT | IDT_TYPE_INTR_GATE
	xor rdi, rdi
.loop:
	call interrupt_write_gate
	add rdi, 1
	cmp rdi, 256
	jne .loop
.done:
	lidt [idtr64]
	ret



	
align 8
idtr64:
	dw (256*16)-1
	dq idt_paddr

	
