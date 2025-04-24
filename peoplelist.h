// peoplelist.h 
// PL��ʾ�����ڳ˿�����
#ifndef PEOPLELIST_H
#define PEOPLELIST_H 

#include "base.h"

// 1.��ʼ���˿����� 
PL_Node *PL_Init();
// 2.����ڵ�
Status PL_Insert(PL_Node *head, People new_people);
// 3.չʾ�˿����� ��Ҫ�ǲ����� 
Status PL_Display(PL_Node *head);
// 4.ɾ���ڵ� 
Status PL_Del(PL_Node *head, PL_Node *tar);
// 5.��������ڳ˿�����
int PL_Get_Num(PL_Node *head); 
// 6.�ͷ� 
Status PL_Destroy(PL_Node *head);

#endif

