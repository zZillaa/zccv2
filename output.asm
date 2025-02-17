section .data

section .text
global function
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 5
	mov [rbp - 4], rax
	mov rbx, 20
	mov [rbp - 8], rbx
	mov r8, [rbp - 4]
	mov r9, [rbp - 8]
	add r8, r9
