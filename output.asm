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

	mov rax, 0
	leave
	ret

function:
	push rbp
	mov rbp, rsp
	sub rsp, 32

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
	mov rbx, 3
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
	mov rbx, [rbp - 24]
	imul rbx, 4
	lea r9, [rbp - 12]
	add r9, rbx
	mov r8, [r9]
	jmp .L2
.L5:
	mov rbx, [rbp - 24]
	imul rbx, 4
	lea r10, [rbp - 12]
	add r10, rbx
	mov r9, [r10]
	mov rbx, 10
	cmp r9, rbx
	jle .L7
.L6:
	mov rbx, 2
	mov r9, [rbp - 24]
	imul r9, 4
	lea r11, [rbp - 12]
	add r11, r9
	mov r10, [r11]
	jmp .L2
.L7:
	mov r9, 8
	mov r11, [rbp - 24]
	imul r11, 4
	lea r13, [rbp - 12]
	add r13, r11
	mov r12, [r13]
.L2:
	inc [rbp - 24]
	jmp .L1
.L3:
	mov r11, 3
	mov [rbp - 28], r11
	mov r11, [rbp - 28]
	mov r13, 4
	mov rdi, r11
	mov rsi, r13
	call add
	mov r14, 0
	mov [rbp - 32], r14
	mov rax, 0
	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov r14, 20
	mov [rbp - 4], r14
	mov rax, 0
	leave
	ret
