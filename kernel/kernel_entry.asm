[bits 32]
[extern main]
[global start]

        ;; the linker will put .setup at offset 0x00100000, so this
        ;; is the entrypoint
[section .setup]
start:  

        lgdt [trickgdt]
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ;;  jump to the higher half kernel
        jmp 0x08:higherhalf

trickgdt:
        dw gdt_end - gdt - 1    ; size of the GDT
        dd gdt                  ; linear address of GDT

gdt:
        dd 0, 0                 ; null gate
        db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40 ; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
        db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40 ; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF

gdt_end:        

[section .text]
higherhalf:
        ;; from now the CPU will translate automatically every address
        ;; by adding the base 0x40000000
        mov esp, sys_stack      ; set up a new stack
        mov ebp, esp
        call main
        jmp $

[global gdt_flush]              ; make 'gdt_flush' accessible from C code
[extern gp]             ; tells the assembler to look at C code for 'gp'

        ;;  this function does the same thing of the 'start' one, this time with
        ;;  the real GDT
gdt_flush:
        lgdt [gp]
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp 0x08:flush2

flush2:
        ret
        
[section .bss]
        resb 0x1000
sys_stack:      
