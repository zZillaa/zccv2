section .data
	arr: 	dq 0, 23, 2, 0

	values: 	dq 3, 4, 5


section .text
global function
global main
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 32

	mov qword [rbp - 4 + 0], 0
	mov qword [rbp - 4 + 4], 1
	mov qword [rbp - 4 + 8], 2
	mov qword [rbp - 4 + 12], 0
	mov rax, 0
	mov [rbp - 4], rax
.L3: 
	mov rax, [rbp - 4]
	mov rbx, 5
	cmp rax, rbx
	setl r8
	movzx r8, r8
	cmp r8, 0
	je .L4
	mov rax, [rbp - 4]
	mov [rbp - 4], rax
	jmp .L3
.L4:
	mov rax, 0
	mov [rbp - 12], rax
.L5:
	mov rax, [rbp - 12]
	mov rbx, 5
	cmp rax, rbx
	setl r8
	movzx r8, r8
	cmp r8, 0
	je .L6
	jmp .L5
.L6:
	mov rax, 0

	leave
	ret

main:
	push rbp
	mov rbp, rsp
	sub rsp, 32

	mov rax, 5
	mov [rbp - 4], rax
	mov qword [rbp - 8 + 0], 10
	mov qword [rbp - 8 + 4], 20
	mov qword [rbp - 8 + 8], 30
	mov qword [rbp - 8 + 12], 40
	mov qword [rbp - 8 + 16], 50
	mov rax, 0

	leave
	ret
