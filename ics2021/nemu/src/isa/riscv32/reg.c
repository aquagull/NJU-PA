#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

const char *csrs[] = {
    "mtvec", "mepc", "mstatus", "mcause"};

void isa_reg_display()
{
  int length = sizeof(regs) / sizeof(regs[0]);
  for (int i = 0; i < length; i++)
    printf("$%s ---------> %x\n", regs[i], cpu.gpr[i]._32);
}

word_t isa_reg_str2val(const char *s, bool *success)
{
  *success = false;
  int length = sizeof(regs) / sizeof(regs[0]);
  for (int i = 0; i < length; i++)
  {
    if (!strcmp(regs[i], s + 1))
    {
      *success = true;
      //printf("i = %d\n",i);
      return cpu.gpr[i]._32;
    }
  }

  if (!strcmp("pc", s + 1))
  {
    *success = true;
    return cpu.pc;
  }
  int csrlen = sizeof(csrs) / sizeof(csrs[0]);
  for (int i = 0; i < csrlen; i++)
  {
    if (!strcmp(csrs[i], s + 1))
    {
      *success = true;
      return cpu.csr[i]._32;
    }
  }
  return 0;
}
