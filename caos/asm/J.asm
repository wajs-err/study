	global main
	global merge_sort
	extern scanf
	extern printf	

	section .text

; this code is terrible
; rdi -- from, rsi -- count (rdi and rsi is probably unchanged)
merge_sort:
	cmp rsi, 1
	jne sort_left
	ret	

sort_left:
	
	push rdi
	push rsi
	
	xor rdx, rdx
	mov rax, rsi
	div qword [two]
	mov rsi, rax
	push rsi
	call merge_sort

sort_right:
	mov r8, [rsp+8]
	add rdi, [rsp]
	sub r8, rsi
	mov rsi, r8
	call merge_sort
	
	pop r8 		; mid
	pop rsi		; count
	pop r12		; from
	mov rdi, r12

	mov rax, r12
	mov r11, 4
	mul r11
	mov r12, rax
	mov r10, r12

	xor rax, rax
	mov rbx, r8
	mov rdx, 0

merge:				; [from, from+mid][from+mid+1, from+count]

	cmp rax, r8
	je push_right

	cmp rbx, rsi
	je push_left

	mov ecx, [array+4*rax+r12]
	cmp ecx, [array+4*rbx+r10]
	jg L
	mov ecx, [array+4*rax+r12]
	mov [tmp_array+4*rdx+r12], ecx
	inc rax
	inc rdx
	jmp merge

L: 
	mov ecx, [array+4*rbx+r10]
	mov [tmp_array+4*rdx+r12], ecx
	inc rbx
	inc rdx
	jmp merge

push_left:
	
	mov ecx, [array+4*rax+r12]
	mov [tmp_array+4*rdx+r12], ecx
	inc rax
	inc rdx

	cmp rax, r8
	jne push_left

	xor rax, rax
	jmp copy_segment

push_right:

	mov ecx, [array+4*rbx+r10]
	mov [tmp_array+4*rdx+r12], ecx
	inc rbx
	inc rdx

	cmp rbx, rsi
	jne push_right

	xor rax, rax
	jmp copy_segment

copy_segment:

	mov ecx, [tmp_array+4*rax+r12]
	mov [array+4*rax+r12], ecx
	inc rax

	cmp rax, rsi
	jne copy_segment

	ret


main:
	
	push rbp
    mov rdi, fmt_1
    mov rsi, n
    call scanf
    pop rbp

    mov ecx, [n]
    mov r15, array

input:

    mov ebx, ecx

    push rbp
    mov rdi, fmt_1
    mov rsi, r15
    call scanf
    pop rbp

    mov ecx, ebx
    add r15, 4
    loop input


	mov rdi, 0
	mov esi, dword [n]
	call merge_sort

    mov ecx, [n]
	mov r15, array


output:

    mov ebx, ecx

    push rbp
    mov rdi, fmt_2
    mov rsi, [r15]
    call printf
    pop rbp

    mov ecx, ebx
	add r15, 4
    loop output	

	xor rax, rax
	ret

	section .rodata
fmt_1:
	db "%d", 0
fmt_2:
	db "%d ", 0
two:
	dq 2

	section .bss
n: 	
	resd 1
array:
	resd 10000
tmp_array:
	resd 10000
