.section .text
    .global main
    .global memory_move

# rdi: dst, rsi: src, rdx: count, rcx: batch_size
memory_move:
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15

    mov %rcx, %rbx
    mov $0, %rcx

    cmp $1, %rbx
    je L1
    cmp $2, %rbx
    je L2
    cmp $4, %rbx
    je L4
    cmp $8, %rbx
    je L8

    jmp return

L1:
    add $1, %rcx

    mov (%rsi,%rcx,1), %al
    mov %al, (%rdi, %rcx, 1)

    cmp %rdx, %rcx
    jne L1

L2:
    cmp %rdx, %rcx
    je return
    add $1, %rcx

    mov (%rsi,%rcx,2), %ax
    mov %ax, (%rdi, %rcx, 2)

    jmp L2

L4:
    cmp %rdx, %rcx
    je return
    add $1, %rcx

    mov (%rsi,%rcx,4), %eax
    mov %eax, (%rdi, %rcx, 4)

    jmp L4

L8:
    cmp %rdx, %rcx
    je return
    add $1, %rcx

    mov (%rsi,%rcx,8), %rax
    mov %rax, (%rdi, %rcx, 8)

    jmp L8

return:
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret

