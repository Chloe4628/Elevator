// eventlist.h
// EL��ʾ�¼����� 
#ifndef EVENTLIST_H
#define EVENTLIST_H

#include "base.h"

// 1.��ʼ���¼����� 
EL_Node *EL_Init();
// 2.����ڵ�
Status EL_Insert(Time_Type time, Event_Type type, int elevator_id, int person_id, Level_Type level);
// 3.�����¼�����
int EL_Get_Num(EL_Node *head); 
// 4.�ͷ�
Status EL_Destroy(EL_Node *head);

#endif

