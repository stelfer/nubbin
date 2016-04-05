;Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

global memory_map_init_early
global memory_enable_ia32e
global memory_flush_tlb	
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
	
;;; See struct memory_page_tables in memory.h
PML4 		equ page_table_paddr
PML4_HOFF 	equ PML4 + 511 * 8
USER_PDP 	equ PML4 + 0x1000
KERN_PDP 	equ USER_PDP + 0x1000
KERN_PDP_HOFF 	equ KERN_PDP + 508 * 8
MMAP_PDP	equ KERN_PDP + 0x1000	
USER_PDS   	equ MMAP_PDP + 0x1000 

bits 32
section .setup
memory_map_init_early:	
	;; Zero the tables
	mov ax, 0
	mov ecx, 0x400
	lea edi, [PML4]
	rep stosd
	lea edi, [USER_PDP]
	rep stosd
	lea edi, [KERN_PDP]
	rep stosd
	lea edi, [USER_PDS]
	rep stosd

	lea eax, [USER_PDS]
	or eax, 0b11
	mov [USER_PDP], eax
	mov [KERN_PDP_HOFF], eax	; 
	
	lea eax, [USER_PDP]
	or eax, 0b11
	mov [PML4], eax

	lea eax, [KERN_PDP]
	or eax, 0b11
	;; Also map Canonical (negative) higher half to same 1G segment
	mov [PML4_HOFF], eax

	
	mov ecx, 0
.loop:
	;; map ecx-th PDP entry to a huge page that starts at address 2MiB*ecx
	mov eax, 0x200000  ; 2MiB
	mul ecx            ; start address of ecx-th page
	or eax, 0b10000011 ; present + writable + huge
	mov [USER_PDS + ecx*8], eax ; map ecx-th entry

	inc ecx
	cmp ecx, 512
	jne .loop

	ret

memory_enable_ia32e:

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
	
memory_flush_tlb:
	mov eax, cr3
	mov cr3, eax
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
	
