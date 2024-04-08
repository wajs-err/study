global main

main:
	mov rax, 1
	mov rdi, 1
	mov rsi, message
	mov rdx, 34
	syscall
	mov rax, 60
	xor rdi, rdi
	syscall
	
message:
	 db "Liberté, égalité, fraternité!", 10
