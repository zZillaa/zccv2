section .data
	arr dq 1, 2, 3, 4, 5
	other: resq 3

section .text
global _start	mov [arr], (nul	mov [other], (null)
l)
	mov rax, 3


_start:
	mov rax, 5
