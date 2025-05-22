// main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "func.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"

// 总时长
Time_Type MAX_TIME = 1000;
// 总人数 
int MAX_PEOPLE = 5;
// 总时钟 表示真实时间 
Time_Type current_time = 0; 
// 人数组
People people[16]; 
// 楼层队列
LQ_Queue queues[LEVEL_NUM + 1][2]; 
// 事件链表
EL_Node *event_list = NULL;  
// 总等待时间 计数用 
Time_Type Total_Wait_Time = 0; 
// 到达人数 计数用
int arrived_people_num = 0; 
// 放弃等待人数 计数用 
int give_up_people_num = 0;
// 生成的人数 没啥用
int generated_people_num = 0; 
// 电梯
Elevator e[2];

int main(int argc, char** argv){
	srand(time(NULL));
	Open_Up();
	Simulation();
	Stat();
	Close_Down();
	return 0;
}
