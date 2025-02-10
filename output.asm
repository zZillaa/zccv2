section .data
	.L0: resq 5
	.L1: resq 10

section .text
global _start

_start:
	push rbp
	mov rbp, rsp
	sub rsp, 32

	mov rax, 5
	mov [rbp - 8], rax
	mov rax, 2
	mov [rbp - 16], rax
	mov rax, 2
	mov rbx, [rbp - 16]
	add rax, rbx
	mov [rbp - 24], rax
	mov rax, 5
	mov rbx, [rbp - 8]
	mul rbx
	mov r8, rax
	mov [rbp - 32], r8
	mov rax, 3
	mov rbx, [rbp - 32]
	mul rbx
	mov r8, rax
	mov [rbp - 40], r8
	mov rax, 4
	mov rbx, [rbp - 40]
	mul rbx
	mov r8, rax
	mov [rbp - 48], r8
	mov rax, 5
	mov rbx, [rbp - 48]
	sub rax, rbx
	mov [rbp - 56], rax	mov [rbp - 56], (null)	mov [rbp - 56], (null)

	mov rsp, rbp
	pop rbp
	ret

	mov rax, 60
	xor rdi, rdi
	syscall

	mov rax, 10

	mov rax, 5
