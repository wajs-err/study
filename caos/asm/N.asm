	global main
	extern scanf
	extern printf

	section .text

main:

;	push rbp
;	mov rdi, fmt_1
;	mov rsi, x
;	call scanf
;	pop rbp

;	push rbp
;	mov rdi, fmt_2
;	mov rsi, [x]
;	call printf
;	pop rbp


	mov rcx, 8
	mov r15, a_1

input_1:

	mov rbx, rcx

	push rbp
	mov rdi, fmt_1
	mov rsi, r15
	call scanf
	pop rbp

	mov rcx, rbx
	inc r15

	loop input_1


	mov rcx, 8
	mov r15, a_2

input_2:
	
	mov rbx, rcx

	push rbp
	mov rdi, fmt_1
	mov rsi, r15
	call scanf
	pop rbp

	mov rcx, rbx
	inc r15

	loop input_2

	movq mm0, qword [a_1]
    paddsb mm0, qword [a_2]
    movq qword [a_1], mm0
 
    mov rcx, 8
    mov r15, a_1

output:
	
	mov rbx, rcx

	push rbp
	mov rdi, fmt_2
	mov rsi, [r15]
	call printf
	pop rbp

	mov rcx, rbx
	inc r15

	loop output

	xor rax, rax
	ret
	
	section .data
a_1:
	times 8 db 0
a_2:
	times 8 db 0
x:
	db 0
fmt_1:
	db "%hhd", 0
fmt_2:
	db "%hhd ", 0