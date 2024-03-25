#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

void error_finfo();
extern const char *regs[];

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc)
{
  for (int i = 0; i < 32; i++)
  {
    if (ref_r->gpr[i]._32 != cpu.gpr[i]._32)
    {
      error_finfo();
      Log("To the regs[%s],expected %x ,but got %x", regs[i], ref_r->gpr[i]._32,
          cpu.gpr[i]._32);
      // assert();
      return false;
    }
  }

  if (ref_r->pc == pc)
    return true;
  else
  {
    Log("PC expected %x, but got %x", ref_r->pc, pc);
    // assert();
    return false;
  }
}

void isa_difftest_attach()
{
}
