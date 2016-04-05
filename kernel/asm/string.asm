
global string_hexify
global strlen
global memcmp
global bzero
	
bits 64
section .text


;;; In: RDI -> the buffer, RSI -> the number, RDX -> length of RSI, RCX -> offset
string_hexify:
	push rbx
	mov rbx, rsi
	add rdx, rdx		; Move to the end +1 (assuming first two are '0x')
	dec rcx
	add rdx, rcx		; Add the offset, and 0-base it as an index

.loop:
	mov al, bl
        and al, 0fh
        cmp al, 09h
        jle .is_anum
        sub al, 09h		; It's A-F
        or al, 60h
	jmp .converted
.is_anum:   			; It's 0-9
        or al, 30h
.converted:
	mov [rdi + rdx], al
	shr rbx, 4
	dec edx
	cmp edx, ecx
	jg .loop
.done:
	pop rbx
	ret


;;; In: rdi -> buffer to scan
;;; Out: RAX -> length of string
strlen:
	push rcx
	push rdi
	xor ecx, ecx
	xor eax, eax
	not ecx
	cld
	repne scasb
	not ecx
	dec ecx
	mov eax, ecx
	pop rdi
	pop rcx
	ret

;; In: rdi -> s1, rsi-> s2, rdx -> size
memcmp:
	push rdi
	push rsi
	xor rax,rax
	mov ecx, edx
	cld
	cmp ecx, ecx
	repe cmpsb
	jz .match
	mov al, byte [rdi]
	sub al, byte [rsi]
.match:	
	pop rsi
	pop rdi
	ret

;;; rdi -> dst, esi -> len
bzero:
	xor rax, rax
	mov ecx, esi
	rep stosb
	ret
	
