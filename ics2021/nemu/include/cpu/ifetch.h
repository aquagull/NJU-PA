#ifndef __CPU_IFETCH_H__

#include <memory/vaddr.h>
/*
 * 完成取指令操作,
 * 底层是调用paddr_read()传入addr与len
 * */
static inline uint32_t instr_fetch(vaddr_t *pc, int len) {
  uint32_t instr = vaddr_ifetch(*pc, len);
  (*pc) += len;
  return instr;
}

#endif
