;;Copyright (C) 2016 by Soren Telfer - MIT License. See LICENSE.txt

;;; Acquire spinlock on %1, or spin to %2
%macro spinlock_acq 2
	push rax
	push rbx
	mov rbx, 1
	xor rax, rax
	lock cmpxchg dword [%1], ebx
	pop rbx
	pop rax
	pause
	jnz %2
%endmacro

;;; Release the spinlock on %1
%macro spinlock_rel 1
	mov [%1], dword 0
%endmacro	
