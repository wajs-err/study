	global main
	extern scanf
	extern printf

	section .text

main:

	push rbp
	mov rdi, fmt_2
	mov rsi, qword x
	call scanf
	pop rbp

	push rbp
	mov rdi, fmt_2
	mov rsi, qword y
	call scanf
	pop rbp	

	mov rax, qword [x]
	mov rbx, qword [y]
	add rax, rbx	
	jnc print_no

	mov rdi, yes
	jmp print

print_no:
	mov rdi, no
	jmp print

print:
	push rbp
	call printf
	pop rbp

	xor rax, rax
	ret	

	section .data	
fmt_1:
	db "%llu", 10, 0
fmt_2:
	db "%llu", 0
yes:
	db "YES", 10, 0
no: 
	db "NO", 10, 0

	section .bss
x:	resq 1
y:	resq 1
