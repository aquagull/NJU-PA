#include <common.h>
#include "syscall.h"
#include "fs.h"

int sys_brk(void *addr)
{
  return 0;
}

#define time_t uint64_t
#define suseconds_t uint64_t

typedef struct
{
  int32_t tv_sec;  /* seconds */
  int32_t tv_usec; /* microseconds */
} timeval;

typedef struct
{
  int tz_minuteswest; /* minutes west of Greenwich */
  int tz_dsttime;     /* type of DST correction */
} timezone;

static AM_TIMER_UPTIME_T uptime;
int gettimeofday(timeval *tv, timezone *tz)
{
  ioe_read(AM_TIMER_UPTIME, &uptime);
  tv->tv_usec = (int32_t)uptime.us;
  tv->tv_sec = (int32_t)uptime.us / 1000000;
  return 0;
}

void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0])
  {

  case SYS_yield:
    yield();
    c->GPRx = 0;
    break;

  case SYS_exit:
    halt(0);
    break;

  case SYS_open:
    c->GPRx = fs_open((const char *)c->GPR2, (int)c->GPR3, (int)c->GPR4);
    // Log("sys_open(%d, %x, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  case SYS_write:
    c->GPRx = fs_write(c->GPR2, (void *)c->GPR3, (size_t)c->GPR4);
    // Log("sys_write(%d, %x, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  case SYS_brk:
    c->GPRx = sys_brk((void *)c->GPR2);
    // Log("sys_brk(%p, %d, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  case SYS_read:
    c->GPRx = fs_read((int)c->GPR2, (void *)c->GPR3, (size_t)c->GPR4);
    // Log("sys_read(%d, %x, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  case SYS_lseek:
    c->GPRx = fs_lseek((int)c->GPR2, (size_t)c->GPR3, (int)c->GPR4);
    // Log("sys_lseek(%d, %d, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  case SYS_close:
    c->GPRx = fs_close((int)c->GPR2);
    // Log("sys_close(%d, %x, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  case SYS_gettimeofday:
    c->GPRx = gettimeofday((timeval *)c->GPR2, (timezone *)c->GPR3);
    // Log("sys_gettimeofday(%p, %p, %d) = %d", c->GPR2, c->GPR3, c->GPR4, c->GPRx);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
