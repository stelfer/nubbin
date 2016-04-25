
global cpu_spin_here
global cpu_move_stack
global cpu_get_zone_addr
global cpu_isr_apic_timer
global cpu_apic_base
;; global cpu_prepare_trampoline


extern asp_start
extern asp_stop	
	
CPU_ZONE_SIZE equ 0x4000	;FIXME: This should be common with the def in cpu.h
	
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

cpu_get_zone_addr:
	;; cpu_trampoline() will push rsp, mov rbp, rsp on entry, so we use that to find the
	;; top of the stack
	mov rax, rsp
	and rax, -CPU_ZONE_SIZE
	ret

cpu_isr_apic_timer:
	;; the region is located at the bottom of the zone, which is a 2*4k aligned region
	pushf
	push rax
	mov rax, rsp
	and rax, -CPU_ZONE_SIZE
	mov rax, [rax]
	mov DWORD [rax + 0xb0], 0
	pop rax
	popf
	ret

;; cpu_prepare_trampoline:
;; 	mov ecx, asp_stop
;; 	sub ecx, asp_start
;; 	mov esi, asp_start
;; 	cld
;; 	rep movsb
;; 	ret
	
