	global main
	extern scanf
	extern printf

	section .text

main:

	push rbp
	mov rdi, fmt_2
	mov rsi, n
	call scanf
	pop rbp

	mov ecx, [n]
	mov r15, array

input:

	mov ebx, ecx
	
	push rbp
	mov rdi, fmt_2
	mov rsi, r15
	call scanf
	pop rbp
	
	mov ecx, ebx	
	add r15, 4
	loop input
	
	mov ecx, [n]

output:

	mov ebx, ecx
	
	push rbp
	mov rdi, fmt_1
	mov rsi, [array+4*ecx-4]
	call printf
	pop rbp

	mov ecx, ebx	
	loop output

	xor rax, rax
	ret	

	section .data	
fmt_1:
	db "%d ", 0
fmt_2:
	db "%d", 0

	section .bss
n:	resd 1
array:
	resd 10000
