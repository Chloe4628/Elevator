# test_ui.py
# 用于测试电梯UI界面的独立程序

import time
import threading
from elevator_ui import ani_show_title, ani_init, ani_update_time, ani_update_elevator_status
from elevator_ui import ani_update_elevator_position, ani_update_waiting_queue, ani_update_general_person_status

def main():
    # 显示欢迎界面
    ani_show_title()
    
    # 初始化UI
    ui = ani_init()
    
    # 模拟电梯运行
    print("开始模拟电梯运行...")
    
    # 更新初始状态
    ani_update_time(0)
    ani_update_general_person_status("模拟开始运行")
    
    # 模拟电梯A从1楼到5楼
    print("电梯A上行...")
    ani_update_elevator_status(0, "上行中")
    ui.elevators[0]["direction"] = "UP"
    
    for floor in range(1, LEVEL_NUM):
        time.sleep(1)
        ani_update_time(floor * 20)
        ani_update_elevator_position(0, floor, floor + 1, None)
    
    # 电梯A到达5楼并开门
    print("电梯A到达5楼...")
    ani_update_elevator_status(0, "开门中...")
    time.sleep(1)
    ani_update_elevator_status(0, "门已开")
    
    # 添加一些等待的乘客
    ani_update_waiting_queue(5, [{"pnum": 1, "pend_level": 1}], [])
    time.sleep(1)
    
    # 电梯A关门并下行
    ani_update_elevator_status(0, "关门中...")
    time.sleep(1)
    ani_update_elevator_status(0, "下行中")
    ui.elevators[0]["direction"] = "DOWN"
    
    # 模拟电梯A从5楼到1楼
    print("电梯A下行...")
    for floor in range(5, 1, -1):
        time.sleep(1)
        ani_update_time(100 + (5 - floor) * 20)
        ani_update_elevator_position(0, floor, floor - 1, None)
    
    # 电梯B从5楼到3楼
    print("电梯B下行...")
    ani_update_elevator_status(1, "下行中")
    ui.elevators[1]["direction"] = "DOWN"
    
    for floor in range(5, 3, -1):
        time.sleep(1)
        ani_update_time(200 + (5 - floor) * 20)
        ani_update_elevator_position(1, floor, floor - 1, None)
    
    # 电梯B到达3楼并开门
    print("电梯B到达3楼...")
    ani_update_elevator_status(1, "开门中...")
    time.sleep(1)
    ani_update_elevator_status(1, "门已开")
    
    # 添加一些等待的乘客
    ani_update_waiting_queue(3, [], [{"pnum": 2, "pend_level": 1}])
    time.sleep(1)
    
    # 电梯B关门并继续下行
    ani_update_elevator_status(1, "关门中...")
    time.sleep(1)
    ani_update_elevator_status(1, "下行中")
    
    # 模拟电梯B从3楼到1楼
    for floor in range(3, 1, -1):
        time.sleep(1)
        ani_update_time(300 + (3 - floor) * 20)
        ani_update_elevator_position(1, floor, floor - 1, None)
    
    # 两部电梯都到达1楼
    ani_update_elevator_status(0, "门已关(停留)")
    ani_update_elevator_status(1, "门已关(停留)")
    ui.elevators[0]["direction"] = "STILL"
    ui.elevators[1]["direction"] = "STILL"
    
    # 更新最终状态
    ani_update_general_person_status("模拟结束")
    ani_update_time(400)
    
    print("模拟结束，按Ctrl+C退出程序")
    
    # 保持窗口打开
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("程序已退出")

if __name__ == "__main__":
    # 定义常量
    LEVEL_NUM = 5
    
    # 启动主程序
    main()