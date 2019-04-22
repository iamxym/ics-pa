#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
      int NO;
      struct watchpoint *next;
      uint32_t value; //最近一次的数值；
      char expr[65536]; //记录监视的表达式。
      int type; //类型，断点和监视点；
      bool isused; //监视点是否使用。
      /* TODO: Add more members if necessary */


                
} WP;


//清空某监视点的下一项指针
void clearWP(WP *wp);

//获取 head 
WP *getHeadWP();

//申请使用一个新监视点（内部调用 new_wp() 并更新链表信息）
WP *createWP();

//删除指定编号的监视点
void removeWP(int no);


//（私有）从内存池中获取下一个能使用的监视点，并作一定预处理
WP *new_wp();

//（私有）释放一个监视点
void free_wp(int no);

// 打印监视点
void print_wp();

#endif
