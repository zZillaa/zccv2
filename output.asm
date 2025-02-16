section .data
	arr dq 1, 2, 3, 4, 5

	z dq 0, 0, 0

	s dq 2, 1

	other dq 6, 7, 8, 9, 10

	x dq 4
	y dq 10
	z dq 20

section .text
global function
global add_more_numbers
global function2
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 5
	mov [rbp - 8], rax
	mov rbx, 20
	mov [rbp - 16], rbx
	mov [rbp - 8], r8
	mov [rbp - 16], r9
	add r8, r9
	mov [rbp - 24], r9
	mov rax, r9

	leave
	ret

add_more_numbers:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov r9, 3
	mov [rbp - 8], r9
	mov r10, 2
	mov [rbp - 16], r10
	mov [rbp - 16], r11
	mov [rbp - 8], r12
	add r11, r12
	mov [rbp - 24], r12
	mov rax, r12

	leave
	ret

function2:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov r12, 3
	mov [rbp - 8], r12
	mov r13, 10
	mov [rbp - 16], r13
	mov r14, 20
	mov [rbp - 24], r14
	mov [rbp - 24], r15
	mov rax, r15

	leave
	ret
