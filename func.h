#ifndef FUNC_H
#define FUNC_H

#include "base.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"

// ��ʼ��һ�� 
Status Open_Up(); 
// �ͷ�һ�� 
Status Close_Down();
// ��������
Status Generate_New_People();
// ��һ���¼�
EL_Node *Get_Next_Event(EL_Node *current_event);
// �����¼�
Status Process_Event(EL_Node *event);
// ��ģ�⺯��
Status Simulation();
// �Ƿ�ͣ��
Status Is_Stop(Elevator *e);
// �Ƿ�ת��
Elevator_Direction Direction_Change(Elevator *e);
// LOOK�㷨
Status Look_Algorithm(Elevator *e);
// ͳ����Ϣ 
Status Stat();
// ����� 
int Get_Random(int min, int max);
// ����Ƚ�
Elevator_Direction Judge_Direction(Level_Type start, Level_Type end);

#endif
