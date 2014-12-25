.data
_g_n: .word 0
.text
.text
_start_fact:
str lr, [sp, #0]
str fp, [sp, #-4]
add fp, sp, #-4
add sp, sp, #-8
ldr lr, =_frameSize_fact
ldr lr, [lr, #0]
sub sp, sp, lr
str r4, [sp, #4]
str r5, [sp, #8]
str r6, [sp, #12]
str r7, [sp, #16]
str r8, [sp, #20]
str r9, [sp, #24]
str r10, [sp, #28]
str r11, [sp, #32]
vstr.f32 s16, [sp, #36]
vstr.f32 s17, [sp, #40]
vstr.f32 s18, [sp, #44]
vstr.f32 s19, [sp, #48]
vstr.f32 s20, [sp, #52]
vstr.f32 s21, [sp, #56]
vstr.f32 s22, [sp, #60]
vstr.f32 s23, [sp, #64]

ldr r10, =_g_n
ldr r4, [r10,#0]
.data
_CONSTANT_1: .word 1
.text
ldr r5, =_CONSTANT_1
ldr r5, [r5, #0]
cmp r4, r5
mov r4, #0
moveq r4, #1
cmp r4, #0
beq _elseLabel_0


ldr r10, =_g_n
ldr r4, [r10,#0]
mov r0, r4
b _end_fact
b _ifExitLabel_0
_elseLabel_0:


ldr r10, =_g_n
ldr r4, [r10,#0]
.data
_CONSTANT_2: .word 1
.text
ldr r5, =_CONSTANT_2
ldr r5, [r5, #0]
sub r4, r4, r5
ldr r5, =_g_n
str r4, [r5, #0]

ldr r10, =_g_n
ldr r4, [r10,#0]
bl _start_fact
mov r5, r0
mul r4, r4, r5
mov r0, r4
b _end_fact
_ifExitLabel_0:
_end_fact:
ldr r4, [sp, #4]
ldr r5, [sp, #8]
ldr r6, [sp, #12]
ldr r7, [sp, #16]
ldr r8, [sp, #20]
ldr r9, [sp, #24]
ldr r10, [sp, #28]
ldr r11, [sp, #32]
vldr.32 s16, [sp, #36]
vldr.32 s17, [sp, #40]
vldr.32 s18, [sp, #44]
vldr.32 s19, [sp, #48]
vldr.32 s20, [sp, #52]
vldr.32 s21, [sp, #56]
vldr.32 s22, [sp, #60]
vldr.32 s23, [sp, #64]
ldr lr, [fp, #4]
mov sp, fp
add sp, sp, #4
ldr fp, [fp,#0]
bx lr
.data
_frameSize_fact: .word 64
.text
_start_MAIN:
str lr, [sp, #0]
str fp, [sp, #-4]
add fp, sp, #-4
add sp, sp, #-8
ldr lr, =_frameSize_MAIN
ldr lr, [lr, #0]
sub sp, sp, lr
str r4, [sp, #4]
str r5, [sp, #8]
str r6, [sp, #12]
str r7, [sp, #16]
str r8, [sp, #20]
str r9, [sp, #24]
str r10, [sp, #28]
str r11, [sp, #32]
vstr.f32 s16, [sp, #36]
vstr.f32 s17, [sp, #40]
vstr.f32 s18, [sp, #44]
vstr.f32 s19, [sp, #48]
vstr.f32 s20, [sp, #52]
vstr.f32 s21, [sp, #56]
vstr.f32 s22, [sp, #60]
vstr.f32 s23, [sp, #64]

.data
_CONSTANT_3: .ascii "Enter a number:\000"
.align 2
.text
ldr r4, =_CONSTANT_3
mov r0, r4
bl _write_str

bl _read_int
mov r4, r0
ldr r5, =_g_n
str r4, [r5, #0]

ldr r10, =_g_n
ldr r4, [r10,#0]
.data
_CONSTANT_4: .word 1
.text
ldr r5, =_CONSTANT_4
ldr r5, [r5, #0]
add r4, r4, r5
ldr r5, =_g_n
str r4, [r5, #0]

ldr r10, =_g_n
ldr r4, [r10,#0]
.data
_CONSTANT_6: .word 1
.text
ldr r5, =_CONSTANT_6
ldr r5, [r5, #0]
cmp r4, r5
mov r4, #0
movgt r4, #1
cmp r4, #0
beq _elseLabel_5


bl _start_fact
mov r4, r0
str r4, [fp, #-4]
b _ifExitLabel_5
_elseLabel_5:


.data
_CONSTANT_7: .word 1
.text
ldr r4, =_CONSTANT_7
ldr r4, [r4, #0]
str r4, [fp, #-4]
_ifExitLabel_5:

.data
_CONSTANT_8: .ascii "The factorial is \000"
.align 2
.text
ldr r4, =_CONSTANT_8
mov r0, r4
bl _write_str

ldr r4, [fp, #-4]
mov r0, r4
bl _write_int

.data
_CONSTANT_9: .ascii "\n\000"
.align 2
.text
ldr r4, =_CONSTANT_9
mov r0, r4
bl _write_str
_end_MAIN:
ldr r4, [sp, #4]
ldr r5, [sp, #8]
ldr r6, [sp, #12]
ldr r7, [sp, #16]
ldr r8, [sp, #20]
ldr r9, [sp, #24]
ldr r10, [sp, #28]
ldr r11, [sp, #32]
vldr.32 s16, [sp, #36]
vldr.32 s17, [sp, #40]
vldr.32 s18, [sp, #44]
vldr.32 s19, [sp, #48]
vldr.32 s20, [sp, #52]
vldr.32 s21, [sp, #56]
vldr.32 s22, [sp, #60]
vldr.32 s23, [sp, #64]
ldr lr, [fp, #4]
mov sp, fp
add sp, sp, #4
ldr fp, [fp,#0]
bx lr
.data
_frameSize_MAIN: .word 68
