bits 32

global start32

extern start64
extern kernel_paddr
extern kernel_pos
extern kernel_size
extern ata_do_st_read
extern ata_do_st_softrst
extern mem_init_tables	
extern mem_enable_ia32e
	
section .setup
start32:  
	call check_long_mode
	call mem_init_tables
	call load_kernel
	call mem_enable_ia32e
	jmp 0x08:start64

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

;;; load the kernel with a 28-bit ATA PIO on the boot drive (i.e. bus0-master)	
load_kernel:
	mov dx, 03F6h		; First bus
	call ata_do_st_softrst
	jc .load_kernel_err

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
	jc .load_kernel_err
	ret
.load_kernel_err:
	mov dx, LOAD_ERR_MSG
	call print_string_pm
	jmp $

%include "nubbin/kernel/asm/print_pm.asm"

NO_LONG_MODE_MSG db "Err:No Long Mode", 0
IN_PROT_MODE_MSG db "In Protected Mode",0
LOAD_ERR_MSG     db "Error loading kernel",0	
STARTING_LONG_MODE_MSG db "Starting Long Mode", 0
