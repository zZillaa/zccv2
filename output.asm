section .data

section .text
global function
global main
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 48

	mov rax, 2
	mov [rbp - 4], rax
	mov rax, 3
	mov [rbp - 8], rax
	inc [rbp - 8]

	mov dword [rbp - 12], 1
	mov dword [rbp - 16], 6
	mov dword [rbp - 20], 8

	mov [rbp - 24], 0
.L1: 
	mov rax, [rbp - 24]
	mov rbx, 10
	cmp rax, rbx
	jg .L3
	mov rax, [rbp - 24]
	imul rax, 4
	lea r8, [rbp - 12]
	add r8, rax
	mov rbx, [r8]
	mov rax, 10
	cmp rbx, rax
	jg .L5
.L4:
	mov rax, 5
	mov [rbp - 32], rax
	jmp .L2
.L5:
	mov rbx, [rbp - 24]
	imul rbx, 4
	lea r9, [rbp - 12]
	add r9, rbx
	mov r8, [r9]
	mov rbx, 10
	cmp r8, rbx
	jg .L7
.L6:
	mov rbx, 2
	mov [rbp - 20], rbx
	jmp .L2
.L7:
	mov r8, 8
	mov [rbp - 44], r8
.L2:
	inc [rbp - 24]
	jmp .L1
.L3:

	mov dword [rbp - 28], 0
	mov dword [rbp - 32], 4
	mov dword [rbp - 36], 5
	mov dword [rbp - 40], 4

	mov rax, 0
	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov r9, 20
	mov [rbp - 4], r9
	mov rax, 0
	leave
	ret
