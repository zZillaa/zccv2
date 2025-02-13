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
global _start

_start:


add:
	push rbp
	mov rbp, rsp
	sub rsp, 2

function:
	push rbp
	mov rbp, rsp
	sub rsp, 4

function1:
	push rbp
	mov rbp, rsp
	sub rsp, 5

function3:
	push rbp
	mov rbp, rsp
	sub rsp, 3

function4:
	push rbp
	mov rbp, rsp
	sub rsp, 3

function5:
	push rbp
	mov rbp, rsp
	sub rsp, 3

function6:
	push rbp
	mov rbp, rsp
	sub rsp, 4
