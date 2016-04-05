global serial_init
global serial_putc
global serial_puts
global serial_put
global serial_putf
global serial_write
	
extern string_hexify
extern strlen	

%include "nubbin/kernel/asm/spinlock.asm"
	
PORT equ 3f8h	
bits 64
section .text
serial_init:
	push rdx
	mov dx, PORT

	add dx, 1
	mov al, 0
	out dx, al		;Disable interrupts

	add dx, 2
	mov al, 0x80
	out dx, al		;Enable DLAB

	sub dx, 3
	mov al, 0x03
	out dx, al		;Set Divisor to 3 lo byte

	add dx, 1
	mov al, 0x00
	out dx, al		; high byte

	add dx, 2
	mov al, 0x03
	out dx, al		; 8 bits, no partiy one stop bit

	sub dx, 1
	mov al, 0xc7
	out dx, al 		; Enable fifo, clear them

	add dx, 2
	mov al, 0x0b
	out dx, al

	pop rdx
	ret

;;; In: DI -> char to send
serial_putc:
	mov edx, PORT
	add dx, 5
.not_rdy:
	in al, dx
	and al, 0x20
	jz .not_rdy

	sub dl, 5
	mov ax, di
	mov ah, 0

 	spinlock_acq serial_lock, .not_rdy
	out dx, al
	spinlock_rel serial_lock
	ret

;;; In: edi -> buffer to print, ebx -> string length
serial_write:
	mov edx, PORT
	add dx, 5
.not_rdy:
	in al, dx
	and al, 0x20
	jz .not_rdy

	;; If we can't acquire, someone is using the port, so recheck for busy
	spinlock_acq serial_lock, .not_rdy

	sub dl, 5		; Write to PORT base
	xor ecx, ecx
.loop:
	mov al, [edi + ecx]		
	out dx, al
	add ecx, 1
	cmp ecx, esi
	jne .loop
	
	spinlock_rel serial_lock
	ret

;;; In: edi -> buffer to print
serial_puts:
	call strlen
	mov esi, eax
	call serial_write
	mov di, 0x0a
	call serial_putc
	ret

;;; In: RDI -> the number, RSI -> the address length
serial_put:
	mov rdx, rsi
	mov rsi, rdi
	cmp rdx, 8
	je .use64
	cmp rdx, qword 4
	je .use32
	cmp rdx, 2
	je .use16
	mov rdi, HEX8_OUT
	jmp .continue
.use64: mov rdi, HEX64_OUT
	jmp .continue
.use32: mov rdi, HEX32_OUT
	jmp .continue
.use16: mov rdi, HEX16_OUT
.continue:
	mov rcx, 2
	call serial_putf
	ret


;;; In: RDI -> the buffer, RSI -> the number, RDX -> the length of rsi, RCX -> offset
serial_putf:
	call string_hexify
	call serial_puts
	ret
	
	
align 4
section .setup
serial_lock dd 0

HEX8_OUT  db '0x00', 0
HEX16_OUT db '0x0000', 0
HEX32_OUT db '0x00000000', 0
HEX64_OUT db '0x0000000000000000', 0
	
