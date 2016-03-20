bits 32
	;; edx: address to print
print_hex_pm:
        pusha
        mov edi, 5               ; index for PRINT_HEX_OUT, we will decrement
.loop:
        cmp edi, 1
        je .done
        mov ebx, edx 
        call to_ascii_pm           ; convert bx -> ascii
        mov [PRINT_HEX_OUT + edi], al  ; set PRINT_HEX_OUT memory
        shr edx, 4               ; right shift dx 4 bits
        dec edi
        jmp .loop
        
.done:  
        mov ebx, PRINT_HEX_OUT
        call print_string_pm
        popa
        ret

        PRINT_HEX_OUT db '0x0000', 0
        
to_ascii_pm:
        ;; Converts the lowest 4 bits of bl to ascii
        ;; al <- the ascii byte
        mov al, bl
        and eax, 0fh
        cmp eax, 09h
        jle .next
        sub eax, 09h
        or eax, 60h
        ret
.next:   
        or eax, 30h
        ret


