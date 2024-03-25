#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(const char *e, bool *success);
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char w_expr[32];
} WP;
WP* new_wp(const char *w_expr, bool *success);
void free_wp(int NO);

void watchpoint_display();
//bool check_watchpoint(WP **point);
#endif
