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
        ;; lgdt [gdt1_descriptor]

        mov ax, 0x10
        mov ds, ax
        mov ss, ax
        mov es, ax
        mov fs, ax
        mov gs, ax

        mov ebp, 0x90000
        mov esp, ebp

        call BEGIN_PM
        
        ;; jmp 0x08:init2
        
;; init2:  
        
        
;;         jmp 0x08:init2_pm
        
;; init2_pm:
        
        
        ;; call BEGIN_PM


        
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
        
gdt1_start:
        dd 0, 0                 ; null gate
        db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40 ; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
        db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40 ; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
gdt1_end:        
gdt1_descriptor:        
        dw gdt1_end - gdt1_start - 1
        dd gdt1_start



        
;; gdt0_start:
;; gdt0_null:
;;         dd 0x00
;;         dd 0x00

;; gdt0_code:
;;         dw 0xffff               ; Limit (0-15)
;;         dw 0x0                  ; Base (0-15)
;;         db 0x0                  ; Base (16-23)
;;         db 10011010b            ; Type flags
;;         db 11001111b            ; 2nd flags, Limit (16-19)
;;         db 0x00                  ; Base (24-31)

;; gdt0_data:
;;         dw 0xffff               ; Limit (0-15)
;;         dw 0x0                  ; Base (0-15)
;;         db 0x0                  ; Base (16-23)
;;         db 10010010b            ; Type flags
;;         db 11001111b            ; 2nd flags, Limit (16-19)
;;         db 0x00                  ; Base (24-31)
;; gdt0_end:

        
;; gdt0_descriptor:
;;         dw gdt0_end - gdt0_start - 1
;;         dd gdt0_start

        CODE_SEG equ gdt0_code - gdt0_start
        DATA_SEG equ gdt0_data - gdt0_start

