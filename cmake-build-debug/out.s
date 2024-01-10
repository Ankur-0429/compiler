.global _main
.align 2
_main:
    ldr x1, =14
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x1, =240
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x1, =1
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x3, [sp, #0]
    add sp, sp, #16
    ldr x4, [sp, #0]
    add sp, sp, #16
    add x1, x3, x4
    mov x1, x1
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x3, [sp, #0]
    add sp, sp, #16
    ldr x4, [sp, #0]
    add sp, sp, #16
    add x1, x3, x4
    mov x1, x1
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x1, [sp, #0]
    mov x1, x1
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x1, [sp, #0]
    mov x1, x1
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x1, [sp, #0]
    mov x1, x1
    sub sp, sp, #16
    str x1, [sp, #0]
    ldr x1, [sp, #0]
    mov x1, x1
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