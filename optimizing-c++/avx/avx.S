
.section .text
    .global avxadd 

avxadd:
    

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
        
    cmp $15, %rsi
    je dd.L15
        
    

dd.L1:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1

    cmp $0, %rdi
    jne dd.L1
    jmp .exit
    
dd.L2:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2

    cmp $0, %rdi
    jne dd.L2
    jmp .exit
    
dd.L3:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3

    cmp $0, %rdi
    jne dd.L3
    jmp .exit
    
dd.L4:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4

    cmp $0, %rdi
    jne dd.L4
    jmp .exit
    
dd.L5:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5

    cmp $0, %rdi
    jne dd.L5
    jmp .exit
    
dd.L6:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6

    cmp $0, %rdi
    jne dd.L6
    jmp .exit
    
dd.L7:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7

    cmp $0, %rdi
    jne dd.L7
    jmp .exit
    
dd.L8:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8

    cmp $0, %rdi
    jne dd.L8
    jmp .exit
    
dd.L9:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9

    cmp $0, %rdi
    jne dd.L9
    jmp .exit
    
dd.L10:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9
    vaddps %ymm10, %ymm0, %ymm10

    cmp $0, %rdi
    jne dd.L10
    jmp .exit
    
dd.L11:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9
    vaddps %ymm10, %ymm0, %ymm10
    vaddps %ymm11, %ymm0, %ymm11

    cmp $0, %rdi
    jne dd.L11
    jmp .exit
    
dd.L12:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9
    vaddps %ymm10, %ymm0, %ymm10
    vaddps %ymm11, %ymm0, %ymm11
    vaddps %ymm12, %ymm0, %ymm12

    cmp $0, %rdi
    jne dd.L12
    jmp .exit
    
dd.L13:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9
    vaddps %ymm10, %ymm0, %ymm10
    vaddps %ymm11, %ymm0, %ymm11
    vaddps %ymm12, %ymm0, %ymm12
    vaddps %ymm13, %ymm0, %ymm13

    cmp $0, %rdi
    jne dd.L13
    jmp .exit
    
dd.L14:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9
    vaddps %ymm10, %ymm0, %ymm10
    vaddps %ymm11, %ymm0, %ymm11
    vaddps %ymm12, %ymm0, %ymm12
    vaddps %ymm13, %ymm0, %ymm13
    vaddps %ymm14, %ymm0, %ymm14

    cmp $0, %rdi
    jne dd.L14
    jmp .exit
    
dd.L15:
    add $-1, %rdi
    vaddps %ymm1, %ymm0, %ymm1
    vaddps %ymm2, %ymm0, %ymm2
    vaddps %ymm3, %ymm0, %ymm3
    vaddps %ymm4, %ymm0, %ymm4
    vaddps %ymm5, %ymm0, %ymm5
    vaddps %ymm6, %ymm0, %ymm6
    vaddps %ymm7, %ymm0, %ymm7
    vaddps %ymm8, %ymm0, %ymm8
    vaddps %ymm9, %ymm0, %ymm9
    vaddps %ymm10, %ymm0, %ymm10
    vaddps %ymm11, %ymm0, %ymm11
    vaddps %ymm12, %ymm0, %ymm12
    vaddps %ymm13, %ymm0, %ymm13
    vaddps %ymm14, %ymm0, %ymm14
    vaddps %ymm15, %ymm0, %ymm15

    cmp $0, %rdi
    jne dd.L15
    jmp .exit
    

.exit:
    ret

