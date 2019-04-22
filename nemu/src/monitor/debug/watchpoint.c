#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>
#include <string.h>
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
bool su = true;
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
    if(su == true){
        init_wp_pool();
        su = false;
        //printf("!!!!!!!!!!!!!\n");
    }
    if(free_ == NULL){
        printf("Erro!free is null.\n");
        assert(0);
    }
    WP *new = NULL;
    new = free_;
    free_ = free_->next;
   // printf("!!!!%d\n",value);
   // printf("!!!!%s\n",str);
    new->value = value;
   // printf("!!!!%d\n",new->value);
    strcpy(new->expr, str);
   // printf("!!!!%d\n",new->value);
   // printf("%s /n",new->expr);
    new->next = NULL;
    new->isused = true;
    if(head == NULL) head = new;
    else{
        new->next = head;
        head = new ;
    }
    return new;
}

//根据给出的编号释放对应的监视点
void free_wp(int no){
    WP *p = head;
    if(head == NULL){
        printf("监视点列表为空。 \n");
        assert(0);
    }
    else if(p->NO == no){
        head = head->next;
        p->value = 0;
        p->isused = false;

        p->next = free_;
        free_ = p;
        printf("已经删除第%d个监视点。\n", no);
       // free(p);
        return;
    }
    else{
        WP *q = head;
        p = p ->next;
        while(p!=NULL){
            if (p->NO == no){
                q->next = p->next;
                p->value = 0;
                p->isused = false;
                p->next = free_;
                free_ = p;
                printf("已经删除第%d个监视点。\n", no);
         //       free(p);free(q);
                return;
            }
            else{
                p = p -> next;
                q = q -> next;
        }
    }
    printf("不存在第%d个监视点。\n",no);
    return;
    }
}

void print_wp(){
    WP *p = head;
    if(p ==NULL){
        printf("监视点为空！\n");
        return;
    }
    else{
        while(p!=NULL){

            printf("%d   %s 0x%08x\n",p->NO , p->expr, p->value);
            p=p->next;
        }
        return;
    }
    return;
}
