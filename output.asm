section .data

section .text
global add
global function
global main
global _start

_start:


add:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov [rbp - 4], rsi
	mov [rbp - 8], rdi
	mov rax, [rbp - 4]
	mov rbx, [rbp - 8]
	add rax, rbx
	leave
	ret

function:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 2
	mov [rbp - 4], rax
	mov rax, 3
	mov [rbp - 8], rax
	mov rax, 3
	mov [rbp - 12], rax
	mov rax, rdi
	mov rbx, rsi
	mov r8, rdi
	mov rdi, rax
	mov rsi, r8
	call add
	mov r9, rax
	mov [rbp - 16], r9
	mov rax, 0
	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 32

	mov r9, 20
	mov [rbp - 20], r9
	mov rax, 0
	leave
	ret
