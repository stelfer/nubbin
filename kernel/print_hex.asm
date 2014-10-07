print_hex:
        pusha
        mov di, 5               ; index for PRINT_HEX_OUT, we will decrement
print_hex_loop1:
        cmp di, 1
        je print_hex_done1
        mov bx, dx 
        call to_ascii           ; convert bx -> ascii
        mov [PRINT_HEX_OUT + di], al  ; set PRINT_HEX_OUT memory
        shr dx, 4               ; right shift dx 4 bits
        dec di
        jmp print_hex_loop1
        
print_hex_done1:  
        mov bx, PRINT_HEX_OUT
        call print_string
        popa
        ret

        PRINT_HEX_OUT db '0x0000', 0
        
to_ascii:
        ;; Converts the lowest 4 bits of bl to ascii
        ;; al <- the ascii byte
        mov al, bl
        and ax, 0fh
        cmp ax, 09h
        jle to_ascii_next
        sub ax, 09h
        or ax, 60h
        ret
to_ascii_next:   
        or ax, 30h
        ret


