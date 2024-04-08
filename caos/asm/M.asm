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

 	xorpd xmm0, xmm0
	movsd xmm0, [x]
	andpd xmm0, [mask]

	push rbp
	mov rdi, fmt_1
	mov rax, 1
	call printf
	pop rbp

	xor rax, rax
	ret

	section .data	
fmt_1:	
	db "%lf", 10, 0
fmt_2:
	db "%lf", 0
align 16
mask:
	dq 0x7fffffffffffffff, 0x7fffffffffffffff

	section .bss
x:	resq 1
y:	resq 1
