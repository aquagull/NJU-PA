#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  // TODO: fix the order of these members to match trap.S
  uintptr_t gpr[32], mcause,mstatus ,mepc ;
  void *pdir;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10]
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]  //返回值在a0寄存器上
// register intptr_t _gpr1 asm (GPR1) = type;
//   register intptr_t _gpr2 asm (GPR2) = a0;
//   register intptr_t _gpr3 asm (GPR3) = a1;
//   register intptr_t _gpr4 asm (GPR4) = a2;
//   register intptr_t ret asm (GPRx);
//   asm volatile (SYSCALL : "=r" (ret) : "r"(_gpr1), "r"(_gpr2), "r"(_gpr3), "r"(_gpr4));
//   return ret;
#endif
