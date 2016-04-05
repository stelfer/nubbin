global apic_get_base_msr
global apic_reg_read32
global apic_set_base_msr
	
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

;;; RDI -> the addres
apic_set_base_msr:
	push rdx
	push rcx
	xor rax,rax
	xor rdx,rdx
	mov ecx, 0x1B
	rdmsr

	bt eax, 8
	jnc .not_bsp
	bts rdi, 8
.not_bsp:	
	bt eax, 11
	jnc .done
	bts rdi, 11
.done:	
	mov ecx, 0x1B
	mov eax, edi
	shr rdi, 32
	mov edx, edi
	and edx, 0xff
	wrmsr

	pop rcx
	pop rdx
	ret
