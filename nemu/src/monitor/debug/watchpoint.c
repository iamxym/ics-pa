#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>
#include <string.h>
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

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
void free_wp(WP *wp);

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
//需要存入表达式和结果。
WP *new_wp(char *str , int value){
    if(free_ == NULL){
        printf("Erro!free is null.\n");
        assert(0);
    }
    WP *new = NULL;
    new = free_;
    free_ = free_->next;
    new->value = value;
    strcpy(new->expr, str);
    new->next = NULL;
    new->isused = true;
    if(head == NULL) head = new;
    else{
        new->next = head;
        head = new ;
    }
    return new;
}




