section .data

section .text
global _start

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 32

	mov rbx, 5
	mov [rbp - 8], rbx
	mov rbx, 1
	mov [rbp - 16], rbx
	mov rbx, [rbp - 8]
	mov r8, [rbp - 8]
	add rbx, r8
	mov [rbp - 24], rbx

	mov rsp, rbp
	pop rbp
	ret

	mov rax, 60
	xor rdi, rdi
	syscall
