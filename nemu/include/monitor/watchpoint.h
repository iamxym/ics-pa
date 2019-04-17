#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  uint32_t value; //最近一次的数值；
  char *expr; //记录监视的表达式。
  int type; //类型，断点和监视点；
  bool isused; //监视点是否使用。
  /* TODO: Add more members if necessary */


} WP;

#endif
