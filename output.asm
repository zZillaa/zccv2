section .data

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
	mov [rbp - 56], rax

	mov rsp, rbp
	pop rbp

	mov rax, 60
	xor rdi, rdi
	syscall
