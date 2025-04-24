// peoplelist.h 
// PL表示电梯内乘客链表
#ifndef PEOPLELIST_H
#define PEOPLELIST_H 

#include "base.h"

// 1.初始化乘客链表 
PL_Node *PL_Init();
// 2.插入节点
Status PL_Insert(PL_Node *head, People new_people);
// 3.展示乘客链表 主要是测试用 
Status PL_Display(PL_Node *head);
// 4.删除节点 
Status PL_Del(PL_Node *head, PL_Node *tar);
// 5.计算电梯内乘客数量
int PL_Get_Num(PL_Node *head); 
// 6.释放 
Status PL_Destroy(PL_Node *head);

#endif

