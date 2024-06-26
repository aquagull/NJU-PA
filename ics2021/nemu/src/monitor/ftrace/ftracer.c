#include "ftracer.h"
#include <utils.h>
#include <stdlib.h>
#include <isa.h>

#ifdef CONFIG_FTRACE

STACK_ENTRY header;
STACK_ENTRY *end = &header;

#endif

void init_ftracer(const char *elf_file)
{
#ifdef CONFIG_FTRACE

    if (elf_file)
        init_elf(elf_file, 0);
    header.des_info = NULL;
    header.cur_info = NULL;
    header.addr = 0;
    header.next = NULL;
#endif
}

#ifdef CONFIG_FTRACE
static void append(paddr_t cur, paddr_t des, int type)
{
    STACK_ENTRY *node = malloc(sizeof(STACK_ENTRY));
    end->next = node;
    node->next = NULL;
    node->addr = cur;
    node->cur_info = check_func(cur);
    node->des_info = check_func(des);
    node->type = type;

    end = node;
}
#endif

void stack_call(paddr_t cur, paddr_t des)
{
#ifdef CONFIG_FTRACE
    append(cur, des, FT_CALL);
#endif
}

void stack_return(paddr_t cur, paddr_t des)
{
#ifdef CONFIG_FTRACE
    append(cur, des, FT_RET);
#endif
}

#ifdef CONFIG_FTRACE_COMPLETELY
static char *action_name[] = {"Call", "Ret"};
#endif

void print_stack_trace()
{
#ifdef CONFIG_FTRACE_COMPLETELY
    printf("===== " ASNI_FMT("Call Stack", ASNI_FG_BLUE) " =====\n");
    for (STACK_ENTRY *cur = &header; cur != end; cur = cur->next)
    {
        STACK_ENTRY *r = cur->next;
        printf("<%#x>" ASNI_FMT(" %-12s ", ASNI_FG_BLUE) ASNI_FMT("%s", ASNI_FG_WHITE)
                   ASNI_FMT("\t<%#x> ", ASNI_FG_YELLOW) ASNI_FMT("%-12s \n", ASNI_FG_BLUE),
               r->addr, r->cur_info ? r->cur_info->func_name : "", action_name[r->type],
               r->des_info ? r->des_info->start : 0, r->des_info ? r->des_info->func_name : "");
    }
#endif
}

void error_finfo()
{
#ifdef CONFIG_FTRACE
    vaddr_t pc = cpu.pc;
    FUNC_INFO *info = check_func(pc);
    printf(ASNI_FMT("Some error happended at %s(%#x).\n", ASNI_FG_RED), info->func_name, pc);
#endif
}