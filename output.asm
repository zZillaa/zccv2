section .data

section .text
global function
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov qword [rbp - 4 + 0], 0
	mov qword [rbp - 4 + 4], 1
	mov qword [rbp - 4 + 8], 2
	mov qword [rbp - 4 + 12], 0
	mov rax, 0
	mov [rbp - 4], rax
.L1: 
	mov rax, [rbp - 4]
	mov rbx, 4
	cmp rax, rbx
	setl r8
	movzx r8, r8
	cmp r8, 0
	je .L2
	mov rax, 6
	mov [(null)], rax
	mov rax, [rbp - 4]
	mov [rbp - 4], rax
	jmp .L1
.L2:
	mov rax, 0

	leave
	ret
