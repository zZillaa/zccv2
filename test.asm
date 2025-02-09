section .data

section .text
global _start

_start:
    push rbp            ; Save old base pointer
    mov rbp, rsp        ; Set new base pointer

    sub rsp, 24         ; Allocate 24 bytes for local variables

    mov rbx, 5          ; Move 5 into rbx (64-bit register)
    mov qword [rbp - 8], rbx ; Store 64-bit rbx into [rbp - 8]
    mov rbx, 6          ; Move 6 into rbx
    mov qword [rbp - 16], rbx ; Store 64-bit rbx into [rbp - 16]

    mov r8, qword [rbp - 8] ; Load 64-bit value from [rbp - 8] into r8
    mov r9, 10           ; Move 10 into r9
    add r8, r9           ; Add r9 to r8
    mov qword [rbp - 24], r8 ; Store result into [rbp - 24]

    ; Exit system call
    mov rsp, rbp         ; Restore stack pointer
    pop rbp              ; Restore base pointer

    mov rax, 60          ; sys_exit system call
    xor rdi, rdi         ; Exit code 0
    syscall	