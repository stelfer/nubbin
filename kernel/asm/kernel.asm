[bits 32]
[extern main]                   ; Export our C entry point
[global _start]

[section .multiboot]
%include "nubbin/kernel/asm/multiboot1.asm"
	
;; the linker will map .setup at offset 0x00100000 + the header length
[section .setup]
_start:  
	;; Check multiboot
	cmp eax, MB_MAGIC
	jz multiboot_ok
	mov al, "D"
	jmp error
multiboot_ok:	

	;; Check for long mode
	mov eax, 0x80000000    ; implicit argument for cpuid
	cpuid                  ; get highest supported argument
	cmp eax, 0x80000001    ; it needs to be at least 0x80000001
	jb .no_long_mode       ; if it's less, the CPU is too old for long mode

				; use extended info to test if long mode is available
	mov eax, 0x80000001    ; argument for extended processor info
	cpuid                  ; returns various feature bits in ecx and edx
	test edx, 1 << 29      ; test if the LM-bit is set in the D-register
	jz .no_long_mode       ; If it's not set, there is no long mode
	jmp long_mode_ok
.no_long_mode:
	;; mov al, "L"
	;; jmp error

long_mode_ok:	
	
        lgdt [gdt1_descriptor]
	
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ;;  jump to the higher half kernel
        jmp 0x08:higherhalf

error:
	mov ebx, 0xb8000

	;; clear the screen
	mov ch, 0		;clear
	mov edx, 80*25
cl_loop:
	mov cl, 0x20
	mov [ebx + edx], cx
	dec edx
	cmp edx, -1
	jnz cl_loop
	
	mov cl, "E"
	mov ch, 3		;cyan
	mov [ebx], cx
	add ebx, 2
	mov ah, 4		;red
	mov [ebx], ax
	hlt
	
;;; The linker will map .text to the higher offset + 0x40000000
[section .text]
higherhalf:
        ;; from now the CPU will translate automatically every address
        ;; by adding the base 0x40000000
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        mov esp, sys_stack      ; set up a new stack
        mov ebp, esp
        call main
        jmp $

;;  This allows us to set the GDT from C
[global gdt_flush]
[extern gp]
gdt_flush:
        lgdt [gp]
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp 0x08:gdt_flush_ret
gdt_flush_ret:
        ret

[section .setup]
gdt1_start:
        ; null
        dd 0, 0
        ; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
        db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40
        ; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF            
        db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40 
gdt1_end:        
gdt1_descriptor:
        dw gdt1_end - gdt1_start - 1
        dd gdt1_start                  
        
[section .bss]
        resb 0x1000
sys_stack:      
