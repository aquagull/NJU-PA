#include <isa.h>

/*
    将当前PC值保存到mepc寄存器
    在mcause寄存器中设置异常号
    从mtvec寄存器中取出异常入口地址
    跳转到异常入口地址
*/
#define REG_MTVEC 0
#define REG_MCAUSE 1
#define REG_MSTATUS 2
#define REG_MEPC 3
#define REG_SATP 4
#define REG_MSCRATCH 5
#define MSTATUS_MPIE 0x00000080
#define MSTATUS_MIE 0x00000008
word_t isa_raise_intr(word_t NO, vaddr_t epc)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  cpu.csr[REG_MEPC]._32 = epc;
  cpu.csr[REG_MCAUSE]._32 = NO;
  cpu.csr[REG_MSTATUS]._32 =0x1800;
      // if (cpu.csr[REG_MSTATUS]._32 & MSTATUS_MIE)
      //   cpu.csr[REG_MSTATUS]._32 |= MSTATUS_MPIE;
      // else
      //   cpu.csr[REG_MSTATUS]._32 &= (~MSTATUS_MPIE);

      //cpu.csr[REG_MSTATUS]._32 &= (~MSTATUS_MIE);
  //Log("The mtvec = %x", cpu.csr[REG_MTVEC]._32);
  return cpu.csr[REG_MTVEC]._32;
}

word_t isa_query_intr()
{
  return INTR_EMPTY;
}
