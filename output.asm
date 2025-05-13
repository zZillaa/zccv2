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
	mov dword [rbp - 16], 2
	mov dword [rbp - 20], 3
	mov dword [rbp - 24], 4


	mov dword [rbp - 28], 5
	mov dword [rbp - 32], 6
	mov dword [rbp - 36], 0

	mov rax, 4
	mov [rbp - 40], rax
	mov rax, 2
	mov [rbp - 20], rax
	mov [rbp - 44], 0
.L2: 
	mov rbx, [rbp - 44]
	mov r8, 10
	cmp rbx, r8
	jg .L3
	mov rbx, [rbp - 44]
	imul rbx, 4
	lea [rbp - 12], r9
	add r9, rbx
	mov r8, r9
	mov rbx, 3
	cmp r8, rbx
	jg .L4
	mov rbx, 2
	mov [rbp - 20], rbx
.L4:
	inc [rbp - 44]
	jmp .L2
.L3:
	mov rax, 0

	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov r8, 20
	mov [rbp - 4], r8
	mov rax, 0

	leave
	ret
