[bits 32]
[extern main]
[global start]
start:  
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
        
        
