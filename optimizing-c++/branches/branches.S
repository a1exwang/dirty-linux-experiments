.section .text
    .global branches
branches:
# rsi: iters, rsi: fps, rdx: fps_size
.L0:
    mov %rdx, %rcx

.LP1.in:
    add $-1, %rcx
    call *(%rsi,%rcx,8)
    cmp $0, %rcx
    jne .LP1.in
.LP1.out:
    add $-1, %rdi

    cmp $0, %rdi
    je .L0

    ret

