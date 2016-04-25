global smbios_find_entry

	
bits 64
section .text

ANCHOR_B0 	equ '_'
ANCHOR_B1 	equ 'S'
ANCHOR_B2 	equ 'M'
ANCHOR_B3_32	equ '_'	
ANCHOR_B3_64 	equ '3'
ANCHOR_B4_64 	equ '_'

;;; In: RDI -> Address pointer
;;; Out: [RDI] set to address and RAX in {0: none, 1: 32-bit, 2: 64-bit}
smbios_find_entry:
	push rdi
	xor rax, rax
	mov rdi, 0xefffe	; 0xf0000 - 2
	mov rsi, 0xfffff
.B0:
	xor rcx, rcx
	cmp rdi, rsi
	je .done
	add rdi, 2
	cmp BYTE [rdi], ANCHOR_B0
	jne .B0
	cmp BYTE [rdi + 1], ANCHOR_B1
	jne .B0
	cmp BYTE [rdi + 2], ANCHOR_B2
	jne .B0
	cmp BYTE [rdi + 3], ANCHOR_B3_32
	je .found32
	cmp BYTE [rdi + 3], ANCHOR_B3_64
	jne .B0
	cmp BYTE [rdi + 4], ANCHOR_B4_64
	jne .B0
.found64:
	mov cx, WORD [rdi + 5]
	cmp ch, 0x18		; The spec length for 64-bit
	jne .B0
	mov rax, 2
	jmp .do_cksum
.found32:
	mov cx, WORD [rdi + 4]
	cmp ch, 0x1f		; The spec length for 32-bit
	jne .B0
	mov rax, 1

.do_cksum:
	xor rdx, rdx
	mov dl, ch		; CH holds the length
	add rdx, rdi

	xor rcx, rcx
.cksum_loop:
	add cl, BYTE [rdx]
	dec rdx
	cmp rdx, rdi
	jge .cksum_loop
	jecxz .done
	jmp .B0
.done:
	mov rdx, rdi
	pop rdi
	mov [rdi], rdx
	ret
