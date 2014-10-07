[bits 16]
print_string:   
        pusha
        mov ah, 0x0e
print_string_loop:
        mov al, [bx]
        cmp al, 0
        je print_string_done
        int 10h
        inc bx
        jmp print_string_loop
print_string_done:   
        popa
        ret
