section .data

section .text
global function
global function2
global function3
global main
global _start

_start:


function:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 2
	mov [rbp - 4], rax
	mov rax, 20
	mov [rbp - 8], rax
	mov rax, 100
	mov rbx, [rbp - 8]
	add rax, rbx
	mov [rbp - 12], rax
	mov rax, [rbp - 12]
	mov rbx, 200
	add rax, rbx
	mov [rbp - 16], rax

function2:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 10
	mov [rbp - 4], rax
	mov rax, 20
	mov [rbp - 8], rax
	mov rax, [rbp - 4]
	mov rbx, [rbp - 8]
	add rax, rbx
	mov [rbp - 12], rax

function3:
	push rbp
	mov rbp, rsp
	sub rsp, 16

	mov rax, 10
	mov [rbp - 4], rax
	push rbp
	mov rbp, rsp
	sub rsp, 16

