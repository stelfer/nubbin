org 0x7c00
bits 16
START   equ 0x001000e8

rm_start:	
        ;; disk_load loads into es:bx -> 0x00100080
        mov bx, 0xffff
        mov es, bx
        mov bx, 0xf8


        mov dh, 30                  ; read 15 sectors 
        call disk_load
        
        ;; Switch to protected mode
        ;; Load the first of three GDT's
	mov sp, tmp_stack
	mov bp, sp

	cli
	call check_a20
	cmp ax, 1
	jne a20_error

	call map_memory

	lgdt [gdt0_descriptor]  ; Load the GDT

        mov eax, cr0
        or eax, 0x1
        mov cr0, eax            ; Set protected mode
        jmp 0x08:pm_init        ; Long jump to 32 bits

a20_error:
	mov bx, A20_ERR_MSG
	call print_string_rm
	jmp $
	
check_a20:
	pushf
	push ds
	push es
	push di
	push si
 
	cli
 
	xor ax, ax ; ax = 0
	mov es, ax
 
	not ax ; ax = 0xFFFF
	mov ds, ax
 
	mov di, 0x0500
	mov si, 0x0510
 
	mov al, byte [es:di]
	push ax
 
	mov al, byte [ds:si]
	push ax
 
	mov byte [es:di], 0x00
	mov byte [ds:si], 0xFF
 
	cmp byte [es:di], 0xFF
 
	pop ax
	mov byte [ds:si], al
 
	pop ax
	mov byte [es:di], al
 
	mov ax, 0
	je .check_a20__exit
 
	mov ax, 1
 
.check_a20__exit:
	pop si
	pop di
	pop es
	pop ds
	popf
 
	ret

map_memory:
	;; Low mem size
	mov bx, 0xffff
	mov es, bx
	mov bx, 0x10

	int 0x12
	mov [es:bx], ax

	;; High mem size
	mov ah, 0x88
	int 0x15
	add bx, 2
	mov [es:bx], ax

	mov di, 0x10 + 6
	xor ebx,ebx
	mov esi, 0
.loop:	

	mov eax, 0xe820
	mov ecx, 20
	mov edx, 0x534D4150
	int 0x15

	jc .error		; continuation bit signals error
	test ebx, ebx
	je .done
	inc esi
	add di, 20
	jmp .loop
.error:
	;; Do something else here?
.done:
	mov bx, 0xffff
	mov es, bx
	mov bx, 0x14
	mov [es:bx], si
	ret
	
tmp_stack_bottom:
	resb 64
tmp_stack:	

bits 32
pm_init:
        ;; 32 Bits mode here, fix the registers as pointing
        ;; To the data section of the current GDT
        mov ax, 0x10
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        call START              ; Jump to the kernel entry point in kernel.asm
        jmp $                   ; For safety

%include "nubbin/kernel/asm/disk_load.asm"

;; Initial GDT
gdt0_start:
        dd 0, 0
gdt0_code:      
        db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x00
gdt0_data:      
        db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x00 
gdt0_end:  
gdt0_descriptor:        
        dw gdt0_end - gdt0_start - 1
        dd gdt0_start

A20_ERR_MSG   db "A20 Not Enabled", 0
	
;; Fill out to a full sector
times 510-($-$$) db 0
dw 0xaa55

