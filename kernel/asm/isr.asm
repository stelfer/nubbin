
global isr_default
global isr_timer
	
extern serial_puts
	
bits 64
section .text


isr_default:
	;; push rdi
	;; mov rdi, INTR_MSG
	;; mov rax, serial_puts
	;; call rax
	;; pop rdi
	jmp $
	iretq


isr_timer:
	;; push rdi
	;; mov rdi, TIMER_MSG
	;; mov rax, serial_puts
	;; call rax
	;; pop rdi
	jmp $
	iretq
	
	
INTR_MSG db "DEFAULT HANDLER", 0

TIMER_MSG db "TIMER", 0
	
