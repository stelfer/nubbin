[org 0x7c00]

        mov [BOOT_DRIVE], dl
        mov bp, 0x9000
        mov sp, bp
        
        mov bx, MSG_REAL_MODE
        call print_string
        
        call load_kernel

        call switch_to_pm

%include "print_hex.asm"
%include "print_string.asm"
%include "switch_to_pm.asm"
%include "print_string_pm.asm"
%include "disk_load.asm"

[bits 16]
load_kernel:
        pusha

        mov bx, MSG_LOAD_KERNEL
        call print_string

        ;; disk_load loads into es:bx
        mov bx, 0xffff
        mov es, bx
        mov bx, 0x10

        ;; read 15 sectors
        mov dh, 15
        mov dl, [BOOT_DRIVE]
        call disk_load
        popa
        ret

[bits 32]
BEGIN_PM:
        mov ebx, MSG_PROT_MODE
        call print_string_pm

        call 0x00100000
        jmp $


BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE db "Landed in 32-bit Protected Mode", 0
MSG_LOAD_KERNEL db "Loading Kernel into memory", 0




;
; Filler
;
;
times 510-($-$$) db 0
dw 0xaa55

