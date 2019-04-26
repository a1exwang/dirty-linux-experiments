import sys

asm_code = """
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
    
{jumps}
    
{loops}

dd.return:
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret
"""

free_registers = ('rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rbp', 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15',)


def generate_loops(add_count, fuse_cmp_jmp=True):
    if fuse_cmp_jmp:
        jump_asm = """
    cmp ${add_count}, %rsi
    je dd.L{add_count}
        """.format(add_count=add_count)
    else:
        jump_asm = """
    cmp ${add_count}, %rsi
    je dd.L{add_count}
        """.format(add_count=add_count)

    adds = "\n"
    for i in range(add_count - 1):
        adds += "    add $1, %{reg}\n".format(reg=free_registers[i])

    if fuse_cmp_jmp:
        t1 = """
dd.L{add_count}:
    add $-1, %rdi{adds}
    cmp $0, %rdi
    jne dd.L{add_count}
    jmp dd.return
    """
    else:
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
for i in range(1, len(free_registers)+1):
    jump, loop = generate_loops(i, True)
    jumps += jump
    loops += loop

print(asm_code.format(jumps=jumps, loops=loops))
