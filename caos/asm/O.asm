	global _start

	section .text

_start:

	mov rax, 0
	mov rdi, 0
	mov rsi, str
	mov rdx, 1
	syscall

	mov al, [str]
	or al, 7
	mov [str], al

	mov rax, 1
	mov rdi, 1
	mov rsi, str
	mov rdx, 1
	syscall

	mov rax, 60
	xor rdi, rdi
	syscall

	section .data
str:
	db 0


