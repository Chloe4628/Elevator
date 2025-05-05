// peoplelist.cpp 
// PL��ʾ�����ڳ˿�����
#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "peoplelist.h"

// 1.��ʼ���˿����� 
PL_Node *PL_Init(){
	PL_Node *head = (PL_Node *)malloc(sizeof(PL_Node));
	head->next = NULL;
	return head;
}
// 2.����ڵ�
Status PL_Insert(PL_Node *head, People new_people){
	PL_Node *new_node = (PL_Node *)malloc(sizeof(PL_Node));
	new_node->people = new_people;
 
	PL_Node *p = head;
	while (p->next != NULL){
		p = p->next;
	}
	new_node->next = p->next;
	p->next = new_node;
	return STATUS_OK;
}
// 3.չʾ�˿����� ��Ҫ�ǲ����� 
Status PL_Display(PL_Node *head){
	printf("-----��ǰ��������%d���˿�-----\n", PL_Get_Num(head)); 
	PL_Node *p = head->next;
	while (p){
		printf("�����%d�����뵽%dF\n", p->people.pnum, p->people.pend_level);
		p = p->next;
	}
	printf("----------\n");
	return STATUS_OK; 
} 
// 4.ɾ���ڵ� 
Status PL_Del(PL_Node *head, PL_Node *tar){
	PL_Node *p = head;
	while (p->next && p->next != tar){
		p = p->next;
	}
	if (p->next == tar){
		p->next = tar->next;
		free(tar);
		return STATUS_OK;
	}
	return STATUS_ERROR;
}
// 5.��������ڳ˿�����
int PL_Get_Num(PL_Node *head){
	int cnt = 0;
	PL_Node *p = head;
	while (p->next){
		p = p->next;
		cnt++;
	}
	return cnt;
} 
// 6.�ͷ�
Status PL_Destroy(PL_Node *head){
	PL_Node *p = head;
	while (p){
		PL_Node *r = p;
		p = p->next;
		free(r);
	}
	return STATUS_OK;
}

