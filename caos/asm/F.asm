	global main
	extern scanf
	extern printf

	section .text

main:

	push rbp
	mov rdi, fmt_2
	mov rsi, dword x
	call scanf
	pop rbp

	push rbp
	mov rdi, fmt_2
	mov rsi, dword y
	call scanf
	pop rbp

	mov eax, dword [x]
	mov ebx, dword [y]
	cmp eax, ebx
	jle print
	jmp swap

swap:
	mov eax, ebx

print:
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
x:	resd 1
y:	resd 1
