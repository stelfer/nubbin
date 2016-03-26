bits 32

extern start64
extern kernel_paddr
extern kernel_pos
extern kernel_size
extern ata_do_st_read
extern ata_do_st_softrst
	
global start32
	
section .setup
start32:  
        mov ax, 0x10
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

	call clear_screen_pm

	call check_long_mode

	call setup_page_tables

	call enable_paging

	call load_kernel
	jc .load_kernel_err
	
	lgdt [gdt1_descriptor]
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
	jmp 0x08:start64

.load_kernel_err:
	mov dx, LOAD_ERR_MSG
	call print_string_pm
	jmp $
	
;;; Ensure that we have long mode
check_long_mode:
	pushad
	mov eax, 0x80000000    ; implicit argument for cpuid
	cpuid                  ; get highest supported argument
	cmp eax, 0x80000001    ; it needs to be at least 0x80000001
	jb .no_long_mode       ; if it's less, the CPU is too old for long mode

				; use extended info to test if long mode is available
	mov eax, 0x80000001    ; argument for extended processor info
	cpuid                  ; returns various feature bits in ecx and edx
	test edx, 1 << 29      ; test if the LM-bit is set in the D-register
	jz .no_long_mode       ; If it's not set, there is no long mode
	popad
	ret
.no_long_mode:
	mov ebx, NO_LONG_MODE_MSG
	call print_string_pm
	hlt
	
;;; Set up 2MiB page identity mappings
setup_page_tables:
	lea eax, [p3_table]
	or eax, 0b11
	mov [p4_table], eax

	;; Also map Canonical (negative) higher half to same 1G segment
	mov [p4_table + 256*8], eax

	lea eax, [p2_table]
	or eax, 0b11
	mov [p3_table], eax

	mov ecx, 0
.loop:
	;; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
	mov eax, 0x200000  ; 2MiB
	mul ecx            ; start address of ecx-th page
	or eax, 0b10000011 ; present + writable + huge
	mov [p2_table + ecx * 8], eax ; map ecx-th entry

	inc ecx
	cmp ecx, 512
	jne .loop

	ret

enable_paging:
	;; load P4 to cr3 register (cpu uses this to access the P4 table)
	lea eax, [p4_table]
	mov cr3, eax

	;; enable PAE-flag in cr4 (Physical Address Extension)
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	;; set the long mode bit in the EFER MSR (model specific register)
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	;; enable paging in the cr0 register
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	ret


;;; Load the kernel with a 28-bit ATA PIO on the boot drive (i.e. bus0-master)	
load_kernel:
	mov dx, 03F6h		; First bus
	call ata_do_st_softrst
	jc .done

	;; Figure out the kernel size in sectors
	mov eax, kernel_size
	mov edx, 0
	mov ecx, 512
	div ecx
	
	test edx, edx
	jz .noextra
	inc eax
.noextra:
	mov bl, al

	;; Calculate the offset from the start of the drive
	mov ecx, kernel_pos
	shr ecx, 9

	mov dx, 01f0h		; First bus
	mov edi, kernel_paddr

	call ata_do_st_read
	jc .done

.done:
	ret
	
%include "nubbin/kernel/asm/print_pm.asm"

NO_LONG_MODE_MSG db "Err:No Long Mode", 0
IN_PROT_MODE_MSG db "In Protected Mode",0
LOAD_ERR_MSG     db "Error loading kernel",0	
gdt1_start:
        dd 0, 0
gdt1_code:	
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10011010b
	db 11101111b
	db 0x00
gdt1_data:	
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10010010b
	db 11101111b
	db 0x00
gdt1_end:        
gdt1_descriptor:
        dw gdt1_end - gdt1_start - 1
        dd gdt1_start                  

section .bss
align 0x1000
p4_table:
	resb 0x1000
p3_table:
	resb 0x1000
p2_table:
	resb 0x1000
stack_top:
	resb 0x1000
sys_stack:      
