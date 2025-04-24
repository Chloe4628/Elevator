#ifndef FUNC_H
#define FUNC_H

#include "base.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"

// 初始化一切 
Status Open_Up(); 
// 释放一切 
Status Close_Down();
// 产生新人
Status Generate_New_People();
// 下一个事件
EL_Node *Get_Next_Event(EL_Node *current_event);
// 处理事件
Status Process_Event(EL_Node *event);
// 总模拟函数
Status Simulation();
// 是否停靠
Status Is_Stop(Elevator *e);
// 是否转向
Elevator_Direction Direction_Change(Elevator *e);
// LOOK算法
Status Look_Algorithm(Elevator *e);
// 统计信息 
Status Stat();
// 随机数 
int Get_Random(int min, int max);
// 方向比较
Elevator_Direction Judge_Direction(Level_Type start, Level_Type end);

#endif
