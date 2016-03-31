global serial_init
global serial_putc
global serial_puts
global serial_putaddr

extern string_hexify
extern strlen	

%include "nubbin/kernel/asm/spinlock.asm"
	
PORT equ 3f8h	
bits 64
section .setup
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

;;; In: BL -> char to send
serial_putc:
	push rdx
	mov edx, PORT
	add dx, 5
.not_rdy:
	in al, dx
	and al, 0x20
	jz .not_rdy

	sub dl, 5
	mov al, bl

 	spinlock_acq serial_lock, .not_rdy
	out dx, al
	spinlock_rel serial_lock
	
	pop rdx
	ret

;;; In: edi -> buffer to print, ebx -> string length
serial_send_string:
	push rdx
	push rcx
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
	cmp ecx, ebx
	jne .loop
	
	spinlock_rel serial_lock
	pop rcx
	pop rdx
	ret

;;; In: edi -> buffer to print
serial_puts:
	call strlen
	push rbx
	mov ebx, eax
	call serial_send_string
	mov bl, 0x0a
	call serial_putc
	pop rbx
	ret

;;; In: EBX -> the addr, EDX -> the address length
serial_putaddr:	
	push rdi
	mov edi, HEX_OUT
	call string_hexify
	mov ebx, eax
	call serial_puts
	pop rdi
	ret

	
align 4
section .setup
serial_lock dd 0
	
HEX_OUT db '0x0000000000000000', 0
	
