// func.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "func.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"

// ��ʼ��һ�� 
Status Open_Up(){
	// ��ʼ������
    for (int i = 0; i < 2; i++) {
        e[i].ecur_people_num = 0;
        e[i].reserved_people_num = 0;
        e[i].direction = STILL;
        e[i].peoplelist = PL_Init();
        e[i].status = IDLE;
        e[i].last_action_time = 0;
        e[i].idle_start_time = 0; 
        e[i].id = i;
        // ���ͣ����
        for (int j = 0; j <= LEVEL_NUM; j++) {
            e[i].levellist[j] = NO;
        }
        // ���ó�ʼ¥��
        if (i == 0){
            e[i].ecur_level = 1;
        } else if (i == 1){
            e[i].ecur_level = LEVEL_NUM;
        }
    }
    // ��ʼ������ 
	LQ_Init();
	// ��ʼ���¼����� 
	event_list = EL_Init();
	return OK;
}
// �ͷ�һ�� 
Status Close_Down(){
	for (int i = 0; i < 2; i++){
		PL_Destroy(e[i].peoplelist);
	}
	EL_Destroy(event_list);
	LQ_Destroy();
	return OK;
}
// ��������
Status Generate_New_People(){
	if (generated_people_num >= MAX_PEOPLE){
		printf("-----������%d��-----\n", generated_people_num);  
		return OVERFLOW;
	}
	generated_people_num++;
	people[generated_people_num].pnum = generated_people_num;
	people[generated_people_num].itv_time = Get_Random(ITV_MIN, ITV_MAX);
	people[generated_people_num].arr_time = current_time + people[generated_people_num].itv_time;
	people[generated_people_num].max_wait = Get_Random(WAIT_MIN, WAIT_MAX); 
	
	people[generated_people_num].pstart_level = Get_Random(1, LEVEL_NUM);
	do {
		people[generated_people_num].pend_level = Get_Random(1, LEVEL_NUM);
	} while (people[generated_people_num].pstart_level == people[generated_people_num].pend_level);
	
	people[generated_people_num].status = WAITING;
	
	EL_Insert(people[generated_people_num].arr_time, PEOPLE_ARRIVAL, -1, generated_people_num, people[generated_people_num].pstart_level); // �˵�������¼����� 
	
	return OK; 
}
// ��һ���¼�
EL_Node *Get_Next_Event(EL_Node *current_event){
	if (current_event == NULL){
		return NULL;
	} 
	EL_Node *p = current_event->next; 
	
	return p;
} 
// �����¼�
Status Process_Event(EL_Node *event){
	current_time = event->time;
	Elevator *ce = NULL;
	People *cp = NULL;
	if (event->elevator_id != -1){
		ce = &e[event->elevator_id];
	}
	if (event->person_id != -1){
		cp = &people[event->person_id];
	}

	switch (event->type){
		case PEOPLE_ARRIVAL:{
			if (!cp){
				printf("�������쳣1������\n");
				exit(1);
			}
			if (cp->status != WAITING){
				printf("�������쳣2������\n");
				exit(2); 
			}
			printf("%dtʱ��%d���˵���%dF�ȴ������ݣ���ȥ%dF\n", cp->arr_time, cp->pnum, cp->pstart_level, cp->pend_level);
			LQ_EnQueue(cp->pstart_level, Judge_Direction(cp->pstart_level, cp->pend_level), *cp);
			
			// ��ǰ�������¼������¼����� 
			EL_Insert(cp->arr_time + cp->max_wait, PEOPLE_GIVE_UP, -1, cp->pnum, cp->pstart_level);
			
			// ���е���
			int suitable = -1;
			Elevator_Direction required_direction = Judge_Direction(cp->pstart_level, cp->pend_level); // �˵�Ҫ���� 
			Elevator_Direction d0 = Judge_Direction(e[0].ecur_level, cp->pstart_level); // ����0���˵ķ��� 
			Elevator_Direction d1 = Judge_Direction(e[1].ecur_level, cp->pstart_level); // ����1���˵ķ���
			 
			if ((e[0].status == IDLE || e[0].status == PARKING) && (e[1].status == IDLE || e[1].status == PARKING) 
			&& (e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY)){
				suitable = Get_Random(0, 1);
			} else if ((e[0].status == IDLE || e[0].status == PARKING) && e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY){
				suitable = 0;
			} else if ((e[1].status == IDLE || e[1].status == PARKING) && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY){
				suitable = 1;
			} else { // ������IDLE����PARKING״̬
				if (((e[0].direction == STILL || e[0].direction == required_direction) && (d0 == required_direction || d0 == STILL))
				&& ((e[1].direction == STILL || e[1].direction == required_direction) && (d1 == required_direction || d1 == STILL))
				&& (e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY)
				&& (e[0].ecur_level != cp->pstart_level || e[0].status != RUNNING)
				&& (e[1].ecur_level != cp->pstart_level || e[1].status != RUNNING)){
					suitable = Get_Random(0, 1);
				} else if ((e[0].direction == STILL || e[0].direction == required_direction) && (d0 == required_direction || d0 == STILL)
				&& e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY
				&& (e[0].ecur_level != cp->pstart_level || e[0].status != RUNNING)){
					suitable = 0;
				} else if ((e[1].direction == STILL || e[1].direction == required_direction) && (d1 == required_direction || d1 == STILL)
				&& e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY
				&& (e[1].ecur_level != cp->pstart_level || e[1].status != RUNNING)){
					suitable = 1;
				}
			}
			
			if (suitable != -1){
				ce = &e[suitable];
				if (ce->ecur_level != cp->pstart_level){ // ���ڵ��ݺ��˲���ͬһ�� ��� 
				
					ce->levellist[cp->pstart_level] = YES;
					if (ce->status == IDLE || ce->status == PARKING){
						ce->status = RUNNING;
						printf("%dtʱ�����е���%d�յ�%dF��%d������������\n", current_time, suitable, cp->pstart_level, cp->pnum);
						
						ce->direction = Judge_Direction(ce->ecur_level, cp->pstart_level);
						Look_Algorithm(ce);
					} else {
						printf("%dtʱ����������%d�յ�%dF��%d�������󣬽�����Ӧ\n", current_time, suitable, cp->pstart_level, cp->pnum);
					}
					// ����IDLE�������ǰ״̬ 
				} else { // ���ڵ��ݺ�����ͬһ�� 
					printf("%dtʱ������%d��%dF����%d����\n", current_time, suitable, cp->pstart_level, cp->pnum);
					
					switch (ce->status){
						case (PARKING):
						case (IDLE):
						case (CLOSING):{
							printf("%dtʱ������%d��%dF����%d���ˣ�׼������\n", current_time, suitable, cp->pstart_level, cp->pnum);
							ce->status = OPENING; 
							ce->last_action_time = current_time;
							
							// ��ǰ���ſ����¼������¼�����
							EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, suitable, -1, cp->pstart_level);							
							break;
						}
						case (OPEN):
						case (SERVING):{
							// �����ϴ�����һֱ���� ��Ȼsuitable = -1 
							if ((ce->direction == required_direction || ce->ecur_people_num + ce->reserved_people_num == 0) && ce->ecur_people_num + ce->reserved_people_num < MAX_CAPACITY){
								if (ce->ecur_people_num + ce->reserved_people_num == 0){
									ce->direction = required_direction;
								}
								// ��Ҫ�ر��� ��Ϊ�˴�û����ʽ���ſ����¼����˽��� 
								printf("%dtʱ������%d��%dF����%d���ˣ��պÿ�����\n", current_time, suitable, cp->pstart_level, cp->pnum);
								ce->status = SERVING;
								ce->reserved_people_num++;
								
								// �˿�ʼ����ʱ�Ͱ�״̬��ΪIN_ELEVATOR�������ʱ���������¼� 
								cp->status = IN_ELEVATOR; 
								
								// �����˽�����ר�ü�ʱ��
								Time_Type io_time = current_time;
								// �ҵ����һ�������¼��ڵ� 
								EL_Node *p = event_list;
								while (p){
									if ((p->type == IN || p->type == OUT) && p->elevator_id == ce->id && p->time > io_time){
										io_time = p->time;	
									}
									p = p->next;
								} 
								
								// ��ǰ���˽����¼������¼�����
								EL_Insert(io_time + PEOPLE_IN_OUT_TIME, IN, ce->id, cp->pnum, ce->ecur_level);
								// �Ѵ��˴Ӷ�����ɾ�� 
								if (LQ_DeQueue(ce->ecur_level, required_direction) == ERROR){
									printf("�������쳣11������\n");
									exit(11);
								}
								
								// ��ǰ���˽����ļ���¼������¼����� 
								EL_Insert(io_time + PEOPLE_IN_OUT_TIME + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
							} else {
								printf("�������쳣10������\n");
								exit(10);
							}
							break;
						}
						case (OPENING):{
							// û��ʲô��Ҫ����� ���Ѿ�������� ���ſ����¼����˲Ž��� 
							break;
						}
					}	
				} 
			}
			// û�к��ʵ��� ������ȴ� �����ڶ��� ������LOOK�㷨���жϷ�����ж��Ƿ�ͣ������
			// �˵����¼���� ������һ�� 
			Generate_New_People();
			break;
		}
		case ELEVATOR_ARRIVAL:{
			if (!ce){
				printf("�������쳣4������\n");
				exit(4);
			}
			if (ce->status != RUNNING && ce->status != PARKING){
				break;
			}
			ce->ecur_level = event->level;
			printf("%dtʱ������%d����%dF\n", current_time, ce->id, ce->ecur_level);
			
			ce->last_action_time = current_time;
			
			Is_Stop(ce);
			break;
		}
		case DOOR_OPENED:{
			if (!ce){
				printf("�������쳣5������\n");
				exit(5);
			}
			printf("%dtʱ������%d��%dF���ţ��ؿͣ�%d/%d��\n", current_time, ce->id, ce->ecur_level, ce->ecur_people_num, MAX_CAPACITY);
			ce->status = OPEN;
			ce->last_action_time = current_time;			
			ce->idle_start_time = current_time;
			
			PL_Node *ptr = ce->peoplelist->next;
			LQ_Node *qtr = queues[ce->ecur_level][ce->direction].front->next;
			Elevator_Direction flag = STILL;
			ce->reserved_people_num = 0;
			
			// �����˽�����ר�ü�ʱ��
			Time_Type io_time = current_time; 
			// ���ں�������������ĵı�� 
			int active = 0;
			
			// �µ��� 
			while (ptr){ 
				if (ptr->people.pend_level == ce->ecur_level){
					active = 1;
					// ��ǰ���˳�ȥ�¼������¼�����
					EL_Insert(io_time + PEOPLE_IN_OUT_TIME, OUT, ce->id, ptr->people.pnum, ce->ecur_level);
					io_time += PEOPLE_IN_OUT_TIME;
					
					PL_Node *tar = ptr;
					ptr = ptr->next;
					// �Ѵ��˴ӳ˿�����ɾ�� 
					if (PL_Del(ce->peoplelist, tar) == ERROR){
						printf("�������쳣6������\n");
						exit(6);
					}
				} else {
					ptr = ptr->next;
				}
			}
			
			// �ϵ���
			// ����ֱ�Ӹ��·��� ���flag����STILL˵��û��Ҫ����
			if (ce->direction == UP || (ce->direction == STILL && ce->id == 0)){
//				puts("fff");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (LQ_Get_Num(ce->ecur_level, UP) != 0){
//					puts("ddd");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					flag = UP;
					ce->direction = UP;
				} else if (LQ_Get_Num(ce->ecur_level, DOWN) != 0){
//					puts("hhh");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					flag = DOWN;
					ce->direction = DOWN;
				}
			}
			if (ce->direction == DOWN || (ce->direction == STILL && ce->id == 1)){ 
//				puts("ggg");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (LQ_Get_Num(ce->ecur_level, UP) != 0){
//					puts("eee");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					flag = UP;
					ce->direction = UP;
				} else if (LQ_Get_Num(ce->ecur_level, DOWN) != 0){
//					puts("iii");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					flag = DOWN;
					ce->direction = DOWN;
				}
			}
			
			// ������ܸı� ����qtrָ��
			if (flag != STILL){
				qtr = queues[ce->ecur_level][flag].front->next;
			}

			while (flag != STILL && qtr && ce->ecur_people_num + ce->reserved_people_num < MAX_CAPACITY){
//				puts("jjj");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (io_time - qtr->people.arr_time <= qtr->people.max_wait){
//					puts("kkk");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					active = 1;
					// ��ǰ���˽����¼������¼�����
					EL_Insert(io_time + PEOPLE_IN_OUT_TIME, IN, ce->id, qtr->people.pnum, ce->ecur_level);
					// ��ǰ����״̬��ΪIN_ELEVATOR�������ʱ���������¼�
					qtr->people.status = IN_ELEVATOR;
					 
					io_time += PEOPLE_IN_OUT_TIME;
					ce->reserved_people_num++;
					
					LQ_Node *help = qtr;
					qtr = qtr->next;
					// �Ѵ��˴Ӷ�����ɾ�� 
					if (LQ_Del(ce->ecur_level, ce->direction, help->people.pnum) == ERROR){
						printf("�������쳣7������\n");
						exit(7);
					}
				} else {
//					puts("!!!1"); // ���˷����Ż����� ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					qtr = qtr->next;			
				}
			}
			
			if (active){
				ce->status = SERVING;
			} else {
				// û�н���� Ҳ�������˷����� 
				printf("55555555555û�н���������������˵Ȳ��������˻���������������\n");
			}
			// ��ǰ������Զ������¼������¼�����
			EL_Insert(io_time + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
			break;
		}
		case DOOR_CHECK:{
			if (!ce){
				printf("�������쳣8������\n");
				exit(8);
			}
			if (ce->status != OPEN && ce->status != SERVING){ 
				break; 
			}
			
//			puts("lll");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// �����ϲ������˻�û������ 
			PL_Node *ptr = ce->peoplelist->next;
			while (ptr){
				if (ptr->people.pend_level == ce->ecur_level){
					printf("�������쳣9������\n");
					exit(9);
				}
				ptr = ptr->next;
			}
					
			if (ce->reserved_people_num == 0){
				ce->status = CLOSING;
				ce->last_action_time = current_time;
				printf("%dtʱ������%d��%dF��ʼ����\n", current_time, ce->id, ce->ecur_level);
				
				// ��ǰ�������Źر��¼������¼����� 
				EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_CLOSED, ce->id, -1, ce->ecur_level);
			} 
			// ���������û������ ��PEOPLE_ARRIVAL�����DOOR_CHECK��� 
			break;
		}
		case DOOR_CLOSED:{
			if (!ce){
				printf("�������쳣13������\n");
				exit(13);
			}
			printf("%dtʱ������%d��%dF�Ѿ�����\n", current_time, ce->id, ce->ecur_level);
			
			if (ce->ecur_people_num != 0){
				ce->status = RUNNING;
				// �����������Ѿ��ڿ���ʱ���� 
				Look_Algorithm(ce);
			} else { // ���ݿ��� 
				ce->status = IDLE;
				ce->idle_start_time = current_time;
				printf("%dtʱ������%d��ʱ������ͣ����%dF\n", current_time, ce->id, ce->ecur_level);
				
				// ��ǰ�����Ƿ�Ҫ��ȥ�������
				EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, ce->id, -1, ce->ecur_level); 
			} 
			break;
		}
		case IN:{
			if (!ce || !cp){
				printf("�������쳣14������\n");
				exit(14);
			}
			printf("%dtʱ��%d������%dF�����˵���%d\n", current_time, cp->pnum, ce->ecur_level, ce->id);
			ce->ecur_people_num++;
			ce->reserved_people_num--;
			ce->last_action_time = current_time;
			
			cp->status = IN_ELEVATOR;
			PL_Insert(ce->peoplelist, *cp);
			
			ce->levellist[cp->pend_level] = YES;
			
//			if (ce->reserved_people_num == 0){
//				EL_Insert(current_time + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
//			}
			
			break;
		}
		case OUT:{
			if (!ce || !cp){
				printf("�������쳣15������\n");
				exit(15);
			}
			if (cp->status == GIVE_UP){
				printf("�������쳣16������\n");
				exit(16);
			}

			printf("%dtʱ��%d������%dF�뿪�˵���%d���ɹ��ִ����\n", current_time, cp->pnum, ce->ecur_level, ce->id);
			ce->ecur_people_num--;
			ce->last_action_time = current_time; 
			
			cp->status = ARRIVED;
			
			arrived_people_num++;
			break;
		}
		case IDLE_CHECK:{
			if (!ce){
				printf("�������쳣17������\n");
				exit(17);
			}
			Level_Type tl = ce->id == 0? 1: LEVEL_NUM;
			
			// ����ϴλ�¼��Ϳ���ʱ�� 
			if (ce->status == IDLE || current_time - ce->last_action_time >= AUTO_ORDER_TIME){
				if (ce->ecur_level != tl){
					printf("%dtʱ������%d�Ѿ����г���%dt������%dF����\n", current_time, ce->id, AUTO_ORDER_TIME, tl);
					
					ce->status = PARKING;
					ce->direction = Judge_Direction(ce->ecur_level, tl);
					Look_Algorithm(ce);
				} 
			} else if (ce->status == IDLE){ // ��ʵ�����ϲ�Ӧ�÷��� �����ϱ���
				// ��ǰ�����Ƿ�Ҫ��ȥ�������
				Time_Type rt = AUTO_ORDER_TIME - (current_time - ce->last_action_time); 
				EL_Insert(current_time + rt, IDLE_CHECK, ce->id, -1, ce->ecur_level); 
			}
			// ����IDLE�����Ч ֱ�ӷ��� 
			break;
		}
		case PEOPLE_GIVE_UP:{
			if (!cp){
				printf("�������쳣18������\n");
				exit(18);
			} 
			if (cp->status == IN_ELEVATOR){
				break;
			}
			
			if (cp->status == WAITING){
				printf("%dtʱ��%d����û�����ˣ��뿪����555555555\n", current_time, cp->pnum);
				cp->status = GIVE_UP;
				give_up_people_num++;
				
				// ���˴Ӷ�����ɾ�� ������LQ_DeQueueҪһ������ ��ʵ������LQ_Del 
				Elevator_Direction dp = Judge_Direction(cp->pstart_level, cp->pend_level);
				LQ_Node *p = queues[cp->pstart_level][dp].front; 
				while (p->next){
					if (p->next->people.pnum == cp->pnum){
						LQ_Node *q = p->next; 
						p->next = p->next->next;
						free(q);
						break;
					}
					p = p->next;
				}
			}
			break;
		}
		default:{
			printf("�������쳣19������\n");
			exit(19);
			break;
		}
			
	}
	return OK;
} 
// ��ģ�⺯��
Status Simulation(){
	EL_Node *current_event = event_list;
	printf("-----ģ�⿪ʼ�����%dt-----\n", MAX_SIMULATION_TIME);
	Generate_New_People();
	while ((current_event = Get_Next_Event(current_event)) != NULL){
		if (current_event->time > MAX_SIMULATION_TIME){
			break;
		}
		Process_Event(current_event);
		
		// �ϱ��� 
		if (current_time >= MAX_SIMULATION_TIME * 2){
			printf("�������쳣3������\n");
			exit(3);
		}
	} 
	printf("-----ģ�����-----\n");
	return OK;
}
// �Ƿ�ͣ��
Status Is_Stop(Elevator *e){
	Status stop = FALSE;
	if (e->levellist[e->ecur_level] == YES || LQ_Get_Num(e->ecur_level, e->direction) != 0){
		stop = TRUE;
		printf("%dtʱ������%dͣ��%dF\n", current_time, e->id, e->ecur_level); 
		e->status = OPENING;
		
		// ��ǰ�������¼������¼����� 
		EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, e->id, -1, e->ecur_level);
		e->levellist[e->ecur_level] = NO;
	} else {
		Look_Algorithm(e); 
	}
	return OK;
}
// �Ƿ�ת��
Elevator_Direction Direction_Change(Elevator *e){
	Elevator_Direction d = e->direction;
	// ��ǰ������������ת�� �����෴������������ת�� ����������STILL 
	if (d == UP){
		for (int i = e->ecur_level; i <= LEVEL_NUM; i++){
			if (e->levellist[i] == YES){
				return UP;
			}
			if (LQ_Get_Num(i, UP) != 0 || LQ_Get_Num(i, UP) != 0){
				return UP;
			}
		}
		for (int i = e->ecur_level; i >= 1; i--){
			if (e->levellist[i] == YES){
				return DOWN;
			}
			if (LQ_Get_Num(i, DOWN) != 0 || LQ_Get_Num(i, DOWN) != 0){
				return DOWN;
			}
		}
	} else if (d == DOWN){
		for (int i = e->ecur_level; i >= 1; i--){
			if (e->levellist[i] == NO){
				return DOWN;
			}
			if (LQ_Get_Num(i, DOWN) != 0 || LQ_Get_Num(i, DOWN) != 0){
				return DOWN;
			}
		}
		for (int i = e->ecur_level; i <= LEVEL_NUM; i++){
			if (e->levellist[i] == YES){
				return UP;
			}
			if (LQ_Get_Num(i, UP) != 0 || LQ_Get_Num(i, UP) != 0){
				return UP;
			}			
		}
	}
	return STILL;
} 
// LOOK�㷨
Status Look_Algorithm(Elevator *e){ 
	Elevator_Direction d = Direction_Change(e);
	
	// �÷��������� ������������ ת�� 
	// PARKING����
	if (e->status != PARKING && d != STILL && d != e->direction){
		printf("%dtʱ������%d��%dFת��\n", current_time, e->id, e->ecur_level);
		e->direction = d;
		
		// �жϸò㷴�����Ƿ���Ҫͣ�� 
		Is_Stop(e);
		// �������Ҫͣ����e->status��RUNNING 
		if (e->status == RUNNING){
			Look_Algorithm(e);  
		}
		// �����Ҫͣ�� e->status����OPENING������һϵ���¼� ֱ���Ź��ϼ�������LOOK�㷨 �������ȷ��� 
		return OK; 
	}
	// ����ת�� ǰ�������� ����PARKING״̬ 
	if ((e->status == RUNNING && d != STILL && d == e->direction) || e->status == PARKING){
		Level_Type next_level = e->ecur_level;
		if (e->direction == UP && e->ecur_level < LEVEL_NUM){ // ���� 
//			puts("aaa");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			next_level++;
		} else if (e->direction == DOWN && e->ecur_level > 1){ // ���� 
//			puts("bbb");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			next_level--;
		} else { // ������IDLE������⴦�� �����ﲻ�ᷢ�� ����Ϊ�˱���
//			puts("ccc"); ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			e->status = IDLE;
			e->direction = STILL;
			e->idle_start_time = current_time;
			
			// ��ǰ�����Ƿ�Ҫ��ȥ�������
			EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, e->id, -1, e->ecur_level);
			return OK; 
		}
		e->last_action_time = current_time;
		// ��ǰ���뵽���¼� 
		EL_Insert(current_time + MOVE_LEVEL_TIME, ELEVATOR_ARRIVAL, e->id, -1, next_level);
	}
	return OK;
}
// ͳ����Ϣ 
Status Stat(){
	printf("-----���ͳ��-----\n");
	
	printf("�ܼ�����������%d\n", generated_people_num);
	printf("�ܼƵ���������%d\n", arrived_people_num);
	printf("�ܼƷ���������%d\n", give_up_people_num);
	printf("%d�˻��ڵȴ�����\n", generated_people_num - arrived_people_num - give_up_people_num);
	
	return OK;
} 
// ����� 
int Get_Random(int min, int max){
    return (rand() % (max - min + 1)) + min;
}
// ����Ƚ� 
Elevator_Direction Judge_Direction(Level_Type start, Level_Type end){
	Elevator_Direction ret = STILL;
	if (start > end){
		ret = DOWN;
	} else if (start < end){
		ret = UP;
	}
	return ret;
} 
// ����Ƚ�
Level_Type Distance(Level_Type a, Level_Type b){
	return abs(a - b);
} 

