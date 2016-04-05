
global task_init

global task_create
global task_destroy
global task_yield	
extern serial_puts

bits 64	
section .text
task_init:
	;; mov rdi, HELLO
	;; mov rax, serial_puts
	;; call rax
	ret


HELLO db "Task Init", 0	
