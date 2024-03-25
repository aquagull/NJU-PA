#include "../include/rtl/rtl.h"
#include <stdio.h>
#include "../../../monitor/ftrace/ftracer.h"
#define c_sext(a, num) (((sword_t)a << num) >> num)

def_EHelper(lui)
{
    rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi)
{
    rtl_addi(s, ddest, dsrc1, c_sext(id_src2->imm, 20));
}

def_EHelper(auipc)
{
    rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(jal)
{
    rtl_addi(s, ddest, &s->pc, 4);
    rtl_addi(s, &s->dnpc, &s->pc, c_sext(id_src1->imm, 11));
    stack_call(s->pc, s->dnpc);
}

def_EHelper(jalr)
{
    rtl_addi(s, s0, &s->pc, 4);
    rtl_addi(s, &s->dnpc, dsrc1, c_sext(id_src2->imm, 20));
    rtl_andi(s, &s->dnpc, &s->dnpc, ~1);
    rtl_addi(s, ddest, s0, 0);
    if (s->isa.instr.i.rd == 0 && s->isa.instr.i.rs1 == 1 && s->isa.instr.i.simm11_0 == 0)
    {
        // ret
        stack_return(s->pc, s->dnpc);
    }
    else
        stack_call(s->pc, s->dnpc);
}

def_EHelper(add)
{
    rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub)
{
    rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll)
{
    rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt)
{
    rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu)
{
    rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(xor)
{
    rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl)
{
    rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra)
{
    rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or)
{
    rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and)
{
    rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltiu)
{
    rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slti)
{
    rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori)
{
    rtl_xori(s, ddest, dsrc1, c_sext(id_src2->imm, 20));
}

def_EHelper(ori)
{
    rtl_ori(s, ddest, dsrc1, c_sext(id_src2->imm, 20));
}

def_EHelper(andi)
{
    rtl_andi(s, ddest, dsrc1, c_sext(id_src2->imm, 20));
}

def_EHelper(slli)
{
    rtl_slli(s, ddest, dsrc1, id_src2->imm & 0b11111u);
}

def_EHelper(srli)
{
    rtl_srli(s, ddest, dsrc1, id_src2->imm & 0b11111u);
}

def_EHelper(srai)
{
    rtl_srai(s, ddest, dsrc1, id_src2->imm & 0b11111u);
}

def_EHelper(beq)
{
    rtl_setrelop(s, RELOP_EQ, s0, dsrc1, dsrc2);
    if (*s0)
    {
        rtl_addi(s, s0, &s->pc, c_sext(id_dest->imm, 19));
        rtl_j(s, *s0);
    }
}

def_EHelper(bne)
{
    rtl_setrelop(s, RELOP_NE, s0, dsrc1, dsrc2);
    if (*s0)
    {
        rtl_addi(s, s0, &s->pc, c_sext(id_dest->imm, 19));
        rtl_j(s, *s0);
    }
}
def_EHelper(blt)
{
    rtl_setrelop(s, RELOP_LT, s0, dsrc1, dsrc2);

    if (*s0)
    {
        rtl_addi(s, s0, &s->pc, c_sext(id_dest->imm, 19));
        rtl_j(s, *s0);
    }
}

def_EHelper(bge)
{
    rtl_setrelop(s, RELOP_GE, s0, dsrc1, dsrc2);

    if (*s0)
    {
        rtl_addi(s, s0, &s->pc, c_sext(id_dest->imm, 19));
        rtl_j(s, *s0);
    }
}

def_EHelper(bltu)
{
    rtl_setrelop(s, RELOP_LTU, s0, dsrc1, dsrc2);
    if (*s0)
    {
        rtl_addi(s, s0, &s->pc, c_sext(id_dest->imm, 19));
        rtl_j(s, *s0);
    }
}

def_EHelper(bgeu)
{
    rtl_setrelop(s, RELOP_GEU, s0, dsrc1, dsrc2);
    if (*s0)
    {
        rtl_addi(s, s0, &s->pc, c_sext(id_dest->imm, 19));
        rtl_j(s, *s0);
    }
}

def_EHelper(mul)
{
    rtl_mulu_lo(s, id_dest->preg, id_src1->preg, id_src2->preg);
}

def_EHelper(mulh)
{
    rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulhu)
{
    rtl_mulu_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div)
{
    rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(divu)
{
    rtl_divu_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem)
{
    rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(remu)
{
    rtl_divu_r(s, ddest, dsrc1, dsrc2);
}
