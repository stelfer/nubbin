;Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

global mem_init_tables
global mem_enable_ia32e	
global gdt32
global gdt32.code
global gdt32.data
global gdt32.descriptor

extern page_table_paddr

CR0_WP 		equ (1 << 16)	;
CR0_PG 		equ (1 << 31)	;
CR4_PSE 	equ (1 << 4)	;
CR4_PAE 	equ (1 << 5)	;
CR4_PGE 	equ (1 << 7)	;
CR4_PCIDE	equ (1 << 17)	;
CR4_SMEP 	equ (1 << 20)	;
CR4_SMAP 	equ (1 << 21)	;
CR4_PKE 	equ (1 << 22)	;
MSR_LME 	equ (1 << 8)	;
MSR_NXE 	equ (1 << 11)	;
EF_AC 		equ (1 << 18)	;

GDT_NULL	equ 0	
GDT_RDWR 	equ (1 << 41)
GDT_EXEC 	equ (1 << 43)
GDT_TYPE 	equ (1 << 44)
GDT_PRESENT 	equ (1 << 47)
GDT_64 		equ (1 << 53)
GDT_32 		equ (1 << 54)	
GDT_GRAN	equ (1 << 55)	
GDT_LIMIT	equ ((0xf <<48) | 0xffff)

GDT_BASE_LOW	equ 0
GDT_LIMIT_LOW	equ GDT_BASE_LOW + 2
GDT_BASE_MID	equ GDT_LIMIT_LOW + 2
GDT_ACCESS	equ GDT_BASE_MID + 1
GDT_LF		equ GDT_ACCESS + 1
GDT_BASE_HIGH	equ GDT_LF + 1

GDT_CODE	equ 8
GDT_DATA	equ 16	
GDT_DESCR	equ 24
	
;;; Initial Page table setup, we identity map the first GB:
;;; V:0x0000000000000000 -> P:0x0000000000000000
;;; then also for the kernel space, the high range
;;; V:0xffffffff00000000 -> P:0x0000000000000000
;;; The offsets below are to achive the 27 bit (PML4,PDP,PD) lookups
PML4 		equ page_table_paddr
PML4_HOFF 	equ PML4 + 511 * 8
PDPL 		equ PML4 + 0x1000
PDPH 		equ PDPL + 0x1000
PDPH_HOFF 	equ PDPH + 508 * 8
PD   		equ PDPH + 0x1000	

bits 32
section .setup
mem_init_tables:	
	;; Zero the tables
	mov ax, 0
	mov ecx, 0x400
	lea edi, [PML4]
	rep stosd
	lea edi, [PDPL]
	rep stosd
	lea edi, [PDPH]
	rep stosd
	lea edi, [PD]
	rep stosd

	lea eax, [PD]
	or eax, 0b11
	mov [PDPL], eax
	mov [PDPH_HOFF], eax	; 
	
	lea eax, [PDPL]
	or eax, 0b11
	mov [PML4], eax

	lea eax, [PDPH]
	or eax, 0b11
	;; Also map Canonical (negative) higher half to same 1G segment
	mov [PML4_HOFF], eax

	
	mov ecx, 0
.loop:
	;; map ecx-th PDP entry to a huge page that starts at address 2MiB*ecx
	mov eax, 0x200000  ; 2MiB
	mul ecx            ; start address of ecx-th page
	or eax, 0b10000011 ; present + writable + huge
	mov [PD + ecx*8], eax ; map ecx-th entry

	inc ecx
	cmp ecx, 512
	jne .loop

	ret

mem_enable_ia32e:

	;; load PML4 to cr3 register (cpu uses this to access the P4 table)
	mov eax, PML4
	mov cr3, eax

	;; set the long mode bit in the EFER MSR (model specific register)
	mov ecx, 0xC0000080
	rdmsr
	or eax, MSR_LME
	wrmsr
	
	;; enable PAE-flag in cr4 (Physical Address Extension)
	mov eax, cr4
	or eax, CR4_PAE 
	mov cr4, eax
	
	;; enable paging in the cr0 register
	mov eax, cr0
	or eax, CR0_PG
	mov cr0, eax

	lgdt [gdt64.descriptor]
	mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
	ret
	

align 8
gdt32:
	dq GDT_NULL
.code: equ $ - gdt32
	dw 0xffff                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 10011010b                 ; Access (exec/read).
	db 11001111b                 ; Flags + LIMIT
	db 0 
.data: equ $ - gdt32     
	dw 0xffff                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 10010010b                 ; Access (exec/read).
	db 11001111b                 ; Flags + LIMIT
	db 0 
.descriptor:        
        dw gdt32.descriptor - gdt32 - 1
        dd gdt32

align 8
gdt64:
	dq GDT_NULL
.code: equ $ - gdt64
	dw 0                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 10011010b                 ; Access (exec/read).
	db 00100000b                 ; Flags + LIMIT
	db 0 
.data: equ $ - gdt64     
	dw 0                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 10010010b                 ; Access (read/write).
	db 00000000b                 ; Granularity.
	db 0      
.descriptor:        
        dw gdt64.descriptor - gdt64 - 1
        dd gdt64
	
