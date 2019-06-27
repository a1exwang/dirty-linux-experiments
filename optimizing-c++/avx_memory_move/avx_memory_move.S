
.section .text
    .global avxmov 
// rdi: dst, rsi: src, rdx: count, rcx: p
avxmov:

    add %rdi, %rdx
    

    cmp $1, %rcx
    je dd.L1
        
    cmp $2, %rcx
    je dd.L2
        
    cmp $3, %rcx
    je dd.L3
        
    cmp $4, %rcx
    je dd.L4
        
    cmp $5, %rcx
    je dd.L5
        
    cmp $6, %rcx
    je dd.L6
        
    cmp $7, %rcx
    je dd.L7
        
    cmp $8, %rcx
    je dd.L8
        
    cmp $9, %rcx
    je dd.L9
        
    cmp $10, %rcx
    je dd.L10
        
    cmp $11, %rcx
    je dd.L11
        
    cmp $12, %rcx
    je dd.L12
        
    cmp $13, %rcx
    je dd.L13
        
    cmp $14, %rcx
    je dd.L14
        
    cmp $15, %rcx
    je dd.L15
        
    

dd.L1:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi

    
    jmp dd.L1
    
dd.L2:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi

    
    jmp dd.L2
    
dd.L3:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi

    
    jmp dd.L3
    
dd.L4:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi

    
    jmp dd.L4
    
dd.L5:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi

    
    jmp dd.L5
    
dd.L6:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi

    
    jmp dd.L6
    
dd.L7:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi

    
    jmp dd.L7
    
dd.L8:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi

    
    jmp dd.L8
    
dd.L9:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi

    
    jmp dd.L9
    
dd.L10:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi
    vmovdqu (%rsi), %ymm10
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm10, (%rdi) 
    add $256, %rdi

    
    jmp dd.L10
    
dd.L11:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi
    vmovdqu (%rsi), %ymm10
    add $256, %rsi
    vmovdqu (%rsi), %ymm11
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm10, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm11, (%rdi) 
    add $256, %rdi

    
    jmp dd.L11
    
dd.L12:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi
    vmovdqu (%rsi), %ymm10
    add $256, %rsi
    vmovdqu (%rsi), %ymm11
    add $256, %rsi
    vmovdqu (%rsi), %ymm12
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm10, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm11, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm12, (%rdi) 
    add $256, %rdi

    
    jmp dd.L12
    
dd.L13:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi
    vmovdqu (%rsi), %ymm10
    add $256, %rsi
    vmovdqu (%rsi), %ymm11
    add $256, %rsi
    vmovdqu (%rsi), %ymm12
    add $256, %rsi
    vmovdqu (%rsi), %ymm13
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm10, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm11, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm12, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm13, (%rdi) 
    add $256, %rdi

    
    jmp dd.L13
    
dd.L14:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi
    vmovdqu (%rsi), %ymm10
    add $256, %rsi
    vmovdqu (%rsi), %ymm11
    add $256, %rsi
    vmovdqu (%rsi), %ymm12
    add $256, %rsi
    vmovdqu (%rsi), %ymm13
    add $256, %rsi
    vmovdqu (%rsi), %ymm14
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm10, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm11, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm12, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm13, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm14, (%rdi) 
    add $256, %rdi

    
    jmp dd.L14
    
dd.L15:
    cmp %rdx, %rdi
    jge .exit
    
    
    vmovdqu (%rsi), %ymm1
    add $256, %rsi
    vmovdqu (%rsi), %ymm2
    add $256, %rsi
    vmovdqu (%rsi), %ymm3
    add $256, %rsi
    vmovdqu (%rsi), %ymm4
    add $256, %rsi
    vmovdqu (%rsi), %ymm5
    add $256, %rsi
    vmovdqu (%rsi), %ymm6
    add $256, %rsi
    vmovdqu (%rsi), %ymm7
    add $256, %rsi
    vmovdqu (%rsi), %ymm8
    add $256, %rsi
    vmovdqu (%rsi), %ymm9
    add $256, %rsi
    vmovdqu (%rsi), %ymm10
    add $256, %rsi
    vmovdqu (%rsi), %ymm11
    add $256, %rsi
    vmovdqu (%rsi), %ymm12
    add $256, %rsi
    vmovdqu (%rsi), %ymm13
    add $256, %rsi
    vmovdqu (%rsi), %ymm14
    add $256, %rsi
    vmovdqu (%rsi), %ymm15
    add $256, %rsi

    
    vmovdqu %ymm1, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm2, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm3, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm4, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm5, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm6, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm7, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm8, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm9, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm10, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm11, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm12, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm13, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm14, (%rdi) 
    add $256, %rdi
    vmovdqu %ymm15, (%rdi) 
    add $256, %rdi

    
    jmp dd.L15
    

.exit:
    ret

