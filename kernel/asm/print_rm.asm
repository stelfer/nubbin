bits 16
;;; Prints [bx] using BIOS 10h interrupt
print_string_rm:   
        pusha
        mov ah, 0x0e
.loop:
        mov al, [bx]
        cmp al, 0
        je .done
        int 10h
        inc bx
        jmp .loop
.done:   
	;; Move to the next line
	mov ah, 0x03
	mov bh, 0
	int 0x10
	mov dl, 0
	inc dh
	mov ah, 0x02
	int 0x10
	mov ah, 0x07
	mov al, 1
	int 0x10
        popa
        ret

print_hex_rm:
        pusha
        mov di, 5               ; index for PRINT_HEX_OUT, we will decrement
.loop:
        cmp di, 1
        je .done
        mov bx, dx 
        call .to_ascii           ; convert bx -> ascii
        mov [PRINT_HEX_OUT + di], al  ; set PRINT_HEX_OUT memory
        shr dx, 4               ; right shift dx 4 bits
        dec di
        jmp .loop
        
.done:  
        mov bx, PRINT_HEX_OUT
        call print_string_rm
        popa
        ret
	
.to_ascii:
        ;; Converts the lowest 4 bits of bl to ascii
        ;; al <- the ascii byte
        mov al, bl
        and ax, 0fh
        cmp ax, 09h
        jle .to_ascii_next
        sub ax, 09h
        or ax, 60h
        ret
.to_ascii_next:   
        or ax, 30h
        ret


PRINT_HEX_OUT db '0x0000', 0
	
