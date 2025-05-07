section .data

section .text
global function
global main
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 96

	mov qword [rbp - 4 + 0], 0
	mov qword [rbp - 4 + 4], 1
	mov qword [rbp - 4 + 8], 2
	mov qword [rbp - 4 + 12], 0
	mov qword [rbp - 8 + 0], 1
	mov qword [rbp - 8 + 4], 1
	mov qword [rbp - 8 + 8], 1
	mov qword [rbp - 8 + 12], 1
	mov qword [rbp - 8 + 16], 1
	mov qword [rbp - 12 + 0], 0
	mov qword [rbp - 12 + 4], 0
	mov qword [rbp - 12 + 8], 0
	mov qword [rbp - 12 + 12], 0
	mov qword [rbp - 12 + 16], 0
	mov qword [rbp - 12 + 20], 0
	mov qword [rbp - 12 + 24], 0
	mov qword [rbp - 12 + 28], 0
	mov qword [rbp - 12 + 32], 0
	mov rax, 0
	mov [rbp - 4], rax
.L3: 
	mov rax, [rbp - 4]
	mov rbx, 4
	cmp rax, rbx
	setl r8
	movzx r8, r8
	cmp r8, 0
	je .L4
	mov rax, 2
	mov [(null)], rax
	mov rax, [rbp - 4]
	mov [rbp - 4], rax
	jmp .L3
.L4:
	mov rax, 0
	mov [rbp - 4], rax
.L5: 
	mov rax, [rbp - 4]
	mov rbx, 5
	cmp rax, rbx
	setl r8
	movzx r8, r8
	cmp r8, 0
	je .L6
	mov rax, 2
	mov [(null)], rax
	mov rax, [rbp - 4]
	mov [rbp - 4], rax
	jmp .L5
.L6:
	mov rax, 20
	mov [rbp - 24], rax
	mov rax, 30
	mov [rbp - 28], rax
	mov rax, 100
	mov rbx, [rbp - 28]
	add rax, rbx
	mov [rbp - 32], rax
	mov rax, [rbp - 32]

	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 20
	mov [rbp - 4], rax
	mov rax, 0

	leave
	ret
