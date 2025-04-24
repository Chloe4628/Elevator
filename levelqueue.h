// levelqueue.h
// LQ表示每层的排队队列 
#ifndef LEVELQUEUE_H
#define LEVELQUEUE_H

#include "base.h"

// 1.初始化队列
Status LQ_Init();
// 2.入队
Status LQ_EnQueue(Level_Type n, Elevator_Direction direction, People new_people);
// 3.展示队列 主要是测试用
Status LQ_Display(Level_Type n, Elevator_Direction direction);
// 4.出队
Status LQ_DeQueue(Level_Type n, Elevator_Direction direction);
// 5.删除指定节点
Status LQ_Del(Level_Type n, Elevator_Direction direction, int num);
// 6.计算队列长度
int LQ_Get_Num(Level_Type n, Elevator_Direction direction);
// 7.释放 
Status LQ_Destroy();

#endif

