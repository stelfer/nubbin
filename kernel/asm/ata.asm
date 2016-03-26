bits 32

global ata_do_st_read
global ata_do_st_softrst	
	
ERR_BIT  equ 00000001b
DRQ_BIT  equ 00001000b
SRV_BIT  equ 00010000b
DF_BIT   equ 00100000b
RDY_BIT  equ 01000000b
BSY_BIT  equ 10000000b
SRST_BIT equ 00000100b
NEIN_BIT equ 00000010b
HOB_BIT  equ 10000000b	
READ_CMD equ 20h

;;; Do an ATA software reset
;;; Input: DX -> status register
;;; Output: CF on error
section .setup
ata_do_st_softrst:
	push eax
	mov al, SRST_BIT
	out dx, al		; software reset
	xor eax, eax
	out dx, al		; return to normal
	in al, dx		; do the 400ns delay
	in al, dx
	in al, dx
	in al, dx

.loop:  ;; Check BSY and RDY
	in al, dx
	and al, BSY_BIT | RDY_BIT
	cmp al, RDY_BIT
	jne .loop

	clc
	pop eax
	ret

;;; Single Tasking Read from the ata
;;; Input: DX -> base bus port, EDI -> buffer, BL -> sectors to read, ECX -> 28bit LBA
;;; Output: CF on error, BL is decrementd
ata_do_st_read:
	pushf
	push edx
	push ecx
	cli

	inc dx			; port 1 -> feature
	mov al, 0
	out dx, al

	inc dx			; port 2 -> sectorcount
	mov al, bl
	out dx, al

	inc dx			; port 3 -> LBA(0-7)
	mov al, cl
	out dx, al

	inc dx			; port 4 -> LBA(8-15)
	shr ecx, 8
	mov al, cl
	out dx, al

	inc dx			; port 5 -> LBA(16-23)
	shr ecx, 8
	mov al, cl
	out dx, al

	inc dx			; port 6 -> LBA(24-27) | 0xe0
	shr ecx, 8
	mov al, cl
	or al, 0xe0
	out dx, al
	
	inc dx			; port 7 -> read cmd
	mov al, 20h
	out dx, al

	mov ecx, 100		; Do a 400ns wait and check
.chk_drq_l:
	in al, dx
	test al, BSY_BIT	; See if BSY is set
	jnz .chk_drq_again
	test al, DRQ_BIT	; See if there is data ready
	jnz .data_ready
.chk_drq_again:
	dec ecx
	jg .chk_drq_l
	
.chk_err_l:
	in al, dx 		; Fell through -- check for err bit
	test al, BSY_BIT
	jnz .chk_err_l
	test al, ERR_BIT | DF_BIT
	jnz .fail
	
.data_ready:
	;; Read one sector at a time
	sub dl, 7		; port 0
	mov ecx, 256
	rep insw
	or dl, 7		; port 7 -> status
	in al, dx
	in al, dx
	in al, dx
	in al, dx

	;; Try another round maybe...
	dec bl
	jz .done
	mov ecx, [esp]
	inc ecx
	jmp .chk_err_l
	
	test al, ERR_BIT | DF_BIT ; Last check on error bit
	jz .fail
	
.fail:
	stc
.done:
	pop ecx
	pop edx
	popf
	ret
	
