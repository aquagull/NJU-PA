#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap) \
    f(addi) f(jal) f(auipc) f(jalr) f(add) f(sub) f(sll) f(slt) \
    f(xor) f(sltu) f(and) f(or) f(sra) f(srl) f(sltiu) f(slti) f(xori) \
    f(ori) f(andi) f(slli) f(srli) f(srai)\
    f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) f(sh) f(sb) \
    f(lbu) f(lh) f(lb) f(lhu) f(mul) f(mulh) f(mulhu) f(div) f(divu)\
    f(rem) f(remu) f(ecall) f(csrrs) f(csrrw) f(mret)

def_all_EXEC_ID();
