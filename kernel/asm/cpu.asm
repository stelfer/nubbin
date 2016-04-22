
global cpu_spin_here
global cpu_move_stack
global cpu_trampoline_get_zone_stack_addr
global cpu_isr_apic_timer
global cpu_apic_base
	
bits 64
section .text

cpu_spin_here:
	jmp $

;;; In: RDI -> new top, RSI -> old top
;;; Note that this is very invasive because it doesn't attempt to move pushed
;;; RBP's, so you may end up jumping back.. This just handles copying the stack
;;; over
cpu_move_stack:	
	;; Copy the stack to the new location
	mov rcx, rsi
	sub rcx, rsp
	inc rcx
	inc rdi
	std
	rep movsb
	inc rdi			; RDI will be the new RSP

	;; Calculate the rbp
	mov rcx, rbp
	sub rcx, rsp

	mov rsp, rdi
	mov rbp, rsp
	sub rbp, rcx
	ret

	
cpu_apic_base:
	mov ecx, 1bh
	rdmsr
	and eax, 0fffff000h
	ret
	
cpu_trampoline_get_zone_stack_addr:
	;; cpu_trampoline() will push rsp, mov rbp, rsp on entry, so we use that to find the
	;; top of the stack
	mov rax, rbp
	add rax, 8
	ret

cpu_isr_apic_timer:
	;; the region is located at the bottom of the zone, which is a 2*4k aligned region
	mov rax, rsp
	and rax, -0x2000
	mov rax, [rax]
	mov DWORD [rax + 0xb0], 0
	ret
