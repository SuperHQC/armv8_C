main:
  mov r1, #4
  bl fact
  MOV r0, #1      @ Load 1 into register r0 (stdout handle)
  SWI 0x6b        @ Print integer in register r1 to stdout
  SWI 0x11        @ Stop program execution
fact: sub sp, sp, #8
  str lr, [sp, #0]
  str r1, [sp, #4]
  cmp r1, #1
  b.ge L1
  mov r1, #1
  add sp, sp, #8
  mov pc, lr
L1: sub r1, r1, #1
  BL fact
  mov r2, r1
  ldr r1, [sp, #4]
  ldr lr, [sp, #0]
  add sp, sp, #8
  mul r1, r2, r1
  mov pc, lr
