.global _main
.align 2
_main:
    mov x1, #0
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x2, [sp, #0]
    add sp, sp, #16
    mov x0, x2
    mov x16, #1
    svc 0
    mov x0, #0
    mov x16, #1
    svc 0