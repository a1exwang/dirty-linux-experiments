
.section .text
    .global main
    .global loop_start

loop_start:
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15

    mov $0x0, %rax
    

    cmp $1, %rsi
    je dd.L1
        
    cmp $2, %rsi
    je dd.L2
        
    cmp $3, %rsi
    je dd.L3
        
    cmp $4, %rsi
    je dd.L4
        
    cmp $5, %rsi
    je dd.L5
        
    cmp $6, %rsi
    je dd.L6
        
    cmp $7, %rsi
    je dd.L7
        
    cmp $8, %rsi
    je dd.L8
        
    cmp $9, %rsi
    je dd.L9
        
    cmp $10, %rsi
    je dd.L10
        
    cmp $11, %rsi
    je dd.L11
        
    cmp $12, %rsi
    je dd.L12
        
    cmp $13, %rsi
    je dd.L13
        
    cmp $14, %rsi
    je dd.L14
        
    

dd.L1:
    add $-1, %rdi

    cmp $0, %rdi
    jne dd.L1
    jmp dd.return
    
dd.L2:
    add $-1, %rdi
    add $1, %rax

    cmp $0, %rdi
    jne dd.L2
    jmp dd.return
    
dd.L3:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx

    cmp $0, %rdi
    jne dd.L3
    jmp dd.return
    
dd.L4:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx

    cmp $0, %rdi
    jne dd.L4
    jmp dd.return
    
dd.L5:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx

    cmp $0, %rdi
    jne dd.L5
    jmp dd.return
    
dd.L6:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi

    cmp $0, %rdi
    jne dd.L6
    jmp dd.return
    
dd.L7:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp

    cmp $0, %rdi
    jne dd.L7
    jmp dd.return
    
dd.L8:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8

    cmp $0, %rdi
    jne dd.L8
    jmp dd.return
    
dd.L9:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8
    add $1, %r9

    cmp $0, %rdi
    jne dd.L9
    jmp dd.return
    
dd.L10:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8
    add $1, %r9
    add $1, %r10

    cmp $0, %rdi
    jne dd.L10
    jmp dd.return
    
dd.L11:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8
    add $1, %r9
    add $1, %r10
    add $1, %r11

    cmp $0, %rdi
    jne dd.L11
    jmp dd.return
    
dd.L12:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8
    add $1, %r9
    add $1, %r10
    add $1, %r11
    add $1, %r12

    cmp $0, %rdi
    jne dd.L12
    jmp dd.return
    
dd.L13:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8
    add $1, %r9
    add $1, %r10
    add $1, %r11
    add $1, %r12
    add $1, %r13

    cmp $0, %rdi
    jne dd.L13
    jmp dd.return
    
dd.L14:
    add $-1, %rdi
    add $1, %rax
    add $1, %rbx
    add $1, %rcx
    add $1, %rdx
    add $1, %rsi
    add $1, %rbp
    add $1, %r8
    add $1, %r9
    add $1, %r10
    add $1, %r11
    add $1, %r12
    add $1, %r13
    add $1, %r14

    cmp $0, %rdi
    jne dd.L14
    jmp dd.return
    

dd.return:
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret

