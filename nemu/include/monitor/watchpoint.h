#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[128];
  uint32_t expr_val;
  /* TODO: Add more members if necessary */


} WP;

void new_wp(char *args);
void free_wp(char *args);
bool check_expr_val();
void print_watchpoint();
#endif
