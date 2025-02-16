section .data
	arr dq 1, 2, 3, 4, 5

	z dq 0, 0, 0

	s dq 2, 1

	other dq 6, 7, 8, 9, 10

	x dq 4
	y dq 10
	z dq 20

section .text
global add
global function
global function1
global function3
global function4
global function5
global function6
global function7
global multiply_two_numbers
global _start

_start:


add:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, [rbp - 16]
	mov rbx, [rbp - 16]
	add rax, rbx
	mov rax, rax

function:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov rax, [rbp - 24]
	mov rax, rax

function1:
	push rbp
	mov rbp, rsp
	sub rsp, 32
	mov rax, [rbp - 24]
	mov rax, rax

function3:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, [rbp - 24]
	mov rax, rax

function4:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, [rbp - 24]
	mov rax, rax

function5:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, [rbp - 24]
	mov rax, rax

function6:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, [rbp - 32]
	mov rax, rax

function7:
	push rbp
	mov rbp, rsp
	sub rsp, 64
	mov rax, [rbp - 8]
	mov rax, rax

multiply_two_numbers:
	push rbp
	mov rbp, rsp
	sub rsp, 16
	mov rax, [rbp - 16]
	mov rbx, [rbp - 16]
	mul rbx
	mov r8, rax
	mov rax, r8
