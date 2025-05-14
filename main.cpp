// main.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "func.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"

// ��ʱ�� ��ʾ��ʵʱ�� 
Time_Type current_time = 0; 
// ������
People people[MAX_PEOPLE + 1]; 
// ¥�����
LQ_Queue queues[LEVEL_NUM + 1][2]; 
// �¼�����
EL_Node *event_list = NULL;  
// �ܵȴ�ʱ�� ������ 
Time_Type Total_Wait_Time = 0; 
// �������� ������
int arrived_people_num = 0; 
// �����ȴ����� ������ 
int give_up_people_num = 0;
// ���ɵ����� ûɶ��
int generated_people_num = 0; 
// ����
Elevator e[2]; // 0��ʾ����A 1��ʾ����B һ����̨���� 

int main(int argc, char** argv){
	srand(5);
	Open_Up();
	Simulation();
	Stat();
	Close_Down();
	return 0;
}
