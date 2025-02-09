section .data

section .text
global _start

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 32

	mov rbx, 5
	mov [rbp - 8], rbx
	mov rbx, 6
	mov [rbp - 16], rbx
	mov rbx, [rbp - 8]
	mov r8, 2
	mov rax, rbx
	imul r8
	mov r9, rax
	mov [rbp - 24], r9
	mov r8, [rbp - 8]
	mov r9, 1
	mov rax, r8
	cqo
	idiv r9
	mov r10, rax
	mov [rbp - 32], r10

	mov rsp, rbp
	pop rbp

	mov rax, 60
	xor rdi, rdi
	syscall
