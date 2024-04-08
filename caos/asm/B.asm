	global main
	extern scanf
	extern printf

main:

	push rbp
	mov rdi, fmt_2
	mov rsi, dword x
	call scanf
	pop rbp

	mov eax, dword [x]
	and eax, 0x0000ffff

	push rbp
	mov rdi, fmt_1
	mov esi, eax
	call printf
	pop rbp

	xor rax, rax
	ret

	section .data	
fmt_1:	
	db "%d", 10, 0
fmt_2:
	db "%d", 0

	section .bss
x:	
	resd 1
