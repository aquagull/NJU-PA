#include "sdb.h"

#define NR_WP 32


static WP wp_pool[NR_WP] = {};
/* head链表组织使用中的监视点结构
 * free组织空闲监视点结构
 * */
static WP *head = NULL, *free_ = NULL;
static int number = 1;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/*w_expr待监督或者计算表达式*/
WP* new_wp(const char *w_expr ,bool *success){
    if(free_ == NULL)
        assert(0);
    WP* res = free_ ;
    res->NO = number++;
    free_ = res->next;
    res->next = NULL;

    expr(w_expr,success);
    strcpy(res->w_expr,w_expr);
    
    /*把res节点放到head链表头*/
    if(head == NULL)
        head = res;
    else{
        res->next = head->next;
        head->next = res;
    }

    return res;
}

static void Insert_free(WP *wp){
    wp->next = free_ ;
    free_ = wp; 
}
/*归还wp到free链表*/ 
void free_wp(int NO){
    if(head->NO == NO){
        WP* tmp = head->next;
        Insert_free(head);
        head = tmp;
        return;
    }
    WP* prev = head;
    while(prev->next){
        if(prev->next->NO == NO){
            WP* tmp = prev->next->next;
            Insert_free(prev->next);
            prev->next = tmp;
            return;
        }
        prev = prev->next;
    }

    printf("没有找到 \e[1;31mWatchPoint(NO.%d)\e[0m\n",NO);
}

void watchpoint_display(){
    printf("NO.\tw_expr\n");
    WP* tmp = head;
    while(tmp)
    {
        printf("\e[1;36m%d\e[0m\t\e[0;32m%s\e[0m\n",tmp->NO,tmp->w_expr);
        tmp = tmp->next;
    }
}

bool check_watchpoint(WP **point){
    WP* tmp = head;
    bool success = true;
    while(tmp){
        /*检查表达式是否改变*/
    if(expr(tmp->w_expr, &success)){
        *point = tmp;
        return true;
    }
        tmp = tmp->next; 
    }
    return false;
}
