asm_code = """
.section .text
    .global avxmov 
// rdi: dst, rsi: src, rdx: count, rcx: p
avxmov:

    add %rdi, %rdx
    
{jumps}
    
{loops}

.exit:
    ret
"""


def generate_loops(n):
    jump_asm = """
    cmp ${n}, %rcx
    je dd.L{n}
        """.format(n=n)

    loads = "\n"
    for i in range(1, n + 1):
        loads += "    vmovdqu (%rsi), %ymm{reg}\n    add $256, %rsi\n".format(reg=i, offset=i*256)
    stores = "\n"
    for i in range(1, n + 1):
        stores += "    vmovdqu %ymm{reg}, (%rdi) \n    add $256, %rdi\n".format(reg=i, offset=i*256)

    t1 = """
dd.L{n}:
    cmp %rdx, %rdi
    jge .exit
    
    {loads}
    {stores}
    
    jmp dd.L{n}
    """
    loop_asm = t1.format(n=n, loads=loads, stores=stores)
    return jump_asm, loop_asm


jumps = ""
loops = ""
for i in range(1, 16):
    jump, loop = generate_loops(i)
    jumps += jump
    loops += loop

print(asm_code.format(jumps=jumps, loops=loops))
