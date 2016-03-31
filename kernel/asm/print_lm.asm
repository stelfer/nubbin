VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x2f

print_hex_lm:
        mov edi, 17
.loop:
        mov rbx, rdx 
        call to_ascii		      ; convert bx -> ascii
	mov [PRINT_HEX_OUT + edi], al ; set PRINT_HEX_OUT memory
        shr rdx, 4		      ; right shift dx 4 bits
	
	dec edi
        cmp edi, 1
	jg .loop

        mov rbx, PRINT_HEX_OUT
        call print_string_lm
        ret

        
to_ascii:
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

print_string_lm:
        lea edx, [VIDEO_MEMORY]

.loop:
        mov al, [ebx]           ; move first char to al
        mov ah, WHITE_ON_BLACK
        
        cmp al, 0
        je .done

        mov [edx], ax
        add ebx, 1
        add edx, 2
        jmp .loop
.done:
	ret

PRINT_HEX_OUT db '0x0000000000000000', 0
TEST_STRING db "A STRING", 0
	
