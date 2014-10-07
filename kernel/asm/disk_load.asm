; load DH sections to ES:BX from drive DL
disk_load:
        push dx
        mov ah, 0x02            ; read sector function
        mov al, dh              ; bytes to read
        mov ch, 0h              ; cylinder
        mov cl, 0x02            ; offset
        mov dh, 0h              ; head

        int 0x13                ; BIOS interrupt
        jc disk_error

        pop dx
        cmp dh, al
        jne wrong_bytes_disk_error
        ret

disk_error:
        mov bx, DISK_ERROR_MSG
        call print_string
        jmp $

wrong_bytes_disk_error:
        mov bx, WRONG_BYTES_ERROR_MSG
        call print_string
        jmp $

%include "print_string.asm"
        
DISK_ERROR_MSG:
        db "Disk read error!", 0

WRONG_BYTES_ERROR_MSG:
        db "Wrong Bytes!", 0
        
