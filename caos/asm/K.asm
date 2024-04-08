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

	xorpd xmm0, xmm0
	xorpd xmm1, xmm1
	movsd xmm0, [x]
	movsd xmm1, [y]
	divsd xmm0, xmm1

	movsd [x], xmm0

	push rbp
	mov rdi, fmt_1
	mov rax, 1
	movsd xmm0, [x]
;	mov rsi, [x] 		; ok without 2 prev lines
	call printf
	pop rbp

	xor rax, rax
	ret

	section .data	
fmt_1:	
	db "%lf", 10, 0
fmt_2:
	db "%lf", 0

	section .bss
x:	resq 1
y:	resq 1
