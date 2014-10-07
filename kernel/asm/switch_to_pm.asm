[bits 16]
switch_to_pm:
        cli
        lgdt [gdt0_descriptor]
        mov eax, cr0
        or eax, 0x1
        mov cr0, eax
        jmp 0x08:init_pm

[bits 32]
init_pm:

        mov ax, 0x10
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        ;; mov ebp, 0x90000
        ;; mov esp, ebp

        call BEGIN_PM
        
gdt0_start:
        dd 0, 0                 ; null gate
gdt0_code:      
        db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x00
gdt0_data:      
        db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x00 
gdt0_end:  
gdt0_descriptor:        
        dw gdt0_end - gdt0_start - 1
        dd gdt0_start
        


        CODE_SEG equ gdt0_code - gdt0_start
        DATA_SEG equ gdt0_data - gdt0_start

