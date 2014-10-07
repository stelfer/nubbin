[bits 32]
[extern main]                   ; Export our C entry point
[global start]

;; the linker will map .setup at offset 0x00100000
[section .setup]
start:  
        lgdt [gdt1_descriptor]
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ;;  jump to the higher half kernel
        jmp 0x08:higherhalf

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
