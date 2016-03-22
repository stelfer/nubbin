disk_load:
	;; [ES:BX] 	= buffer to read to
	;; CL 		= start sector
	;; DH		= sectors to read
	;; DL		= drive
        push dx
        mov ah, 0x02
        mov al, dh
        mov ch, 0h
        ;; mov cl, 0x02            ; offset
        mov dh, 0h

        int 0x13                ; BIOS interrupt
        jc disk_error

        pop dx
        cmp dh, al
        jne wrong_bytes_disk_error
        ret

disk_error:
        mov bx, DISK_ERROR_MSG
        call print_string_rm
        jmp $

wrong_bytes_disk_error:
        mov bx, WRONG_BYTES_ERROR_MSG
        call print_string_rm
        jmp $

%include "nubbin/kernel/asm/print_rm.asm"
        
DISK_ERROR_MSG:
        db "Disk read error!", 0

WRONG_BYTES_ERROR_MSG:
        db "Wrong Bytes!", 0
        
