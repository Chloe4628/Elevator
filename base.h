// base.h
#ifndef BASE_H
#define BASE_H

// ʱ�䵥λ�����룩 
#define ANIMATION_DELAY 70
// ����������ʱ�� 
#define MAX_SIMULATION_TIME 1200
// ��������1F��λʱ��
#define MOVE_LEVEL_TIME 20
// �˳���ʱ���������Сֵ 
#define ITV_MAX 100
#define ITV_MIN 0
// Ը��ȴ�ʱ��������Сֵ 
#define WAIT_MAX 500
#define WAIT_MIN 200
// ���ݿ�����ʱ��
#define DOOR_OPEN_CLOSE_TIME 20
// �˽���ʱ��
#define PEOPLE_IN_OUT_TIME 25
// �����Զ����ż��ʱ��
#define DOOR_CHECK_INTERVAL 40
// �����Զ��������ʱ��
#define AUTO_ORDER_TIME 300
// ��¥���� 
#define LEVEL_NUM 5
// ����ؿ��� 
#define MAX_CAPACITY 15
// �������������
#define MAX_PEOPLE 5

// ʱ���� 
typedef int Time_Type;
// ������ 
typedef int Level_Type;
// ����״̬ö��
typedef enum{
    STATUS_FALSE = 0,
    STATUS_TRUE = 1,
    STATUS_OK = 1,
    STATUS_ERROR = -1,
    STATUS_OVERFLOW = 0,
}Status; 
// ����״̬ö��
typedef enum{
	IDLE,
	RUNNING,
	PARKING, // ��ȥ���� 
	OPENING,
	OPEN,
	CLOSING,
	SERVING // �����˽��� 
}Elevator_Status; 
// ��״̬ö��
typedef enum {
    WAITING,
    IN_ELEVATOR,
    ARRIVED, // �ѵ��� 
    GIVE_UP
}People_Status;
// ���ݷ���ö��
typedef enum {
    UP = 0,   
    DOWN = 1,
    STILL = 2
}Elevator_Direction;
// ����ͣ����־
typedef enum {
    NO,
    YES 
}Stop;
// �¼�����ö��
typedef enum{
	PEOPLE_ARRIVAL,
	ELEVATOR_ARRIVAL,
	DOOR_OPENED,
	DOOR_CHECK,
	DOOR_CLOSED,
	PEOPLE_IN,
	PEOPLE_OUT,
	IDLE_CHECK,
	PEOPLE_GIVE_UP
}Event_Type; 

// �˿ͽṹ
typedef struct{
	Time_Type itv_time;
	Time_Type arr_time;
	Time_Type max_wait;
	Level_Type pstart_level;
	Level_Type pend_level;
	People_Status status;
	int pnum;
	int elenum;
}People;
// �˿�����ڵ� 
typedef struct PL_Node{
	People people;
	struct PL_Node *next;
}PL_Node;
// �¼��ڵ� 
typedef struct EL_Node{
    Time_Type time;  
    Event_Type type; 
    int elevator_id; // ��ص���ID��-1��ʾ���ض����ݣ�
    int person_id; // ��س˿�ID��-1��ʾ���ض��˿ͣ�
    Level_Type level; // ���¥��
    struct EL_Node *next;
}EL_Node;
// ���нڵ�
typedef struct LQ_Node{
	People people;
	struct LQ_Node *next;
}LQ_Node;
// ���нṹ
typedef struct{
	LQ_Node *front;
	LQ_Node *rear; 
}LQ_Queue; 
// ���ݽṹ
typedef struct {
    Level_Type ecur_level;           
    Stop levellist[LEVEL_NUM + 1]; // �Ƿ�ͣ���������飨��0����������
    Elevator_Direction direction;    
    PL_Node *peoplelist; // �����ڳ˿�����
    int ecur_people_num;
    int reserved_people_num; 
    Elevator_Status status; 
    Time_Type last_action_time; // �ϴζ���ʱ��
    Time_Type idle_start_time; // ���п�ʼʱ�� (����IDLE_CHECK)
    int id;
}Elevator;

// �ⲿ����  
extern Time_Type current_time; 
extern People people[MAX_PEOPLE + 1]; 
extern LQ_Queue queues[LEVEL_NUM + 1][2]; 
extern EL_Node *event_list;  
extern Time_Type Total_Wait_Time; 
extern int arrived_people_num; 
extern int give_up_people_num;
extern int generated_people_num; 
extern Elevator e[2];

#endif
