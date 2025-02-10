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
	mov rax, rax
	imul rbx
	mov [rbp - 32], rax

	mov rsp, rbp
	pop rbp
	ret

	mov rax, 60
	xor rdi, rdi
	syscall
