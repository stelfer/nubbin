global apic_get_base_msr
global apic_reg_read32
global apic_spurious_isr
global apic_enable
global apic_remap_pic

PIC1		equ 0x20	
PIC2		equ 0xA0	
PIC1_COMMAND	equ PIC1
PIC1_DATA	equ (PIC1+1)
PIC2_COMMAND	equ PIC2
PIC2_DATA	equ (PIC2+1)

ICW1_ICW4	equ 0x01
ICW1_SINGLE	equ 0x02
ICW1_INTERVAL4	equ 0x04
ICW1_LEVEL	equ 0x08
ICW1_INIT	equ 0x10
 
ICW4_8086	equ 0x01
ICW4_AUTO	equ 0x02
ICW4_BUF_SLAVE	equ 0x08
ICW4_BUF_MASTER	equ 0x0C
ICW4_SFNM	equ 0x10
 
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
	
pic_iowait:
	nop
	ret

section .setup	
bits 32	
apic_remap_pic:
	in al, PIC1_DATA
	push ax
	in al, PIC2_DATA
	push ax

	mov dx, PIC1_COMMAND
	mov al, ICW1_INIT + ICW1_ICW4
	out dx, al 
	nop

	mov dx, PIC2_COMMAND
	mov al, ICW1_INIT + ICW1_ICW4
	out dx, al
	nop

	mov dx, PIC1_DATA
	mov al, 0x20
	out dx, al
	nop

	mov dx, PIC2_DATA
	mov al, 0x70
	out dx, al
	nop

	mov dx, PIC1_DATA
	mov al, ICW4_8086
	out dx, al
	nop

	mov dx, PIC2_DATA
	mov al, ICW4_8086
	out dx, al
	nop
	

	pop ax
	mov dx, PIC2_DATA
	out dx, al
	pop ax
	mov dx, PIC1_DATA
	out dx, al
	ret
