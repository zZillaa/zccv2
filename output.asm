section .data

section .text
global _start

_start:
	mov r8, 5
	mov [rbp - 8], r8
	mov r8, 6
	mov [rbp - 16], r8
	mov r8, [rbp - 8]
	mov r9, 10
	add r8, r9
	mov [rbp - 24], r8
	mov r8, [rbp - 8]
	mov r9, [rbp - 8]
	sub r8, r9
	mov [rbp - 32], r8
