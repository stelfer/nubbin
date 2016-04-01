global cpu_init


extern serial_puts
extern serial_putaddr
extern serial_putaddrf

bits 64
section .setup
cpu_init:
	xor eax, eax
	cpuid
	mov edi, eax
	mov esi, 1
	call serial_putaddr
	
	mov eax, 1
	cpuid

	mov r8, rax
	
	mov edi, eax
	mov esi, 4
	call serial_putaddr

	mov rax, r8
	mov eax, esi
	and esi, 0xf
	mov edi, STEPPING
	mov edx, 1
	mov rcx, 12
	call serial_putaddrf

	mov rax, r8
	shr eax, 4
	mov esi, eax
	and esi, 0xf
	mov edi, MODEL
	mov edx, 1
	mov rcx, 9
	call serial_putaddrf

	mov rax, r8
	shr eax, 8
	mov esi, eax
	and esi, 0xf
	mov edi, FAMILY
	mov edx, 1
	mov rcx, 10
	call serial_putaddrf
	
	mov rdi, MSG
	call serial_puts
	ret
	
MSG db "A Message", 0	

STEPPING db "STEPPING: 0x00",0
MODEL db "MODEL: 0x00",0
FAMILY db "FAMILY: 0x00",0
TYPE db "TYPE: 0x00",0

