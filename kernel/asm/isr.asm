
global isr_trap
	
bits 64
section .text

%macro fault 2
global isr_int%1
extern %2	
isr_int%1:
	mov rdi, %1
	mov rsi, rsp
	call %2
	iretq
%endmacro

%macro fault_code 2
global isr_int%1
extern %2	
isr_int%1:
	mov rdi, %1
	mov rsi, rsp
	pop rdx
	call %2
	iretq
%endmacro

%macro abort 2
global isr_int%1
extern %2	
isr_int%1:
	mov rdi, %1
	mov rsi, rsp
	call %2
	hlt
	iretq
%endmacro

%macro abort_code 2
global isr_int%1
extern %2	
isr_int%1:
	mov rdi, %1
	mov rsi, rsp
	pop rdx
	call %2
	hlt
	iretq
%endmacro
	
%macro page_fault 2
global isr_int%1
extern %2	
isr_int%1:
	pop rsi
	mov rdi, rsp
	mov rdx, cr2
	call %2
	iretq
%endmacro

%macro spurious_fault 1
global isr_int%1
isr_int%1:
	iretq
%endmacro
	
;;;
;;; 0-31 Reserved interrupts
fault 		000h, interrupt_fault
fault 		001h, interrupt_trap
fault 		002h, interrupt_fault
fault 		003h, interrupt_trap
fault		004h, interrupt_trap
fault 		005h, interrupt_fault
fault 		006h, interrupt_fault
fault 		007h, interrupt_fault
abort_code 	008h, interrupt_abort_code
fault_code 	00ah, interrupt_fault_code
fault_code 	00bh, interrupt_fault_code
fault_code 	00ch, interrupt_fault_code
fault_code 	00dh, interrupt_fault_code
page_fault	00eh, memory_isr_pf
fault 		010h, interrupt_fault
fault_code 	011h, interrupt_fault_code
abort 		012h, interrupt_abort
fault 		013h, interrupt_fault
fault 		014h, interrupt_fault
fault_code 	01eh, interrupt_fault_code

