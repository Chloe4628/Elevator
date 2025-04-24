// eventlist.h
// EL表示事件链表 
#ifndef EVENTLIST_H
#define EVENTLIST_H

#include "base.h"

// 1.初始化事件链表 
EL_Node *EL_Init();
// 2.插入节点
Status EL_Insert(Time_Type time, Event_Type type, int elevator_id, int person_id, Level_Type level);
// 3.计算事件数量
int EL_Get_Num(EL_Node *head); 
// 4.释放
Status EL_Destroy(EL_Node *head);

#endif

