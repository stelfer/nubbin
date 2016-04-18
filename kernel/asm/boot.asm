bits 16

extern start16	
extern setup_pos		; Defined by the linker, see linker script
extern kernel_pos
	
section .boot	
;;; The boot sector, we use BIOS 13h to load .setup
	jmp 0x00:start		; Sanity (Ensure CS=0)
start:				; |
	mov ax, 0		; |
	mov ss, ax		; |
	mov ds, ax		; |
	mov es, ax		; |
	mov fs, ax		; |
	mov gs, ax		; |
	mov sp, 0x7c00		; |
	mov bp, sp		; x
	
	mov bx, setup_pos	; boot loaded pos + 512
	shr bx, 4		; move it into es, so >> 4
	mov es, bx		; dst segment
	mov bx, 0		; dst offset

	mov ch, 0h		; cylinder
	mov cl, 2		; sector
        mov dh, 0h		; head

	mov ax, kernel_pos	; num_sectors = kernel_pos/512 - 1
	shr ax, 9
	dec ax
	push ax			; save to compare after read

        mov ah, 0x02		; service
        int 0x13                ; BIOS interrupt
        jc .disk_error

	pop dx			; Make sure we read enough
        cmp al, dl
        jne .wrong_bytes

	jmp start16		; jump to loaded sector
	
.disk_error:
        mov bx, DISK_ERROR_MSG
        call print_string_rm
        jmp $

.wrong_bytes:
        mov bx, WRONG_BYTES_ERROR_MSG
        call print_string_rm
        jmp $

%include "nubbin/kernel/asm/print_rm.asm"
        
DISK_ERROR_MSG:
        db "Disk read error!", 0

WRONG_BYTES_ERROR_MSG:
        db "Wrong Bytes!", 0
        
times 510-($-$$) db 0 		; fill out to a full sector
dw 0xaa55			; boot magic
