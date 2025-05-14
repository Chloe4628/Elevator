// base.h
#ifndef BASE_H
#define BASE_H

// 时间单位（毫秒） 
#define ANIMATION_DELAY 70
// 电梯总运行时间 
#define MAX_SIMULATION_TIME 1200
// 电梯运行1F单位时间
#define MOVE_LEVEL_TIME 20
// 人出现时间间隔最大最小值 
#define ITV_MAX 100
#define ITV_MIN 0
// 愿意等待时间的最大最小值 
#define WAIT_MAX 500
#define WAIT_MIN 200
// 电梯开关门时间
#define DOOR_OPEN_CLOSE_TIME 20
// 人进出时间
#define PEOPLE_IN_OUT_TIME 25
// 电梯自动关门检测时间
#define DOOR_CHECK_INTERVAL 40
// 电梯自动候命检测时间
#define AUTO_ORDER_TIME 300
// 总楼层数 
#define LEVEL_NUM 5
// 最大载客量 
#define MAX_CAPACITY 15
// 最多生成总人数
#define MAX_PEOPLE 5

// 时间类 
typedef int Time_Type;
// 层数类 
typedef int Level_Type;
// 函数状态枚举
typedef enum{
    STATUS_FALSE = 0,
    STATUS_TRUE = 1,
    STATUS_OK = 1,
    STATUS_ERROR = -1,
    STATUS_OVERFLOW = 0,
}Status; 
// 电梯状态枚举
typedef enum{
	IDLE,
	RUNNING,
	PARKING, // 正去候命 
	OPENING,
	OPEN,
	CLOSING,
	SERVING // 正有人进出 
}Elevator_Status; 
// 人状态枚举
typedef enum {
    WAITING,
    IN_ELEVATOR,
    ARRIVED, // 已到达 
    GIVE_UP
}People_Status;
// 电梯方向枚举
typedef enum {
    UP = 0,   
    DOWN = 1,
    STILL = 2
}Elevator_Direction;
// 电梯停靠标志
typedef enum {
    NO,
    YES 
}Stop;
// 事件类型枚举
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

// 乘客结构
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
// 乘客链表节点 
typedef struct PL_Node{
	People people;
	struct PL_Node *next;
}PL_Node;
// 事件节点 
typedef struct EL_Node{
    Time_Type time;  
    Event_Type type; 
    int elevator_id; // 相关电梯ID（-1表示无特定电梯）
    int person_id; // 相关乘客ID（-1表示无特定乘客）
    Level_Type level; // 相关楼层
    struct EL_Node *next;
}EL_Node;
// 队列节点
typedef struct LQ_Node{
	People people;
	struct LQ_Node *next;
}LQ_Node;
// 队列结构
typedef struct{
	LQ_Node *front;
	LQ_Node *rear; 
}LQ_Queue; 
// 电梯结构
typedef struct {
    Level_Type ecur_level;           
    Stop levellist[LEVEL_NUM + 1]; // 是否停靠布尔数组（第0个不计数）
    Elevator_Direction direction;    
    PL_Node *peoplelist; // 电梯内乘客链表
    int ecur_people_num;
    int reserved_people_num; 
    Elevator_Status status; 
    Time_Type last_action_time; // 上次动作时间
    Time_Type idle_start_time; // 空闲开始时间 (用于IDLE_CHECK)
    int id;
}Elevator;

// 外部变量  
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
