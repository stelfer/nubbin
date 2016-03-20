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
        
clear_screen:
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
