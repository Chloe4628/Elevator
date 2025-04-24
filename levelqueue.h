// levelqueue.h
// LQ��ʾÿ����ŶӶ��� 
#ifndef LEVELQUEUE_H
#define LEVELQUEUE_H

#include "base.h"

// 1.��ʼ������
Status LQ_Init();
// 2.���
Status LQ_EnQueue(Level_Type n, Elevator_Direction direction, People new_people);
// 3.չʾ���� ��Ҫ�ǲ�����
Status LQ_Display(Level_Type n, Elevator_Direction direction);
// 4.����
Status LQ_DeQueue(Level_Type n, Elevator_Direction direction);
// 5.ɾ��ָ���ڵ�
Status LQ_Del(Level_Type n, Elevator_Direction direction, int num);
// 6.������г���
int LQ_Get_Num(Level_Type n, Elevator_Direction direction);
// 7.�ͷ� 
Status LQ_Destroy();

#endif

