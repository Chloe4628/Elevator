// levelqueue.cpp
// ǰ׺LQ ��ʾÿ����ŶӶ��� 
#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "levelqueue.h" 

// 1.��ʼ������
Status LQ_Init(){
	for (int i = 1; i <= LEVEL_NUM; i++){
		queues[i][UP].front = (LQ_Node *)malloc(sizeof(LQ_Node));
		queues[i][UP].front->next = NULL;
		queues[i][UP].rear = queues[i][UP].front;
		queues[i][DOWN].front = (LQ_Node *)malloc(sizeof(LQ_Node));
		queues[i][DOWN].front->next = NULL;
		queues[i][DOWN].rear = queues[i][DOWN].front;		
	}
	return STATUS_OK;
}
// 2.���
Status LQ_EnQueue(Level_Type n, Elevator_Direction direction, People new_people){
	LQ_Node *new_node = (LQ_Node *)malloc(sizeof(LQ_Node));
	new_node->people = new_people;
	new_node->next = NULL;
	queues[n][direction].rear->next = new_node;
	queues[n][direction].rear = new_node;
	return STATUS_OK;
}
// 3.չʾ���� ��Ҫ�ǲ�����
Status LQ_Display(Level_Type n, Elevator_Direction direction){  
	if (direction == UP){
		printf("-----��ǰ%dF���϶���-----\n", n);
	} else if (direction == DOWN) {
		printf("-----��ǰ%dF���¶���-----\n", n);
	}
	LQ_Node *p = queues[n][direction].front->next;
	while (p){
		printf("%d�����뵽%dF\n", p->people.pnum, p->people.pend_level);
		p = p->next;
	}
	printf("----------\n");
	return STATUS_OK;
} 
// 4.����
Status LQ_DeQueue(Level_Type n, Elevator_Direction direction){
	LQ_Node *p = queues[n][direction].front->next;
	if (p){
		queues[n][direction].front->next = p->next;
		if (p->next == NULL){
			queues[n][direction].rear = queues[n][direction].front;
		}
		free(p);
		return STATUS_OK;
	}
	return STATUS_ERROR;
}
// 5.ɾ��ָ���ڵ�
Status LQ_Del(Level_Type n, Elevator_Direction direction, int num){
	LQ_Node *p = queues[n][direction].front;
	while (p->next){
		if (p->next->people.pnum == num){
			LQ_Node *q = p->next;
			p->next = q->next;
			
			if (q == queues[n][direction].rear){
				queues[n][direction].rear = p;
			}
			free(q);
			return STATUS_OK;
		}
		p = p->next;
	}
	return STATUS_ERROR;
}
// 6.������г���
int LQ_Get_Num(Level_Type n, Elevator_Direction direction){
	int cnt = 0;
	LQ_Node *p = queues[n][direction].front;
	while (p->next){
		p = p->next;
		cnt++;
	}
	return cnt;
}
// 7.�ͷ� 
Status LQ_Destroy(){
	for (int i = 1; i <= LEVEL_NUM; i++){
		LQ_Node *p = queues[i][UP].front;
		while (p){
			LQ_Node *r = p;
			p = p->next;
			free(r);
		}
		p = queues[i][DOWN].front;
		while (p){
			LQ_Node *r = p;
			p = p->next;
			free(r);
		}
	}
	return STATUS_OK;
}
