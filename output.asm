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

	mov [rbp - 4], rdi
	mov rax, [rbp - 4]
	leave
	ret

function:
	push rbp
	mov rbp, rsp
	sub rsp, 48

	mov rbx, 2
	mov [rbp - 4], rbx
	mov rbx, 3
	mov [rbp - 8], rbx
	mov rbx, 3
	mov [rbp - 12], rbx

	mov dword [rbp - 16], 1
	mov dword [rbp - 20], 2
	mov dword [rbp - 24], 3
	mov dword [rbp - 28], 4
	mov dword [rbp - 32], 5

	mov rbx, [rbp - 12]
	mov rdi, rbx
	call add
	mov rbx, rax
	mov [rbp - 36], rbx
	mov rax, 0
	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 48

	mov rbx, 20
	mov [rbp - 40], rbx
	mov rax, 0
	leave
	ret
