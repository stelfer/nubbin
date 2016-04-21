
global cpu_spin_here
global cpu_move_stack

global cpu_trampoline_get_zone_stack_addr
	
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

	
cpu_get_apic_id:
	mov eax, 0x0000000B
	cpuid

global cpu_enable_apic	
cpu_enable_apic:
	mov ecx, 1bh
	rdmsr
	bts eax, 11
	wrmsr
	sti

	.loop:
	hlt
	jmp .loop
	
	ret

global cpu_has_apic
cpu_has_apic:
	push rbx
	mov eax, 1
	cpuid
	mov eax, edx
	and eax, 0x200
	pop rbx
	ret

global cpu_apic_enabled
cpu_apic_enabled:
	mov ecx, 1bh
	rdmsr
	and eax, 0800h
	ret

global cpu_apic_base
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

