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
	mov [rbp - 4], rax
	mov rax, 20
	mov [rbp - 8], rax
	mov [rbp - 8], rax
	mov rax, [rbp - 4]
	mov rbx, [rbp - 8]
	add rax, rbx
	mov [rbp - 12], rax
