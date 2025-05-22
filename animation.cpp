// animation.cpp
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <conio.h>

#include "base.h"
#include "peoplelist.h"
#include "levelqueue.h"
#include "animation.h"

// ����ƶ�
void gotoxy(int x, int y){
	COORD coord = {(SHORT)x, (SHORT)y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
} 
// ���ָ�������ַ�
void Clear_Line(int x_start, int y, int length){
	gotoxy(x_start, y);
	for (int i = 0; i < length; i++){
		printf(" ");
	}
} 
// �����������
void Input_People(){
	printf("\n\n\n\n");
	printf("\n\t\t                   ���������������1-10��:");
	while (scanf("%d", &MAX_PEOPLE) != 1 || MAX_PEOPLE <= 0 || MAX_PEOPLE >= 11){
		printf("\t\t                   �������� �Զ��趨����Ϊ5\n"); 
		MAX_PEOPLE = 5; 
		_getch();
		break;
	}
	system("cls");
}
// ������ʱ�� 
void Input_Time(){
	printf("\n\n\n\n");
	printf("\n\t\t                   �������������ʱ�䣨1-3000��:");
	while (scanf("%d", &MAX_TIME) != 1 || MAX_TIME <= 0 || MAX_TIME >= 3001){
		printf("\t\t                   �������� �Զ��趨ʱ��Ϊ1000t\n"); 
		MAX_TIME = 1000; 
		_getch();
		break;
	}
	system("cls");
} 
// ��¥����ת��Ϊ����̨y����
int Floor2Y(Level_Type floor){
	return BASE_Y + (LEVEL_NUM - floor) * FLOOR_HEIGHT;
}
// ��̬���
void Draw_Static_Frame(){
	// ����
	for (int i = 0; i < 25; i++){
		for (int j = 0; j < 100; j++){
			printf(" ");
		}
		printf("\n");
	} 
	
	for (int i = 0; i < 2; i++){
		int base_x = (i == 0)? ELEVATOR_0_X : ELEVATOR_1_X;
		int shaft_start_y = Floor2Y(LEVEL_NUM);
		int shaft_end_y   = Floor2Y(1) + FLOOR_HEIGHT;
		
		// ����߽� 
		for (int j = shaft_start_y; j <= shaft_end_y; j++){
			gotoxy(base_x - 1, j - 3);
			printf("|");
			gotoxy(base_x +  ELEVATOR_WIDTH, j - 3);
			printf("|");
		}
		// ¥��ָ� 
		for (Level_Type fl = 1; fl <= LEVEL_NUM; fl++){
			int floor_y = Floor2Y(fl);
			gotoxy(base_x, floor_y - 3);
			for (int j = 0; j < ELEVATOR_WIDTH; j++){
				printf("=");
			}
			if (fl == 1){
				gotoxy(base_x, floor_y + FLOOR_HEIGHT - 3);
				for (int k = 0; k < ELEVATOR_WIDTH; k++){
					printf("=");
				}
			}
		}
		// ���ݱ�ǩ
		gotoxy(base_x + ELEVATOR_WIDTH / 2 - 2, BASE_Y - 4);
		printf("����%c", 65 + i);
	}
	// ¥����
	int floor_num_x = ELEVATOR_0_X - 3;
	for (Level_Type fl = 1; fl <= LEVEL_NUM; fl++){
		gotoxy(floor_num_x, Floor2Y(fl) - 1);
		printf("%dF", fl);
		gotoxy(floor_num_x + 57, Floor2Y(fl) - 1);
		printf("%dF", fl);
		gotoxy(WAITING_QUEUE_X, Floor2Y(fl) - 1);
		printf("���飺"); 
	} 
}
// �հ׸��ǵ�����ʾ
void Erase_Elevator(int id, Level_Type floor){
	int base_x = (id == 0)? ELEVATOR_0_X : ELEVATOR_1_X;
	int y = Floor2Y(floor) - 1;
	Clear_Line(base_x, y, ELEVATOR_WIDTH);
} 
// ���ݺͳ˿���Ϣ
void Draw_Elevator(int id, Level_Type floor, Elevator *e){
	int base_x = (id == 0)? ELEVATOR_0_X : ELEVATOR_1_X;
	int y = Floor2Y(floor) - 1;
	
	char buffer[ELEVATOR_WIDTH + 1] = {0};
	int current_pos = 0;
	
	buffer[current_pos] = '[';
	current_pos++;
	
	if (id == 0){
		switch (e->direction){
			case (UP): snprintf(buffer + current_pos, ELEVATOR_WIDTH - current_pos - 1, "��"); break;
			case (DOWN): snprintf(buffer + current_pos, ELEVATOR_WIDTH - current_pos - 1, "��"); break;
			case (STILL): snprintf(buffer + current_pos, ELEVATOR_WIDTH - current_pos - 1, "��"); break;
		}
	} else if (id == 1){
		switch (e->direction){
			case (UP): snprintf(buffer + current_pos, ELEVATOR_WIDTH - current_pos - 1, "��"); break;
			case (DOWN): snprintf(buffer + current_pos, ELEVATOR_WIDTH - current_pos - 1, "��"); break;
			case (STILL): snprintf(buffer + current_pos, ELEVATOR_WIDTH - current_pos - 1, "��"); break;
		}
	}
	current_pos += 2;
	
	PL_Node *p = e->peoplelist->next;
	while (p && current_pos < ELEVATOR_WIDTH - 2){
        int written = snprintf(
            buffer + current_pos,
            ELEVATOR_WIDTH - current_pos - 1,
            " %d(%dF)",
            p->people.pnum,
            p->people.pend_level
        );
	    if (written > 0){
			current_pos += written;
		} else {
			break;
		}
		p = p->next;
	}
	
	while (current_pos < ELEVATOR_WIDTH - 1){
		buffer[current_pos] = ' ';
		current_pos++;
	} 
	buffer[current_pos] = ']';
	current_pos++;
	buffer[current_pos] = '\0';
	
	gotoxy(base_x, y);
	printf("%s", buffer);
}
// �հ׸��ǵȺ����
void Erase_Waiting_Queue(Level_Type floor){
	int y = Floor2Y(floor);
	Clear_Line(WAITING_QUEUE_X + 6, y - 1, 30);
} 
// �Ⱥ����
void Draw_Waiting_Queue(Level_Type floor, LQ_Queue *uq, LQ_Queue *dq){
	int y = Floor2Y(floor);
	gotoxy(WAITING_QUEUE_X + 6, y - 1);
	int cnt = 0;
	char buffer[16];
	// UP����
	LQ_Node *p = uq->front->next;
	while (p && cnt < MAX_WAIT_DISPLAY){
        snprintf(buffer, sizeof(buffer), " %d��(%dF)", p->people.pnum, p->people.pend_level);
        printf("%s", buffer);
        p = p->next;
        cnt++;
	}
	// DOWN����
	LQ_Node *q = dq->front->next;
	while (q && cnt < MAX_WAIT_DISPLAY){
		snprintf(buffer, sizeof(buffer), " %d��(%dF)", q->people.pnum, q->people.pend_level);
		printf("%s", buffer);
		q = q->next;
		cnt++;
	} 
} 
// ��ӭ����
void Ani_Show_Title(){
	printf("\n\n\n");
	printf("\n\t\t********************************************************");
	printf("\n\t\t                                                        ");
	printf("\n\t\t��������������������ӭ�������ݹ���ϵͳ������������������\n\n");
	printf("\t\t                    ���������ʼģ��");
	printf("\n\t\t                                                        ");
	printf("\n\t\t********************************************************\n");
	_getch();
} 
// ��ʼ��
void Ani_Init(){
	system("cls");
	
	Input_Time();
	Input_People(); 
	
	Draw_Static_Frame();
	Ani_Update_Time(0);
	
	Ani_Update_Elevator_Status(0, "׼������");
	Ani_Update_Elevator_Status(1, "׼������");
	Draw_Elevator(0, 1, &e[0]);
	Draw_Elevator(1, LEVEL_NUM, &e[1]);
	Ani_Update_General_Person_Status("��ʼģ��");
} 
// ʱ��
void Ani_Update_Time(Time_Type ct){
	gotoxy(0, 0);
	printf("��ǰʱ�䣺%5d", ct);
} 
// ���µ���λ��
void Ani_Update_Elevator_Position(int id, Level_Type of, Level_Type nf, Elevator *e){
	Erase_Elevator(id, of);
	Draw_Elevator(id, nf, e);
} 
// ���µ����ڳ˿�
void Ani_Update_Elevator_People(int id, Level_Type cf, Elevator *e){
	Draw_Elevator(id, cf, e);
} 
// ���µ���״̬
void Ani_Update_Elevator_Status(int id, const char *msg){
	int x = 0, y = 0;
	if (id == 0){
		x = 15;
		y = 4;
	} else if (id == 1){
		x = 43;
		y = 4;
	}
	Clear_Line(x, y, 28);
	gotoxy(x, y);
	printf("%s", msg);
} 
// ���µȴ�����
void Ani_Update_Waiting_Queue(Level_Type floor, LQ_Queue *uq, LQ_Queue *dq){
	Erase_Waiting_Queue(floor);
	Draw_Waiting_Queue(floor, uq, dq);
} 
// ȫ����ʾ
void Ani_Update_General_Person_Status(const char *msg){
	Clear_Line(0, 1, 70);
	gotoxy(0, 1);
	printf("��ʾ��Ϣ��%s", msg);
} 
// debug��ʾ����A
void Ani_Debug_A(const char *msg){
} 
// debug��ʾ����B
void Ani_Debug_B(const char *msg){
} 
// �˳�
void Ani_Clean_Up(){
	gotoxy(0, 10);
	printf("��ʾ����\n");
} 

