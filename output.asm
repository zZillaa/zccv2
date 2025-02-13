section .data
	arr dq 1, 2, 3, 4, 5
	z dq 0, 0, 0
	s dq 2, 1
	push rbp
	mov rbp, rsp
	sub rsp, 32


	mov rsp, rbp
	pop rbp
	ret

	mov rax, 60
	xor rdi, rdi
	syscall

section .text
global _start

_start:
