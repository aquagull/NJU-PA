#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      
      case -1:
      ev.event = EVENT_YIELD;
      break;

    case 0 ... 19:
    
      ev.event = EVENT_SYSCALL;
      break;

    case 0x80000007:
    
      ev.event = EVENT_IRQ_TIMER;
      break;

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  //内联汇编代码，常用于在C语言环境下对硬件进行操作
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));  //设置异常

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
