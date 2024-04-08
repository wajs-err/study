	global main
	global fun
	extern scanf
	extern printf
	section .text

fun:

	comisd xmm0, [one]
	ja L
	ret

L:
	addsd xmm1, xmm0
	subsd xmm1, [one]
	
	sub rsp, 16
	movsd qword [rsp], xmm0

	divsd xmm0, [two]
	call fun
	
	movsd xmm0, qword [rsp]
	divsd xmm0, [three]
	call fun

	add rsp, 16
	ret	

main:

	push rbp
	mov rdi, fmt_2
	mov rsi, x
	call scanf
	pop rbp

;	movsd xmm0, [x]	
;	movsd xmm1, [one]
;	comisd xmm0, [one]
;	jbe R

;	sub rsp, 8
;	mov dword [rsp], 7
;	add rsp, 8

	movsd xmm1, [zero]
	call fun

	movsd xmm0, xmm1

	push rbp
	mov rdi, fmt_1
	mov rax, 1
	call printf
	pop rbp

R:

	xor rax, rax
	ret

	section .rodata
fmt_1:
	db "%.10lf", 10, 0
fmt_2:
	db "%lf", 0
;x:	
;	dq 0
zero:
	dq 0
one:
	dq 0x3ff0000000000000
two:
	dq 0x4000000000000000
three:
	dq 0x4008000000000000
	
	section .data
x:
	dq 0
