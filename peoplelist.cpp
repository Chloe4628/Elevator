// peoplelist.cpp 
// PL表示电梯内乘客链表
#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "peoplelist.h"

// 1.初始化乘客链表 
PL_Node *PL_Init(){
	PL_Node *head = (PL_Node *)malloc(sizeof(PL_Node));
	head->next = NULL;
	return head;
}
// 2.插入节点
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
// 3.展示乘客链表 主要是测试用 
Status PL_Display(PL_Node *head){
	printf("-----当前电梯内有%d个乘客-----\n", PL_Get_Num(head)); 
	PL_Node *p = head->next;
	while (p){
		printf("电梯里，%d号人想到%dF\n", p->people.pnum, p->people.pend_level);
		p = p->next;
	}
	printf("----------\n");
	return STATUS_OK; 
} 
// 4.删除节点 
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
// 5.计算电梯内乘客数量
int PL_Get_Num(PL_Node *head){
	int cnt = 0;
	PL_Node *p = head;
	while (p->next){
		p = p->next;
		cnt++;
	}
	return cnt;
} 
// 6.释放
Status PL_Destroy(PL_Node *head){
	PL_Node *p = head;
	while (p){
		PL_Node *r = p;
		p = p->next;
		free(r);
	}
	return STATUS_OK;
}

