section .data
	.L0: resq 5

section .text
global _start

_start:	mov [arr], (null)

	mov rax, 5
