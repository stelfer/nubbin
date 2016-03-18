[org 0x7c00]
[bits 16]
START equ 0x00100000
        
        ;; disk_load loads into es:bx -> 0x00100000
        mov bx, 0xffff
        mov es, bx
        mov bx, 0x10

        mov dh, 15                  ; read 15 sectors 
        call disk_load
        
        ;; Switch to protected mode
        ;; Load the first of three GDT's
	mov sp, tmp_stack
	mov bp, sp

	cli
	call check_a20
	cmp ax, 1
	jne error

	lgdt [gdt0_descriptor]  ; Load the GDT

        mov eax, cr0
        or eax, 0x1
        mov cr0, eax            ; Set protected mode
        jmp 0x08:init_pm        ; Long jump to 32 bits

error:
	mov bx, 0xb8000
	mov cl, "A"
	mov ch, 3		;cyan
	mov [bx], cx
	add bx, 2
	mov ah, 4		;red
	mov [bx], ax
	hlt
	
	
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

tmp_stack_bottom:
	resb 64
tmp_stack:	

[bits 32]
init_pm:
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

;; Fill out to a full sector
times 510-($-$$) db 0
dw 0xaa55

