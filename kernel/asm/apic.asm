global apic_get_base_msr
global apic_reg_read32
global apic_spurious_isr
global apic_enable
	
bits 64
section .text	
apic_get_base_msr:	
	xor rax,rax
	xor rdx,rdx
	mov ecx, 0x1B
	rdmsr
	shr rdx, 32
	or rax, rdx
	ret

apic_reg_read32:
	xor rax,rax
	mov eax, dword[rdi]
	ret

apic_spurious_isr:
	iretq

apic_disable_pic:
	mov al, 0xff
	out 0xa1, al
	out 0x21, al
	ret
	
apic_enable:
	call apic_disable_pic
	mov ecx, 1bh
	rdmsr
	bts eax, 11
	wrmsr
	sti
	ret
	
