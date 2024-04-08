	global main
	extern scanf
	extern printf

	section .text

main:

	push rbp
	mov rdi, fmt_2
	mov rsi, x
	call scanf
	pop rbp

	push rbp
	mov rdi, fmt_2
	mov rsi, y
	call scanf
	pop rbp	

	mov rax, [x]
	mov rbx, [y]
	
	mov rcx, rax
	mul rbx
	mov rdx, rax
	mov rax, rcx
	mov rcx, rdx

	cmp rax, rbx
	jae gcd
	
	mov rdx, rax
	mov rax, rbx
	mov rbx, rdx

gcd:

	cmp rbx, 0
	je lcm

	xor rdx, rdx
	div rbx
	
	mov rax, rbx
	mov rbx, rdx
	jmp gcd

lcm:

	mov rbx, rax
	mov rax, rcx
	xor rdx, rdx
	div rbx

	push rbp
	mov rdi, fmt_1
	mov rsi, rax
	call printf
	pop rbp

	xor rax, rax
	ret	

	section .data	
fmt_1:
	db "%llu", 10, 0
fmt_2:
	db "%llu", 0

	section .bss
x:	resq 1
y:	resq 1
