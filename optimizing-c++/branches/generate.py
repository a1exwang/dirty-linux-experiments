asm_code = """
.section .text
    .global branches

branches:
    
{jumps}
    
{loops}

.exit:
    ret
"""


def generate(add_count):
    jump_asm = """
    cmp ${add_count}, %rsi
    je dd.L{add_count}
        """.format(add_count=add_count)

    adds = "\n"
    for i in range(1, add_count+1):
        adds += "    vaddps %ymm{reg}, %ymm0, %ymm{reg}\n".format(reg=i)

    t1 = """
dd.L{add_count}:
    add $-1, %rdi
    cmp $0, %rdi{adds}
    jne dd.L{add_count}
    jmp dd.return
    """

    loop_asm = t1.format(add_count=add_count, adds=adds)
    return jump_asm, loop_asm


jumps = ""
loops = ""
for i in range(1, 16):
    jump, loop = generate(i, True)
    jumps += jump
    loops += loop

print(asm_code.format(jumps=jumps, loops=loops))
