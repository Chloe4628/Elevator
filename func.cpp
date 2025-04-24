// func.cpp
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "func.h"
#include "peoplelist.h"
#include "eventlist.h"
#include "levelqueue.h"

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
	return OK;
}
// 释放一切 
Status Close_Down(){
	for (int i = 0; i < 2; i++){
		PL_Destroy(e[i].peoplelist);
	}
	EL_Destroy(event_list);
	LQ_Destroy();
	return OK;
}
// 产生新人
Status Generate_New_People(){
	if (generated_people_num >= MAX_PEOPLE){
		printf("-----已生成%d人-----\n", generated_people_num);  
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
	
	EL_Insert(people[generated_people_num].arr_time, PEOPLE_ARRIVAL, -1, generated_people_num, people[generated_people_num].pstart_level); // 人到达加入事件链表 
	
	return OK; 
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

	switch (event->type){
		case PEOPLE_ARRIVAL:{
			if (!cp){
				printf("？？？异常1？？？\n");
				exit(1);
			}
			if (cp->status != WAITING){
				printf("？？？异常2？？？\n");
				exit(2); 
			}
			printf("%dt时，%d号人到达%dF等待进电梯，想去%dF\n", cp->arr_time, cp->pnum, cp->pstart_level, cp->pend_level);
			LQ_EnQueue(cp->pstart_level, Judge_Direction(cp->pstart_level, cp->pend_level), *cp);
			
			// 提前将放弃事件加入事件链表 
			EL_Insert(cp->arr_time + cp->max_wait, PEOPLE_GIVE_UP, -1, cp->pnum, cp->pstart_level);
			
			// 呼叫电梯
			int suitable = -1;
			Elevator_Direction required_direction = Judge_Direction(cp->pstart_level, cp->pend_level); // 人的要求方向 
			Elevator_Direction d0 = Judge_Direction(e[0].ecur_level, cp->pstart_level); // 电梯0到人的方向 
			Elevator_Direction d1 = Judge_Direction(e[1].ecur_level, cp->pstart_level); // 电梯1到人的方向
			 
			if ((e[0].status == IDLE || e[0].status == PARKING) && (e[1].status == IDLE || e[1].status == PARKING) 
			&& (e[0].ecur_people_num + e[0].reserved_people_num < MAX_CAPACITY && e[1].ecur_people_num + e[1].reserved_people_num < MAX_CAPACITY)){
				suitable = Get_Random(0, 1);
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
				if (ce->ecur_level != cp->pstart_level){ // 现在电梯和人不在同一层 标记 
				
					ce->levellist[cp->pstart_level] = YES;
					if (ce->status == IDLE || ce->status == PARKING){
						ce->status = RUNNING;
						printf("%dt时，空闲电梯%d收到%dF的%d号人需求，启动\n", current_time, suitable, cp->pstart_level, cp->pnum);
						
						ce->direction = Judge_Direction(ce->ecur_level, cp->pstart_level);
						Look_Algorithm(ce);
					} else {
						printf("%dt时，工作电梯%d收到%dF的%d号人需求，将会响应\n", current_time, suitable, cp->pstart_level, cp->pnum);
					}
					// 不是IDLE则继续当前状态 
				} else { // 现在电梯和人在同一层 
					printf("%dt时，电梯%d在%dF遇到%d号人\n", current_time, suitable, cp->pstart_level, cp->pnum);
					
					switch (ce->status){
						case (PARKING):
						case (IDLE):
						case (CLOSING):{
							printf("%dt时，电梯%d在%dF遇到%d号人，准备开门\n", current_time, suitable, cp->pstart_level, cp->pnum);
							ce->status = OPENING; 
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
								printf("%dt时，电梯%d在%dF遇到%d号人，刚好开着门\n", current_time, suitable, cp->pstart_level, cp->pnum);
								ce->status = SERVING;
								ce->reserved_people_num++;
								
								// 人开始进入时就把状态设为IN_ELEVATOR以免进入时发生放弃事件 
								cp->status = IN_ELEVATOR; 
								
								// 处理人进出的专用计时器
								Time_Type io_time = current_time;
								// 找到最后一个进出事件节点 
								EL_Node *p = event_list;
								while (p){
									if ((p->type == IN || p->type == OUT) && p->elevator_id == ce->id && p->time > io_time){
										io_time = p->time;	
									}
									p = p->next;
								} 
								
								// 提前将人进入事件加入事件链表
								EL_Insert(io_time + PEOPLE_IN_OUT_TIME, IN, ce->id, cp->pnum, ce->ecur_level);
								// 把此人从队列中删除 
								if (LQ_DeQueue(ce->ecur_level, required_direction) == ERROR){
									printf("？？？异常11？？？\n");
									exit(11);
								}
								
								// 提前将人进入后的检查事件加入事件链表 
								EL_Insert(io_time + PEOPLE_IN_OUT_TIME + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
							} else {
								printf("？？？异常10？？？\n");
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
			}
			// 没有合适电梯 则继续等待 人留在队列 后续由LOOK算法的判断方向和判断是否停靠处理
			// 此到达事件解决 产生下一个 
			Generate_New_People();
			break;
		}
		case ELEVATOR_ARRIVAL:{
			if (!ce){
				printf("？？？异常4？？？\n");
				exit(4);
			}
			if (ce->status != RUNNING && ce->status != PARKING){
				break;
			}
			ce->ecur_level = event->level;
			printf("%dt时，电梯%d来到%dF\n", current_time, ce->id, ce->ecur_level);
			
			ce->last_action_time = current_time;
			
			Is_Stop(ce);
			break;
		}
		case DOOR_OPENED:{
			if (!ce){
				printf("？？？异常5？？？\n");
				exit(5);
			}
			printf("%dt时，电梯%d在%dF开门（载客：%d/%d）\n", current_time, ce->id, ce->ecur_level, ce->ecur_people_num, MAX_CAPACITY);
			ce->status = OPEN;
			ce->last_action_time = current_time;			
			ce->idle_start_time = current_time;
			
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
					EL_Insert(io_time + PEOPLE_IN_OUT_TIME, OUT, ce->id, ptr->people.pnum, ce->ecur_level);
					io_time += PEOPLE_IN_OUT_TIME;
					
					PL_Node *tar = ptr;
					ptr = ptr->next;
					// 把此人从乘客链表删除 
					if (PL_Del(ce->peoplelist, tar) == ERROR){
						printf("？？？异常6？？？\n");
						exit(6);
					}
				} else {
					ptr = ptr->next;
				}
			}
			
			// 上电梯
			// 这里直接更新方向 如果flag还是STILL说明没人要进入
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
			
			// 方向可能改变 重置qtr指针
			if (flag != STILL){
				qtr = queues[ce->ecur_level][flag].front->next;
			}

			while (flag != STILL && qtr && ce->ecur_people_num + ce->reserved_people_num < MAX_CAPACITY){
//				puts("jjj");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if (io_time - qtr->people.arr_time <= qtr->people.max_wait){
//					puts("kkk");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					active = 1;
					// 提前将人进入事件加入事件链表
					EL_Insert(io_time + PEOPLE_IN_OUT_TIME, IN, ce->id, qtr->people.pnum, ce->ecur_level);
					// 提前将人状态设为IN_ELEVATOR以免进入时发生放弃事件
					qtr->people.status = IN_ELEVATOR;
					 
					io_time += PEOPLE_IN_OUT_TIME;
					ce->reserved_people_num++;
					
					LQ_Node *help = qtr;
					qtr = qtr->next;
					// 把此人从队列中删除 
					if (LQ_Del(ce->ecur_level, ce->direction, help->people.pnum) == ERROR){
						printf("？？？异常7？？？\n");
						exit(7);
					}
				} else {
//					puts("!!!1"); // 有人放弃才会这样 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					qtr = qtr->next;			
				}
			}
			
			if (active){
				ce->status = SERVING;
			} else {
				// 没有进出活动 也许是有人放弃了 
				printf("55555555555没有进出活动，看来是有人等不及放弃了或者上了其他电梯\n");
			}
			// 提前将检查自动关门事件加入事件链表
			EL_Insert(io_time + DOOR_CHECK_INTERVAL, DOOR_CHECK, ce->id, -1, ce->ecur_level);
			break;
		}
		case DOOR_CHECK:{
			if (!ce){
				printf("？？？异常8？？？\n");
				exit(8);
			}
			if (ce->status != OPEN && ce->status != SERVING){ 
				break; 
			}
			
//			puts("lll");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			// 理论上不会有人还没出电梯 
			PL_Node *ptr = ce->peoplelist->next;
			while (ptr){
				if (ptr->people.pend_level == ce->ecur_level){
					printf("？？？异常9？？？\n");
					exit(9);
				}
				ptr = ptr->next;
			}
					
			if (ce->reserved_people_num == 0){
				ce->status = CLOSING;
				ce->last_action_time = current_time;
				printf("%dt时，电梯%d在%dF开始关门\n", current_time, ce->id, ce->ecur_level);
				
				// 提前将电梯门关闭事件加入事件链表 
				EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_CLOSED, ce->id, -1, ce->ecur_level);
			} 
			// 如果还有人没进电梯 在PEOPLE_ARRIVAL会加入DOOR_CHECK检查 
			break;
		}
		case DOOR_CLOSED:{
			if (!ce){
				printf("？？？异常13？？？\n");
				exit(13);
			}
			printf("%dt时，电梯%d在%dF已经关门\n", current_time, ce->id, ce->ecur_level);
			
			if (ce->ecur_people_num != 0){
				ce->status = RUNNING;
				// 方向理论上已经在开门时决定 
				Look_Algorithm(ce);
			} else { // 电梯空了 
				ce->status = IDLE;
				ce->idle_start_time = current_time;
				printf("%dt时，电梯%d暂时无任务，停留在%dF\n", current_time, ce->id, ce->ecur_level);
				
				// 提前加入是否要回去待命检查
				EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, ce->id, -1, ce->ecur_level); 
			} 
			break;
		}
		case IN:{
			if (!ce || !cp){
				printf("？？？异常14？？？\n");
				exit(14);
			}
			printf("%dt时，%d号人在%dF进入了电梯%d\n", current_time, cp->pnum, ce->ecur_level, ce->id);
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
				printf("？？？异常15？？？\n");
				exit(15);
			}
			if (cp->status == GIVE_UP){
				printf("？？？异常16？？？\n");
				exit(16);
			}

			printf("%dt时，%d号人在%dF离开了电梯%d，成功抵达！！！\n", current_time, cp->pnum, ce->ecur_level, ce->id);
			ce->ecur_people_num--;
			ce->last_action_time = current_time; 
			
			cp->status = ARRIVED;
			
			arrived_people_num++;
			break;
		}
		case IDLE_CHECK:{
			if (!ce){
				printf("？？？异常17？？？\n");
				exit(17);
			}
			Level_Type tl = ce->id == 0? 1: LEVEL_NUM;
			
			// 检查上次活动事件和空闲时间 
			if (ce->status == IDLE || current_time - ce->last_action_time >= AUTO_ORDER_TIME){
				if (ce->ecur_level != tl){
					printf("%dt时，电梯%d已经空闲超过%dt，返回%dF候命\n", current_time, ce->id, AUTO_ORDER_TIME, tl);
					
					ce->status = PARKING;
					ce->direction = Judge_Direction(ce->ecur_level, tl);
					Look_Algorithm(ce);
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
				printf("？？？异常18？？？\n");
				exit(18);
			} 
			if (cp->status == IN_ELEVATOR){
				break;
			}
			
			if (cp->status == WAITING){
				printf("%dt时，%d号人没耐心了，离开队列555555555\n", current_time, cp->pnum);
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
			}
			break;
		}
		default:{
			printf("？？？异常19？？？\n");
			exit(19);
			break;
		}
			
	}
	return OK;
} 
// 总模拟函数
Status Simulation(){
	EL_Node *current_event = event_list;
	printf("-----模拟开始，最多%dt-----\n", MAX_SIMULATION_TIME);
	Generate_New_People();
	while ((current_event = Get_Next_Event(current_event)) != NULL){
		if (current_event->time > MAX_SIMULATION_TIME){
			break;
		}
		Process_Event(current_event);
		
		// 上保险 
		if (current_time >= MAX_SIMULATION_TIME * 2){
			printf("？？？异常3？？？\n");
			exit(3);
		}
	} 
	printf("-----模拟结束-----\n");
	return OK;
}
// 是否停靠
Status Is_Stop(Elevator *e){
	Status stop = FALSE;
	if (e->levellist[e->ecur_level] == YES || LQ_Get_Num(e->ecur_level, e->direction) != 0){
		stop = TRUE;
		printf("%dt时，电梯%d停靠%dF\n", current_time, e->id, e->ecur_level); 
		e->status = OPENING;
		
		// 提前将开门事件加入事件链表 
		EL_Insert(current_time + DOOR_OPEN_CLOSE_TIME, DOOR_OPENED, e->id, -1, e->ecur_level);
		e->levellist[e->ecur_level] = NO;
	} else {
		Look_Algorithm(e); 
	}
	return OK;
}
// 是否转向
Elevator_Direction Direction_Change(Elevator *e){
	Elevator_Direction d = e->direction;
	// 当前方向有需求则不转向 否则相反方向有需求则转向 均无需求先STILL 
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
// LOOK算法
Status Look_Algorithm(Elevator *e){ 
	Elevator_Direction d = Direction_Change(e);
	
	// 该方向无需求 反方向有需求 转向 
	// PARKING除外
	if (e->status != PARKING && d != STILL && d != e->direction){
		printf("%dt时，电梯%d在%dF转向\n", current_time, e->id, e->ecur_level);
		e->direction = d;
		
		// 判断该层反方向是否需要停靠 
		Is_Stop(e);
		// 如果不需要停靠则e->status是RUNNING 
		if (e->status == RUNNING){
			Look_Algorithm(e);  
		}
		// 如果需要停靠 e->status会变成OPENING并且有一系列事件 直到门关上继续调用LOOK算法 在这里先返回 
		return OK; 
	}
	// 不必转向 前方有需求 或者PARKING状态 
	if ((e->status == RUNNING && d != STILL && d == e->direction) || e->status == PARKING){
		Level_Type next_level = e->ecur_level;
		if (e->direction == UP && e->ecur_level < LEVEL_NUM){ // 往上 
//			puts("aaa");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			next_level++;
		} else if (e->direction == DOWN && e->ecur_level > 1){ // 往下 
//			puts("bbb");////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			next_level--;
		} else { // 理论上IDLE情况另外处理 在这里不会发生 但是为了保险
//			puts("ccc"); ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			e->status = IDLE;
			e->direction = STILL;
			e->idle_start_time = current_time;
			
			// 提前加入是否要回去待命检查
			EL_Insert(current_time + AUTO_ORDER_TIME, IDLE_CHECK, e->id, -1, e->ecur_level);
			return OK; 
		}
		e->last_action_time = current_time;
		// 提前加入到达事件 
		EL_Insert(current_time + MOVE_LEVEL_TIME, ELEVATOR_ARRIVAL, e->id, -1, next_level);
	}
	return OK;
}
// 统计信息 
Status Stat(){
	printf("-----结果统计-----\n");
	
	printf("总计生成人数：%d\n", generated_people_num);
	printf("总计到达人数：%d\n", arrived_people_num);
	printf("总计放弃人数：%d\n", give_up_people_num);
	printf("%d人还在等待……\n", generated_people_num - arrived_people_num - give_up_people_num);
	
	return OK;
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

