bits 16
extern pm_start

NUM_BIOS_MMAP_ENTRIES equ 20
	
section .boot	
rm_start:	
	;; Do two loads here: (1), load the 7 remaining sectors in the .setup section to
	;; NEXT; (2), load another 30 sectors into START. Everything in NEXT will handle
	;; our real-mode work

	;; Load to NEXT
	mov bx, 0x07e0
	mov es, bx
	mov bx, 0
	mov cl, 2
	mov dh, 7
	mov dl, 0
	call disk_load

	;; Load to START
        mov bx, 0xffff
        mov es, bx
        mov bx, 0x10
	mov cl, 9
        mov dh, 15
	mov dl, 0
        call disk_load

	jmp rm_stage2

%include "nubbin/kernel/asm/disk_load.asm"
;;; Fill out to a full sector
times 510-($-$$) db 0
dw 0xaa55

section .setup
rm_stage2:	
	
        ;; Switch to protected mode
        ;; Load the first of three GDT's
	mov sp, tmp_stack
	mov bp, sp

	cli
	call check_a20
	cmp ax, 1
	jne a20_error

	call read_bios_mmap

	lgdt [gdt0_descriptor]  ; Load the GDT

        mov eax, cr0
        or eax, 0x1
        mov cr0, eax            ; Set protected mode
        jmp 0x08:pm_start        ; Long jump to 32 bits

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

read_bios_mmap:
	;; Low mem size
	mov [bios_mmap.low_sz], ax

	;; High mem size
	mov ah, 0x88
	int 0x15
	mov [bios_mmap.high_sz], ax


	mov di, 0
	mov es, di
	mov di, bios_mmap.tbl_start

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
	cmp esi, NUM_BIOS_MMAP_ENTRIES
	je .num_bios_mmap_entries_exceeded
	add di, 20

	jmp .loop
.num_bios_mmap_entries_exceeded:
	mov dx, 0xdead
	call print_hex_rm
	jmp $
.jcerror:
	mov dx, 0xfeed
	call print_hex_rm
	jmp $
.error:
	mov dx, 0xdead
	call print_hex_rm
	jmp $
	
	;; Do something else here?
.done:
	mov [bios_mmap.num_items], esi
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

