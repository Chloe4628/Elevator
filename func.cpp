// func.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#include "base.h"
#include "func.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"
#include "animation.h"

// 初始化一切 
Status Open_Up(){
	// 初始化电梯
    for (int i = 0; i < 2; i++) {
        e[i].ecur_people_num = 0;
        e[i].reserved_people_num = 0;
        e[i].direction = STILL;
        e[i].peoplelist = PL_Init();
        e[i].status = IDLE;
        e[i].last_action_time = 0;
        e[i].idle_start_time = 0; 
        e[i].id = i;
        // 清空停靠表
        for (int j = 0; j <= LEVEL_NUM; j++) {
            e[i].levellist[j] = NO;
        }
        // 设置初始楼层
        if (i == 0){
            e[i].ecur_level = 1;
        } else if (i == 1){
            e[i].ecur_level = LEVEL_NUM;
        }
    }
    // 初始化队列 
	LQ_Init();
	// 初始化事件链表 
	event_list = EL_Init();
	Ani_Show_Title();
	Ani_Init();

	return STATUS_OK;
}
// 释放一切 
Status Close_Down(){
	for (int i = 0; i < 2; i++){
		PL_Destroy(e[i].peoplelist);
	}
	EL_Destroy(event_list);
	LQ_Destroy();
	Ani_Clean_Up();
	return STATUS_OK;
}
// 产生新人
Status Generate_New_People(){
	if (generated_people_num >= MAX_PEOPLE){ 
		return STATUS_OVERFLOW;
	}
	generated_people_num++;
	people[generated_people_num].pnum = generated_people_num;
	people[generated_people_num].itv_time = Get_Random(ITV_MIN, ITV_MAX);
	people[generated_people_num].arr_time = current_time + people[generated_people_num].itv_time;
	people[generated_people_num].max_wait = Get_Random(WAIT_MIN, WAIT_MAX);
	people[generated_people_num].elenum = -1;
	
	people[generated_people_num].pstart_level = Get_Random(1, LEVEL_NUM);
	do {
		people[generated_people_num].pend_level = Get_Random(1, LEVEL_NUM);
	} while (people[generated_people_num].pstart_level == people[generated_people_num].pend_level);
	
	people[generated_people_num].status = WAITING;
	
	EL_Insert(people[generated_people_num].arr_time, PEOPLE_ARRIVAL, -1, generated_people_num, people[generated_people_num].pstart_level); // 人到达加入事件链表 
	
	return STATUS_OK; 
}
// 下一个事件
EL_Node *Get_Next_Event(EL_Node *current_event){
	if (current_event == NULL){
		return NULL;
	} 
	EL_Node *p = current_event->next; 
	
	return p;
} 
// 处理事件
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
	Ani_Update_Time(current_time);

	switch (event->type){
		case PEOPLE_ARRIVAL:{
			if (!cp){
				exit(1);
			}
			if (cp->status != WAITING){
				exit(2); 
			}
			
			char msg[100];
			snprintf(msg, sizeof(msg), "%dt，%d号人到达%dF等待进电梯，想去%dF\n", current_time, cp->pnum, cp->pstart_level, cp->pend_level);
			Ani_Update_General_Person_Status(msg);

			// 提前将放弃事件加入事件链表 
			EL_Insert(cp->arr_time + cp->max_wait, PEOPLE_GIVE_UP, -1, cp->pnum, cp->pstart_level);
			
			// 呼叫电梯
			int suitable = -1;
			Elevator_Direction required_direction = Judge_Direction(cp->pstart_level, cp->pend_level); // 人的要求方向 
			Elevator_Direction d0 = Judge_Direction(e[0].ecur_level, cp->pstart_level); // 电梯0到人的方向 
			Elevator_Direction d1 = Judge_Direction(e[1].ecur_level, cp->pstart_level); // 电梯1到人的方向
			 
			if ((e[0].status == IDLE || e[0].status == PARKING) && (e[1].status == IDLE || e[1].status == PARKING) 
			&& (e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY)){
				if (Distance(e[0].ecur_level, cp->pstart_level) < Distance(e[1].ecur_level, cp->pstart_level)){
					suitable = 0;
				} else if (Distance(e[0].ecur_level, cp->pstart_level) > Distance(e[1].ecur_level, cp->pstart_level)){
					suitable = 1;
				} else {
					suitable = Get_Random(0, 1);
				}
			} else if ((e[0].status == IDLE || e[0].status == PARKING) && e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY){
				suitable = 0;
			} else if ((e[1].status == IDLE || e[1].status == PARKING) && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY){
				suitable = 1;
			} else { // 都不是IDLE或者PARKING状态
				if (((e[0].direction == STILL || e[0].direction == required_direction) && (d0 == required_direction || d0 == STILL))
				&& ((e[1].direction == STILL || e[1].direction == required_direction) && (d1 == required_direction || d1 == STILL))
				&& (e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY)
				&& (e[0].ecur_level != cp->pstart_level || e[0].status != RUNNING)
				&& (e[1].ecur_level != cp->pstart_level || e[1].status != RUNNING)){
					if (Distance(e[0].ecur_level, cp->pstart_level) < Distance(e[1].ecur_level, cp->pstart_level)){
						suitable = 0;
					} else if (Distance(e[0].ecur_level, cp->pstart_level) > Distance(e[1].ecur_level, cp->pstart_level)){
						suitable = 1;
					} else {
						suitable = Get_Random(0, 1);
					}
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
				cp->elenum = suitable;
				if (ce->ecur_level != cp->pstart_level){ // 现在电梯和人不在同一层 标记 
					ce->levellist[cp->pstart_level] = YES;
					if (ce->status == IDLE || ce->status == PARKING){
						ce->status = RUNNING;
						char msg[100];
						snprintf(msg, sizeof(msg), "启动"); 
						Ani_Update_Elevator_Status(suitable, msg);
						
						ce->direction = Judge_Direction(ce->ecur_level, cp->pstart_level);
						Look_Algorithm(ce);
					} 
					// 不是IDLE则继续当前状态 
				} else { // 现在电梯和人在同一层 
					switch (ce->status){
						case (PARKING):
						case (IDLE):
						case (CLOSING):{
							ce->status = OPENING; 							
							Ani_Update_Elevator_Status(suitable, "开门中……");
							
							ce->last_action_time = current_time;
							
							// 提前将门开了事件加入事件链表
							EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, suitable, -1, cp->pstart_level);							
							break;
						}
						case (OPEN):
						case (SERVING):{
							// 理论上此条件一直满足 不然suitable = -1 
							if ((ce->direction == required_direction || ce->ecur_people_num + ce->reserved_people_num == 0) && ce->ecur_people_num + ce->reserved_people_num < MAX_CAPACITY){
								if (ce->ecur_people_num + ce->reserved_people_num == 0){
									ce->direction = required_direction;
								}
								// 需要特别处理 因为此处没有显式的门开了事件让人进入 
								ce->status = SERVING;
								Ani_Update_Elevator_Status(suitable, "门已开");
								ce->reserved_people_num++;
								
								// 人开始进入时就把状态设为IN_ELEVATOR以免进入时发生放弃事件 
								cp->status = IN_ELEVATOR;
								people[cp->pnum].status = IN_ELEVATOR; 
								
								cp->elenum = ce->id;
								people[cp->pnum].elenum = ce->id;
								
								// 处理此情况的专用计时器
								Time_Type open_time = 0;
								Time_Type io_time = 0;
								// 找到之前的门开事件 
								EL_Node *p = event_list;
								while (p){
									if (p->type == DOOR_OPENED && p->elevator_id == ce->id && p->level == ce->ecur_level && p->time <= current_time){
										if (p->time > open_time){
											open_time = p->time;
										}
									}
									p = p->next;
								} 
								// 找到门开后最后的进出 如果没有另说
								p = event_list;
								while (p){
									if ((p->type == PEOPLE_IN || p->type == PEOPLE_OUT) && p->elevator_id == ce->id && p->level == ce->ecur_level && p->time > open_time){
										if (p->time > io_time){
											io_time = p->time;
										}
									}
									p = p->next;
								} 
								if (io_time > current_time){
									io_time = io_time;
								} else { // 如果门虽然开了但是之前没有进出 也就是上一个循环io_time没有被成功赋值 或者进出事件在current_time已经结束 
									io_time = current_time;
								} 
								// 提前将人进入事件加入事件链表
								EL_Insert(io_time + PEOPLE_IN_OUT_TIME, PEOPLE_IN, ce->id, cp->pnum, ce->ecur_level);
								
								// 提前将人进入后的检查事件加入事件链表 
								EL_Insert(io_time + PEOPLE_IN_OUT_TIME + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
							} else {
								exit(10);
							}
							break;
						}
						case (OPENING):{
							// 没有什么需要处理的 人已经进入队列 在门开了事件中人才进入 
							break;
						}
					}	
				} 
			} else {
				// 没有合适电梯 则继续等待 人留在队列 后续由LOOK算法的判断方向和判断是否停靠处理
				cp->elenum = -1;
			}
			LQ_EnQueue(cp->pstart_level, Judge_Direction(cp->pstart_level, cp->pend_level), *cp);
			Ani_Update_Waiting_Queue(cp->pstart_level, &queues[cp->pstart_level][UP], &queues[cp->pstart_level][DOWN]);
			// 此到达事件解决 产生下一个 
			Generate_New_People();
			break;
		}
		case ELEVATOR_ARRIVAL:{
			if (!ce){
				exit(4);
			}
			if (ce->status != RUNNING && ce->status != PARKING){
				break;
			}
			Level_Type old_level = ce->ecur_level;
			ce->ecur_level = event->level;
			
			ce->last_action_time = current_time;
			
			Ani_Update_Elevator_Position(ce->id, old_level, ce->ecur_level, ce);
			Is_Stop(ce);
			break;
		}
		case DOOR_OPENED:{
			if (!ce){
				exit(5);
			}
			ce->status = OPEN;
			ce->last_action_time = current_time;			
			ce->idle_start_time = current_time;
			
			char msg[100];
			snprintf(msg, sizeof(msg), "门已开");
			Ani_Update_Elevator_Status(ce->id, msg);
			
			PL_Node *ptr = ce->peoplelist->next;
			LQ_Node *qtr = queues[ce->ecur_level][ce->direction].front->next;
			Elevator_Direction flag = STILL;
			ce->reserved_people_num = 0;
			
			// 处理人进出的专用计时器
			Time_Type io_time = current_time; 
			// 关于后续电梯情况更改的标记 
			int active = 0;
			
			// 下电梯 
			while (ptr){ 
				if (ptr->people.pend_level == ce->ecur_level){
					active = 1;
					// 提前将人出去事件加入事件链表
					EL_Insert(io_time + PEOPLE_IN_OUT_TIME, PEOPLE_OUT, ce->id, ptr->people.pnum, ce->ecur_level);
					io_time += PEOPLE_IN_OUT_TIME;

					ptr = ptr->next;
				} else {
					ptr = ptr->next;
				}
			}
			
			// 上电梯
			// 这里直接更新方向 如果flag还是STILL说明没人要进入
			if (ce->direction == UP || (ce->direction == STILL && ce->id == 0)){
				for (int i = ce->direction; i <= LEVEL_NUM; i++){
					if (ce->levellist[i] == YES){
						flag = UP;
						ce->direction = UP;
						break;
					}
				}
				if (flag != UP){
					if (LQ_Get_Num_Wait(ce->ecur_level, UP, ce->id) != 0){
						flag = UP;
						ce->direction = UP;
					} else if (LQ_Get_Num_Wait(ce->ecur_level, DOWN, ce->id) != 0){
						flag = DOWN;
						ce->direction = DOWN;
					}
				} 
			}
			
			if (ce->direction == DOWN || (ce->direction == STILL && ce->id == 1)){ 
				for (int i = ce->direction; i >= 1; i--){
					if (ce->levellist[i] == YES){
						flag = DOWN;
						ce->direction = DOWN;
						break;
					}
				}
				if (flag != DOWN){
					if (LQ_Get_Num_Wait(ce->ecur_level, DOWN, ce->id) != 0){
						flag = DOWN;
						ce->direction = DOWN;
					} else if (LQ_Get_Num_Wait(ce->ecur_level, UP, ce->id) != 0){
						flag = UP;
						ce->direction = UP;
					}
				}
			}
			
			// 方向可能改变 重置qtr指针
			if (flag != STILL){
				qtr = queues[ce->ecur_level][flag].front->next;
			}

			while (flag != STILL && qtr && ce->ecur_people_num + ce->reserved_people_num < MAX_CAPACITY){
				if (io_time - qtr->people.arr_time <= qtr->people.max_wait && qtr->people.status == WAITING){
					if (qtr->people.elenum != -1 && qtr->people.elenum != ce->id){
						qtr = qtr->next;
						continue;
					}
					active = 1;
					// 提前将人进入事件加入事件链表
					EL_Insert(io_time + PEOPLE_IN_OUT_TIME, PEOPLE_IN, ce->id, qtr->people.pnum, ce->ecur_level);
					// 提前将人状态设为IN_ELEVATOR以免进入时发生放弃事件
					qtr->people.status = IN_ELEVATOR;
					people[qtr->people.pnum].status = IN_ELEVATOR;
					
					qtr->people.elenum = ce->id;
					people[qtr->people.pnum].elenum = ce->id;
					 
					io_time += PEOPLE_IN_OUT_TIME;
					ce->reserved_people_num++;
					qtr = qtr->next;
				} else {
					qtr = qtr->next;			
				}
			}
			
			if (active){
				ce->status = SERVING;
			} else {
				// 没有进出活动 也许是有人放弃了 或者刚好进入了开着的电梯 
			}
			// 提前将检查自动关门事件加入事件链表
			EL_Insert(io_time + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
			break;
		}
		case DOOR_CHECK:{
			if (!ce){
				exit(8);
			}
			if (ce->status != OPEN && ce->status != SERVING){ 
				break; 
			}
			
			// 理论上不会有人还没出电梯 
			PL_Node *ptr = ce->peoplelist->next;
			while (ptr){
				if (ptr->people.pend_level == ce->ecur_level){
					exit(9);
				}
				ptr = ptr->next;
			}
					
			if (ce->reserved_people_num == 0){
				ce->status = CLOSING;
				ce->last_action_time = current_time;
				char msg[100];
				snprintf(msg, sizeof(msg), "关门中……");
				Ani_Update_Elevator_Status(ce->id, msg);
				// 提前将电梯门关闭事件加入事件链表 
				EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_CLOSED, ce->id, -1, ce->ecur_level);
			} 
			// 如果还有人没进电梯 在PEOPLE_ARRIVAL会加入DOOR_CHECK检查 
			break;
		}
		case DOOR_CLOSED:{
			if (!ce){
				exit(13);
			}
			if (ce->status != CLOSING){
				break;
			} 
			char msg[100];
			snprintf(msg, sizeof(msg), "门已关");
			Ani_Update_Elevator_Status(ce->id, msg);
			
			Elevator_Direction d;
			if (ce->ecur_people_num != 0){
				ce->status = RUNNING;
				// 内部有人 方向理论上已经在开门时决定
				Look_Algorithm(ce);
			} else { // 电梯空了 
				Status demand = STATUS_FALSE;
				for (int i = 1; i <= LEVEL_NUM; i++){
					if (ce->levellist[i] == YES){
						demand = STATUS_TRUE;
						break;
					}
				}
				for (int i = 1; i <= LEVEL_NUM; i++){
					if (LQ_Get_Num_Wait(i, UP, ce->id) != 0 || LQ_Get_Num_Wait(i, DOWN, ce->id) != 0){
						demand = STATUS_TRUE;
						break;
					}
				}
				if (demand == STATUS_TRUE){
					d = Direction_Change(ce); // 但是还有需求 不能停止 
					if (d != STILL){
						ce->status = RUNNING;
						ce->direction = d;
						Look_Algorithm(ce);
						break;
					} else {
						ce->status = IDLE;
						ce->direction = STILL;
						ce->idle_start_time = current_time;
						char msg[100];
						snprintf(msg, sizeof(msg), "门已关（停留）");
						Ani_Update_Elevator_Status(ce->id, msg);

						// 提前加入是否要回去待命检查
						EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, ce->id, -1, ce->ecur_level); 
						break;
					}
				}
			
				ce->status = IDLE;
				ce->direction = STILL;
				ce->idle_start_time = current_time;
				char msg[100];
				snprintf(msg, sizeof(msg), "门已关（停留）");
				Ani_Update_Elevator_Status(ce->id, msg);
				// 提前加入是否要回去待命检查
				EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, ce->id, -1, ce->ecur_level); 
			} 
			break;
		}
		case PEOPLE_IN:{
			if (!ce || !cp){
				exit(14);
			}
			if (cp->status == GIVE_UP){
				ce->reserved_people_num--;
				break;
			}
			if (cp->elenum != ce->id){
				ce->reserved_people_num--;
				break;
			}
			// 把此人从队列中删除 
			Elevator_Direction required_direction = Judge_Direction(cp->pstart_level, cp->pend_level);
			if (LQ_Del(ce->ecur_level, required_direction, cp->pnum) == STATUS_ERROR){
				exit(11);
			}
			ce->ecur_people_num++;
			ce->reserved_people_num--;
			ce->last_action_time = current_time;
			
			cp->status = IN_ELEVATOR;
			PL_Insert(ce->peoplelist, *cp);
			
			ce->levellist[cp->pend_level] = YES;
			
			Ani_Update_Elevator_People(ce->id, ce->ecur_level, ce);
			Ani_Update_Waiting_Queue(ce->ecur_level, &queues[ce->ecur_level][UP], &queues[ce->ecur_level][DOWN]);
			char msg[100];
			snprintf(msg, sizeof(msg), "%d号人在%dF进入电梯%c", cp->pnum, ce->ecur_level, ce->id + 65);
			Ani_Update_General_Person_Status(msg);
			Ani_Update_Elevator_Status(ce->id, "上客中……");
			
			break;
		}
		case PEOPLE_OUT:{
			if (!ce || !cp){
				exit(15);
			}
			if (cp->status == GIVE_UP){
				exit(16);
			}
			// 把此人从乘客链表删除
			PL_Node *tar = ce->peoplelist->next;
			while (tar && tar->people.pnum != cp->pnum){
				tar = tar->next;
			} 
			if (!tar){
				exit(20);
			}
			if (PL_Del(ce->peoplelist, tar) == STATUS_ERROR){
				exit(6);
			}

			ce->ecur_people_num--;
			ce->last_action_time = current_time; 
			
			cp->status = ARRIVED;
			
			arrived_people_num++;
			
			Ani_Update_Elevator_People(ce->id, ce->ecur_level, ce);
			char msg[100];
			snprintf(msg, sizeof(msg), "%d号人在%dF离开电梯%c", cp->pnum, ce->ecur_level, ce->id + 65); 
			Ani_Update_General_Person_Status(msg);
			Ani_Update_Elevator_Status(ce->id, "下客中……");
			break;
		}
		case IDLE_CHECK:{
			if (!ce){
				exit(17);
			}
			Level_Type tl = ce->id == 0? 1: LEVEL_NUM;
			
			// 检查空闲时间 
			if (ce->status == IDLE || current_time - ce->idle_start_time >= AUTO_ORDER_TIME){
				if (ce->ecur_level != tl){
					ce->status = PARKING;
					ce->direction = Judge_Direction(ce->ecur_level, tl);
					Ani_Update_Elevator_Status(ce->id, "返回待机");
					
					if (e->id == 0){
						Ani_Debug_A("");
					} else if (e->id == 1){
						Ani_Debug_B("");
					}
					
					Look_Algorithm(ce);
				} else {
					ce->direction = STILL;
					ce->status = IDLE;
					Ani_Update_Elevator_Status(ce->id, "待机");
				} 
			} else if (ce->status == IDLE){ // 其实理论上不应该发生 但是上保险
				// 提前加入是否要回去待命检查
				Time_Type rt = AUTO_ORDER_TIME - (current_time - ce->last_action_time); 
				EL_Insert(current_time + rt, IDLE_CHECK, ce->id, -1, ce->ecur_level); 
			}
			// 若非IDLE检查无效 直接返回 
			break;
		}
		case PEOPLE_GIVE_UP:{
			if (!cp){
				exit(18);
			} 
			if (cp->status == IN_ELEVATOR){
				break;
			}
			
			if (cp->status == WAITING){
				cp->status = GIVE_UP;
				give_up_people_num++;
				
				// 把人从队列中删除 不能用LQ_DeQueue要一个个找 其实可以用LQ_Del 
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
				Ani_Update_Waiting_Queue(cp->pstart_level, &queues[cp->pstart_level][UP], &queues[cp->pstart_level][DOWN]);
			}
			break;
		}
		default:{
			exit(19);
			break;
		}
			
	}
	return STATUS_OK;
} 
// 总模拟函数
Status Simulation(){
	Generate_New_People();
	EL_Node *current_event = event_list->next;
	
	Time_Type t = 0;
	while (t <= MAX_TIME){
		current_time = t;
		Ani_Update_Time(t);

		while (current_event != NULL && current_event->time <= current_time){
			if (current_event->time > MAX_TIME){
				break;
			}
			Process_Event(current_event);
			current_event = current_event->next;
			// 上保险 
			if (current_time >= MAX_TIME * 2){
				exit(3);
			}
		} 
		Sleep(ANIMATION_DELAY);
		t++; 
	}
	return STATUS_OK;
}
// 是否停靠
Status Is_Stop(Elevator *e){
	Status stop = STATUS_FALSE;
	if (e->levellist[e->ecur_level] == YES || LQ_Get_Num_Wait(e->ecur_level, e->direction, e->id) != 0){
		stop = STATUS_TRUE;
		char msg[100];
		snprintf(msg, sizeof(msg), "停靠%dF", e->ecur_level);

		e->status = OPENING;
		
		// 提前将开门事件加入事件链表 
		EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, e->id, -1, e->ecur_level);
		e->levellist[e->ecur_level] = NO;
	} else if (e->ecur_level == LEVEL_NUM && e->direction == UP && LQ_Get_Num_Wait(e->ecur_level, DOWN, e->id) != 0){
		stop = STATUS_TRUE;
		char msg[100];
		snprintf(msg, sizeof(msg), "停靠%dF", e->ecur_level);

		e->status = OPENING;
		
		// 提前将开门事件加入事件链表 
		EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, e->id, -1, e->ecur_level);
		e->levellist[e->ecur_level] = NO;
	} else if (e->ecur_level == 1 && e->direction == DOWN && LQ_Get_Num_Wait(e->ecur_level, UP, e->id) != 0){
		stop = STATUS_TRUE;
		char msg[100];
		snprintf(msg, sizeof(msg), "停靠%dF", e->ecur_level);

		e->status = OPENING;
		
		// 提前将开门事件加入事件链表 
		EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, e->id, -1, e->ecur_level);
		e->levellist[e->ecur_level] = NO;
	} else {
		Look_Algorithm(e); 
	}
	return STATUS_OK;
}
// 是否转向
Elevator_Direction Direction_Change(Elevator *e){
	Elevator_Direction d = e->direction;
	// 当前方向有需求则不转向 否则相反方向有需求则转向 均无需求先STILL 
	if (d == UP){
		for (int i = e->ecur_level + 1; i <= LEVEL_NUM; i++){
			if (e->levellist[i] == YES){
				return UP;
			}
			if (LQ_Get_Num_Wait(i, UP, e->id) != 0 || LQ_Get_Num_Wait(i, DOWN, e->id) != 0){
				return UP;
			}
		}
		for (int i = e->ecur_level - 1; i >= 1; i--){
			if (e->levellist[i] == YES){
				return DOWN;
			}
			if (LQ_Get_Num_Wait(i, UP, e->id) != 0 || LQ_Get_Num_Wait(i, DOWN, e->id) != 0){
				return DOWN;
			}
		}
		if (LQ_Get_Num_Wait(e->ecur_level, UP, e->id) != 0){
			return UP;
		} else if (LQ_Get_Num_Wait(e->ecur_level, DOWN, e->id) != 0){
			return DOWN;
		}
	} else if (d == DOWN){
		for (int i = e->ecur_level - 1; i >= 1; i--){
			if (e->levellist[i] == YES){
				return DOWN;
			}
			if (LQ_Get_Num_Wait(i, UP, e->id) != 0 || LQ_Get_Num_Wait(i, DOWN, e->id) != 0){
				return DOWN;
			}
		}
		for (int i = e->ecur_level + 1; i <= LEVEL_NUM; i++){
			if (e->levellist[i] == YES){
				return UP;
			}
			if (LQ_Get_Num_Wait(i, UP, e->id) != 0 || LQ_Get_Num_Wait(i, DOWN, e->id) != 0){
				return UP;
			}			
		}
		if (LQ_Get_Num_Wait(e->ecur_level, DOWN, e->id) != 0){
			return DOWN;
		} else if (LQ_Get_Num_Wait(e->ecur_level, UP, e->id) != 0){
			return UP;
		}
	} 

	return STILL;
} 
// LOOK算法
Status Look_Algorithm(Elevator *e){ 
	Elevator_Direction d = Direction_Change(e);
	
	// 该方向无需求 反方向有需求 转向 
	// PARKING除外
	if (e->status != PARKING && d != STILL && d != e->direction){
		char msg[100];
		snprintf(msg, sizeof(msg), "转换方向"); 
		if (e->id == 0){
			Ani_Debug_A(msg);
		} else if (e->id == 1){
			Ani_Debug_B(msg);
		}
		e->direction = d;
		
		// 判断该层反方向是否需要停靠 
		Is_Stop(e);
		// 如果不需要停靠则e->status是RUNNING 
		if (e->status == RUNNING){
			Look_Algorithm(e);  
		}
		// 如果需要停靠 e->status会变成OPENING并且有一系列事件 直到门关上继续调用LOOK算法 在这里先返回 
		return STATUS_OK; 
	}
	// 不必转向 前方有需求 或者PARKING状态 
	if ((e->status == RUNNING && d != STILL && d == e->direction) || e->status == PARKING){
		Level_Type next_level = e->ecur_level;
		if (e->direction == UP && e->ecur_level < LEVEL_NUM){  
			next_level++;
		} else if (e->direction == DOWN && e->ecur_level > 1){ 
			next_level--;
		} else if (e->status == PARKING){ 
			Level_Type tl = e->id == 0? 1: LEVEL_NUM;

			if (tl == e->ecur_level){ // PARKING到候命层了
				e->last_action_time = current_time;
				e->status = IDLE;
				e->direction = STILL;
				Ani_Update_Elevator_Status(e->id, "待机");
				return STATUS_OK;
			} 

			e->direction = Judge_Direction(e->ecur_level, tl);
			Look_Algorithm(e); 
		} else {
			// 可能已经到最低层或最高层了 
			e->status = IDLE;
			e->direction = STILL;
			e->idle_start_time = current_time;

			// 提前加入是否要回去待命检查 
			EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, e->id, -1, e->ecur_level);
			return STATUS_OK; 
		}
		e->last_action_time = current_time;
		// 提前加入到达事件 
		EL_Insert(current_time + MOVE_LEVEL_TIME, ELEVATOR_ARRIVAL, e->id, -1, next_level);
		return STATUS_OK; 
	}
	if (d == STILL){  
		e->direction = STILL;
		if (e->status == RUNNING){
			// 如果人刚刚好被另一台接走 此电梯缺少一个检查事件
			e->last_action_time = current_time;
			e->status = IDLE;
			e->direction = STILL;
			Ani_Update_Elevator_Status(e->id, "门已关（停留）");
			EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, e->id, -1, e->ecur_level);
		} 
	}
	return STATUS_OK;
}
// 统计信息 
Status Stat(){
	_getch();
	system("cls");
	printf("-----结果统计-----\n");
	printf("总计生成人数：%d\n", generated_people_num);
	printf("总计到达人数：%d\n", arrived_people_num);
	printf("总计放弃人数：%d\n", give_up_people_num);
	printf("%d人还在等待……\n", generated_people_num - arrived_people_num - give_up_people_num);
	
	return STATUS_OK;
} 
// 随机数 
int Get_Random(int min, int max){
    return (rand() % (max - min + 1)) + min;
}
// 方向比较 
Elevator_Direction Judge_Direction(Level_Type start, Level_Type end){
	Elevator_Direction ret = STILL;
	if (start > end){
		ret = DOWN;
	} else if (start < end){
		ret = UP;
	}
	return ret;
} 
// 距离比较
Level_Type Distance(Level_Type a, Level_Type b){
	return abs(a - b);
} 

