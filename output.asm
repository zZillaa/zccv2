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
	sub rsp, 48

	mov rax, 2
	mov [rbp - 4], rax
	mov rax, 3
	mov [rbp - 8], rax
	mov rax, 3
	mov [rbp - 12], rax

	mov dword [rbp - 16], 1
	mov dword [rbp - 20], 2
	mov dword [rbp - 24], 3
	mov dword [rbp - 28], 4
	mov dword [rbp - 32], 5

	mov rdi, 4
	mov rsi, 0
	mov rdi, 0
	call add
	mov rax, rax
	mov [rbp - 36], rax
	mov rax, 0
	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 48

	mov rax, 20
	mov [rbp - 40], rax
	mov rax, 0
	leave
	ret
