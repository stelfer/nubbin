
global isr_trap
global isr
global isr_abort
global isr_fault_code
global isr_fault	

global isr_pf

	
extern interrupt_trap
extern interrupt
extern interrupt_abort
extern interrupt_fault_code
extern interrupt_fault	

extern memory_isr_pf
	
bits 64
section .text

isr_trap:
	mov rdi, rsp
	call interrupt_trap
	jmp $
	iretq

isr_interrupt:
	mov rdi, rsp
	call interrupt
	iretq

isr_abort:
	mov rdi, rsp
	call interrupt_abort
	iretq

isr_fault_code:
	pop rsi
	mov rdi, rsp
	call interrupt_fault_code
	iretq

isr_fault:
	mov rdi, rsp
	call interrupt_abort
	iretq

isr_pf:
	pop rsi
	mov rdi, rsp
	mov rdx, cr2
	call memory_isr_pf
	iretq
	
