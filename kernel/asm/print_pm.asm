VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

bits 32

print_string_pm:
        pusha
        lea edx, [VIDEO_MEMORY]

print_string_pm_loop:
        mov al, [ebx]           ; move first char to al
        mov ah, WHITE_ON_BLACK
        
        cmp al, 0
        je print_string_pm_done

        mov [edx], ax
        add ebx, 1
        add edx, 2
        jmp print_string_pm_loop

print_string_pm_done:
	popa
        ret
        
clear_screen_pm:
	pushad
	mov ebx, VIDEO_MEMORY
	mov ch, 0		;clear
	mov edx, 80*25
.loop:
	mov cl, 0x20
	mov [ebx + edx], cx
	dec edx
	cmp edx, -1
	jnz .loop
	popad
	ret

;;; Prints [edx] to video buffer using
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


	
