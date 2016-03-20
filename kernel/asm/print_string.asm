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
