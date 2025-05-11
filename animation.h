// animation.h
#ifndef ANIMATION_H
#define ANIMATION_H

#define FLOOR_HEIGHT 3
#define ELEVATOR_0_X 16
#define ELEVATOR_1_X 44
#define ELEVATOR_WIDTH 25
#define MAX_WAIT_DISPLAY 15

#define BASE_Y 9
#define TIME_POS_Y 2
#define ELEVATOR_0_STATUS_Y 4
#define ELEVATOR_1_STATUS_Y 4
#define PERSON_0_STATUS_Y 6
#define PERSON_1_STATUS_Y 6

#define WAITING_QUEUE_X 84

#include "base.h"

void gotoxy(int x, int y);
void Clear_Line(int x_start, int y, int length);
int Floor2Y(Level_Type floor);
void Draw_Static_Frame();
void Erase_Elevator(int id, Level_Type floor);
void Draw_Elevtor(int id, Level_Type floor, Elevator *e);
void Erase_Waiting_Queue(Level_Type floor);
void Draw_Waiting_Queue(Level_Type floor, LQ_Queue *uq, LQ_Queue *dq);
void Ani_Show_Title();
void Ani_Init();
void Ani_Update_Time(Time_Type ct);
void Ani_Update_Elevator_Position(int id, Level_Type of, Level_Type nf, Elevator *e);
void Ani_Update_Elevator_People(int id, Level_Type cf, Elevator *e);
void Ani_Update_Elevator_Status(int id, const char *msg);
void Ani_Update_Waiting_Queue(Level_Type floor, LQ_Queue *uq, LQ_Queue *dq);
void Ani_Update_General_Person_Status(const char *msg);
void Ani_Debug_A(const char *msg);
void Ani_Debug_B(const char *msg);
void Ani_Clean_Up();

#endif
