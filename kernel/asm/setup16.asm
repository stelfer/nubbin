bits 16

extern start32
extern print_hex_rm
extern print_string_rm
	
global start16

NUM_BIOS_MMAP_ENTRIES equ 20
	
section .setup

;;; Handle all real mode activities here
start16:	
	;; We're 
	mov sp, tmp_stack
	mov bp, sp

	cli

	call check_a20
	test ax, ax
	jz .check_a20_err

	call read_bios_mmap
	jc .read_bios_mmap_err
	
	;; Switch to protected mode
	lgdt [gdt0_descriptor]  ; Load the GDT

        mov eax, cr0
        or eax, 0x1
        mov cr0, eax            ; Set protected mode
        jmp 0x08:start32        ; Long jump to 32 bits

.check_a20_err:
	mov bx, A20_ERR_MSG
	call print_string_rm
	jmp $
.read_bios_mmap_err:
	mov bx, READ_BIOS_ERR_MSG
	call print_string_rm
	jmp $
	
;;; From osdev
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
	je .done
 
	mov ax, 1
.done:
	pop si
	pop di
	pop es
	pop ds
	popf
 
	ret

;;; Read the bios memory info while we are in real mode
;;; Input:
;;; Output CF -> error
read_bios_mmap:
	;; Low mem size
	pusha
	int 12h
	mov [bios_mmap.low_sz], ax

	;; High mem size
	mov ah, 88h
	int 15h
	mov [bios_mmap.high_sz], ax

	mov di, 0
	mov es, di
	mov di, bios_mmap.tbl_start

	xor ebx,ebx
	mov esi, 0

.loop:	
	mov eax, 0xe820
	mov ecx, 24
	mov edx, 0x534D4150
	int 0x15
	jc .error		; CF bit signals error

	test ebx, ebx
	jz .done

	inc esi
	cmp esi, NUM_BIOS_MMAP_ENTRIES
	je .error
	add di, 24
	jmp .loop

.error:
	stc
	popa
	ret
.done:
	mov [bios_mmap.num_items], esi
	popa
	ret

;; Initial GDT
tmp_stack_bottom:
	times 64 db 0
tmp_stack:	
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

global bios_mmap
bios_mmap:
	.low_sz dw 0
	.high_sz dw 0
	.num_items dd 0xdeadbeef
	.tbl_start times 20*NUM_BIOS_MMAP_ENTRIES db 0 ;reserve space for 20 tables
	
A20_ERR_MSG   db "A20 Not Enabled", 0
READ_BIOS_ERR_MSG   db "Unable to read BIOS Memory", 0

