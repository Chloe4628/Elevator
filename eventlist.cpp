// eventlist.cpp
// EL��ʾ�¼����� 
#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "eventlist.h"

// 1.��ʼ���¼����� 
EL_Node *EL_Init(){
	EL_Node *head = (EL_Node *)malloc(sizeof(EL_Node));
	head->next = NULL;
	head->time = 0;
	return head;
}
// 2.����ڵ㣨����ʱ��˳�� 
Status EL_Insert(Time_Type time, Event_Type type, int elevator_id, int person_id, Level_Type level){
	EL_Node *new_node = (EL_Node *)malloc(sizeof(EL_Node));
	new_node->time = time;
	new_node->type = type;
	new_node->elevator_id = elevator_id;
	new_node->person_id = person_id;
	new_node->level = level;
 
	EL_Node *p = event_list;
	while (p->next != NULL && p->next->time <= time){
		p = p->next;
	}
	new_node->next = p->next;
	p->next = new_node;
	return STATUS_OK;
}
// 3.�����¼�����
int EL_Get_Num(EL_Node *head){
	int cnt = 0;
	EL_Node *p = head;
	while (p->next){
		p = p->next;
		cnt++;
	}
	return cnt;
} 
// 4.�ͷ�
Status EL_Destroy(EL_Node *head){
	EL_Node *p = head;
	while (p){
		EL_Node *r = p;
		p = p->next;
		free(r);
	}
	return STATUS_OK;
}