;;; Generate stub code for 32-255 so that we can catch the interrupt
;;; number if we want to use the line in the future
fault		020h, cpu_isr_apic_timer
fault		021h, interrupt_irq
fault		022h, interrupt_irq
fault		023h, interrupt_irq
fault		024h, interrupt_irq
fault		025h, interrupt_irq
fault		026h, interrupt_irq
spurious_fault	027h
fault		028h, interrupt_irq
fault		029h, interrupt_irq
fault		02ah, interrupt_irq
fault		02bh, interrupt_irq
fault		02ch, interrupt_irq
fault		02dh, interrupt_irq
fault		02eh, interrupt_irq
fault		02fh, interrupt_irq
fault		030h, interrupt_irq
fault		031h, interrupt_irq
fault		032h, interrupt_irq
fault		033h, interrupt_irq
fault		034h, interrupt_irq
fault		035h, interrupt_irq
fault		036h, interrupt_irq
fault		037h, interrupt_irq
fault		038h, interrupt_irq
fault		039h, interrupt_irq
fault		03ah, interrupt_irq
fault		03bh, interrupt_irq
fault		03ch, interrupt_irq
fault		03dh, interrupt_irq
fault		03eh, interrupt_irq
fault		03fh, interrupt_irq
fault		040h, interrupt_irq
fault		041h, interrupt_irq
fault		042h, interrupt_irq
fault		043h, interrupt_irq
fault		044h, interrupt_irq
fault		045h, interrupt_irq
fault		046h, interrupt_irq
fault		047h, interrupt_irq
fault		048h, interrupt_irq
fault		049h, interrupt_irq
fault		04ah, interrupt_irq
fault		04bh, interrupt_irq
fault		04ch, interrupt_irq
fault		04dh, interrupt_irq
fault		04eh, interrupt_irq
fault		04fh, interrupt_irq
fault		050h, interrupt_irq
fault		051h, interrupt_irq
fault		052h, interrupt_irq
fault		053h, interrupt_irq
fault		054h, interrupt_irq
fault		055h, interrupt_irq
fault		056h, interrupt_irq
fault		057h, interrupt_irq
fault		058h, interrupt_irq
fault		059h, interrupt_irq
fault		05ah, interrupt_irq
fault		05bh, interrupt_irq
fault		05ch, interrupt_irq
fault		05dh, interrupt_irq
fault		05eh, interrupt_irq
fault		05fh, interrupt_irq
fault		060h, interrupt_irq
fault		061h, interrupt_irq
fault		062h, interrupt_irq
fault		063h, interrupt_irq
fault		064h, interrupt_irq
fault		065h, interrupt_irq
fault		066h, interrupt_irq
fault		067h, interrupt_irq
fault		068h, interrupt_irq
fault		069h, interrupt_irq
fault		06ah, interrupt_irq
fault		06bh, interrupt_irq
fault		06ch, interrupt_irq
fault		06dh, interrupt_irq
fault		06eh, interrupt_irq
fault		06fh, interrupt_irq
fault		070h, interrupt_irq
fault		071h, interrupt_irq
fault		072h, interrupt_irq
fault		073h, interrupt_irq
fault		074h, interrupt_irq
fault		075h, interrupt_irq
fault		076h, interrupt_irq
fault		077h, interrupt_irq
fault		078h, interrupt_irq
fault		079h, interrupt_irq
fault		07ah, interrupt_irq
fault		07bh, interrupt_irq
fault		07ch, interrupt_irq
fault		07dh, interrupt_irq
fault		07eh, interrupt_irq
fault		07fh, interrupt_irq
fault		080h, interrupt_irq
fault		081h, interrupt_irq
fault		082h, interrupt_irq
fault		083h, interrupt_irq
fault		084h, interrupt_irq
fault		085h, interrupt_irq
fault		086h, interrupt_irq
fault		087h, interrupt_irq
fault		088h, interrupt_irq
fault		089h, interrupt_irq
fault		08ah, interrupt_irq
fault		08bh, interrupt_irq
fault		08ch, interrupt_irq
fault		08dh, interrupt_irq
fault		08eh, interrupt_irq
fault		08fh, interrupt_irq
fault		090h, interrupt_irq
fault		091h, interrupt_irq
fault		092h, interrupt_irq
fault		093h, interrupt_irq
fault		094h, interrupt_irq
fault		095h, interrupt_irq
fault		096h, interrupt_irq
fault		097h, interrupt_irq
fault		098h, interrupt_irq
fault		099h, interrupt_irq
fault		09ah, interrupt_irq
fault		09bh, interrupt_irq
fault		09ch, interrupt_irq
fault		09dh, interrupt_irq
fault		09eh, interrupt_irq
fault		09fh, interrupt_irq
fault		0a0h, interrupt_fault
fault		0a1h, interrupt_fault
fault		0a2h, interrupt_fault
fault		0a3h, interrupt_fault
fault		0a4h, interrupt_fault
fault		0a5h, interrupt_fault
fault		0a6h, interrupt_fault
fault		0a7h, interrupt_fault
fault		0a8h, interrupt_fault
fault		0a9h, interrupt_fault
fault		0aah, interrupt_fault
fault		0abh, interrupt_fault
fault		0ach, interrupt_fault
fault		0adh, interrupt_fault
fault		0aeh, interrupt_fault
fault		0afh, interrupt_fault
fault		0b0h, interrupt_fault
fault		0b1h, interrupt_fault
fault		0b2h, interrupt_fault
fault		0b3h, interrupt_fault
fault		0b4h, interrupt_fault
fault		0b5h, interrupt_fault
fault		0b6h, interrupt_fault
fault		0b7h, interrupt_fault
fault		0b8h, interrupt_fault
fault		0b9h, interrupt_fault
fault		0bah, interrupt_fault
fault		0bbh, interrupt_fault
fault		0bch, interrupt_fault
fault		0bdh, interrupt_fault
fault		0beh, interrupt_fault
fault		0bfh, interrupt_fault
fault		0c0h, interrupt_fault
fault		0c1h, interrupt_fault
fault		0c2h, interrupt_fault
fault		0c3h, interrupt_fault
fault		0c4h, interrupt_fault
fault		0c5h, interrupt_fault
fault		0c6h, interrupt_fault
fault		0c7h, interrupt_fault
fault		0c8h, interrupt_fault
fault		0c9h, interrupt_fault
fault		0cah, interrupt_fault
fault		0cbh, interrupt_fault
fault		0cch, interrupt_fault
fault		0cdh, interrupt_fault
fault		0ceh, interrupt_fault
fault		0cfh, interrupt_fault
fault		0d0h, interrupt_fault
fault		0d1h, interrupt_fault
fault		0d2h, interrupt_fault
fault		0d3h, interrupt_fault
fault		0d4h, interrupt_fault
fault		0d5h, interrupt_fault
fault		0d6h, interrupt_fault
fault		0d7h, interrupt_fault
fault		0d8h, interrupt_fault
fault		0d9h, interrupt_fault
fault		0dah, interrupt_fault
fault		0dbh, interrupt_fault
fault		0dch, interrupt_fault
fault		0ddh, interrupt_fault
fault		0deh, interrupt_fault
fault		0dfh, interrupt_fault
fault		0e0h, interrupt_fault
fault		0e1h, interrupt_fault
fault		0e2h, interrupt_fault
fault		0e3h, interrupt_fault
fault		0e4h, interrupt_fault
fault		0e5h, interrupt_fault
fault		0e6h, interrupt_fault
fault		0e7h, interrupt_fault
fault		0e8h, interrupt_fault
fault		0e9h, interrupt_fault
fault		0eah, interrupt_fault
fault		0ebh, interrupt_fault
fault		0ech, interrupt_fault
fault		0edh, interrupt_fault
fault		0eeh, interrupt_fault
fault		0efh, interrupt_fault
fault		0f0h, interrupt_fault
fault		0f1h, interrupt_fault
fault		0f2h, interrupt_fault
fault		0f3h, interrupt_fault
fault		0f4h, interrupt_fault
fault		0f5h, interrupt_fault
fault		0f6h, interrupt_fault
fault		0f7h, interrupt_fault
fault		0f8h, interrupt_fault
fault		0f9h, interrupt_fault
fault		0fah, interrupt_fault
fault		0fbh, interrupt_fault
fault		0fch, interrupt_fault
fault		0fdh, interrupt_fault
fault		0feh, interrupt_fault
