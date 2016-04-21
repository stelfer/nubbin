bits 32

global start32

extern start64
extern kernel_paddr
extern kernel_pos
extern kernel_size
extern user_paddr
extern user_pos
extern user_size
extern ata_do_st_read
extern ata_do_st_softrst
extern memory_map_init_early
extern memory_enable_ia32e
	
section .setup
start32:
	call check_long_mode
	call remap_pic
	call memory_map_init_early
	call load_kernel
 	call load_user
	call memory_enable_ia32e
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

PIC1		equ 0x20	
PIC2		equ 0xA0	
PIC1_COMMAND	equ PIC1
PIC1_DATA	equ (PIC1+1)
PIC2_COMMAND	equ PIC2
PIC2_DATA	equ (PIC2+1)

ICW1_ICW4	equ 0x01
ICW1_SINGLE	equ 0x02
ICW1_INTERVAL4	equ 0x04
ICW1_LEVEL	equ 0x08
ICW1_INIT	equ 0x10
 
ICW4_8086	equ 0x01
ICW4_AUTO	equ 0x02
ICW4_BUF_SLAVE	equ 0x08
ICW4_BUF_MASTER	equ 0x0C
ICW4_SFNM	equ 0x10
 
pic_iowait:
	nop
	ret
	
remap_pic:
	in al, PIC1_DATA
	push ax
	in al, PIC2_DATA
	push ax

	mov dx, PIC1_COMMAND
	mov al, ICW1_INIT + ICW1_ICW4
	out dx, al 
	nop

	mov dx, PIC2_COMMAND
	mov al, ICW1_INIT + ICW1_ICW4
	out dx, al
	nop

	mov dx, PIC1_DATA
	mov al, 0x20
	out dx, al
	nop

	mov dx, PIC2_DATA
	mov al, 0x70
	out dx, al
	nop

	mov dx, PIC1_DATA
	mov al, ICW4_8086
	out dx, al
	nop

	mov dx, PIC2_DATA
	mov al, ICW4_8086
	out dx, al
	nop
	

	pop ax
	mov dx, PIC2_DATA
	out dx, al
	pop ax
	mov dx, PIC1_DATA
	out dx, al
	ret
	
;;; In: EDI -> dest buffer, esi -> bytes to read, edx -> pos
;;; Out: CF on error, EAX -> number of blocks written
load:
	push edx

	mov dx, 03F6h		; First bus
	call ata_do_st_softrst
	jnc .no_error
	ret
.no_error:

	;; Figure out the kernel size in sectors
	mov eax, esi
	mov edx, 0
	mov ecx, 512
	div ecx

	;; If kernel_size % 512 > 0, then grab one extra sector
	test edx, edx
	jz .noextra
	inc eax
.noextra:
	mov bl, al

	;; Calculate the offset from the start of the drive
	pop ecx
	shr ecx, 9

	mov dx, 01f0h		; First bus

	call ata_do_st_read

	sub eax, ebx
	ret

load_kernel:
	mov edi, kernel_paddr
	mov esi, kernel_size
	mov edx, kernel_pos
	call load
	jc .error
	test eax, eax
	jz .error
	ret
.error:
	mov dx, KERN_LOAD_ERR_MSG
	call print_string_pm
	jmp $

load_user:
	mov edi, user_paddr
	mov esi, user_size
	mov edx, user_pos
	call load
	jc .error
	test eax, eax
	jz .error
	ret
.error:
	mov dx, USER_LOAD_ERR_MSG
	call print_string_pm
	jmp $

%include "nubbin/kernel/asm/print_pm.asm"

NO_LONG_MODE_MSG db "Err:No Long Mode", 0
IN_PROT_MODE_MSG db "In Protected Mode",0
KERN_LOAD_ERR_MSG     db "Error loading kernel code",0	
USER_LOAD_ERR_MSG     db "Error loading user code",0	
STARTING_LONG_MODE_MSG db "Starting Long Mode", 0
