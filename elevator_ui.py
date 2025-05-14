# elevator_ui.py
import tkinter as tk
from tkinter import ttk, font
import time
import threading

# 常量定义 - 对应原animation.h中的定义
FLOOR_HEIGHT = 80  # 楼层高度（像素）
ELEVATOR_WIDTH = 120  # 电梯宽度
LEVEL_NUM = 5  # 楼层数
MAX_CAPACITY = 15  # 电梯最大容量
MAX_WAIT_DISPLAY = 15  # 最大等待显示人数

# 颜色定义
BG_COLOR = "#f0f0f0"  # 背景色
ELEVATOR_COLOR = "#d0d0d0"  # 电梯颜色
ELEVATOR_BORDER = "#404040"  # 电梯边框
FLOOR_LINE_COLOR = "#808080"  # 楼层线颜色
TEXT_COLOR = "#000000"  # 文本颜色
UP_COLOR = "#4CAF50"  # 上行颜色
DOWN_COLOR = "#F44336"  # 下行颜色
STILL_COLOR = "#9E9E9E"  # 静止颜色

class ElevatorUI:
    def __init__(self, root):
        self.root = root
        self.root.title("电梯模拟系统")
        self.root.geometry("1000x700")
        self.root.configure(bg=BG_COLOR)
        
        # 创建字体
        self.title_font = font.Font(family="Arial", size=16, weight="bold")
        self.normal_font = font.Font(family="Arial", size=10)
        self.small_font = font.Font(family="Arial", size=8)
        
        # 创建主框架
        self.main_frame = tk.Frame(self.root, bg=BG_COLOR)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # 创建信息区域
        self.info_frame = tk.Frame(self.main_frame, bg=BG_COLOR)
        self.info_frame.pack(fill=tk.X, pady=5)
        
        # 时间显示
        self.time_label = tk.Label(self.info_frame, text="当前时间：0", font=self.normal_font, bg=BG_COLOR)
        self.time_label.pack(side=tk.LEFT, padx=10)
        
        # 通用状态信息
        self.status_label = tk.Label(self.info_frame, text="提示信息：开始模拟", font=self.normal_font, bg=BG_COLOR)
        self.status_label.pack(side=tk.LEFT, padx=20)
        
        # 电梯状态区域
        self.elevator_status_frame = tk.Frame(self.main_frame, bg=BG_COLOR)
        self.elevator_status_frame.pack(fill=tk.X, pady=5)
        
        # 电梯A状态
        self.elevator_a_status = tk.Label(self.elevator_status_frame, text="电梯A：准备就绪", font=self.normal_font, bg=BG_COLOR)
        self.elevator_a_status.pack(side=tk.LEFT, padx=10)
        
        # 电梯B状态
        self.elevator_b_status = tk.Label(self.elevator_status_frame, text="电梯B：准备就绪", font=self.normal_font, bg=BG_COLOR)
        self.elevator_b_status.pack(side=tk.LEFT, padx=50)
        
        # 创建电梯和楼层区域
        self.elevator_frame = tk.Frame(self.main_frame, bg=BG_COLOR)
        self.elevator_frame.pack(fill=tk.BOTH, expand=True, pady=10)
        
        # 创建画布
        self.canvas = tk.Canvas(self.elevator_frame, bg=BG_COLOR, highlightthickness=0)
        self.canvas.pack(fill=tk.BOTH, expand=True)
        
        # 等待队列区域
        self.queue_frame = tk.Frame(self.main_frame, bg=BG_COLOR)
        self.queue_frame.pack(fill=tk.X, pady=5)
        
        # 初始化电梯数据
        self.elevators = [
            {"id": 0, "floor": 1, "direction": "STILL", "people": [], "status": "空闲"},
            {"id": 1, "floor": LEVEL_NUM, "direction": "STILL", "people": [], "status": "空闲"}
        ]
        
        # 初始化等待队列数据
        self.waiting_queues = {}
        for floor in range(1, LEVEL_NUM + 1):
            self.waiting_queues[floor] = {"UP": [], "DOWN": []}
        
        # 绘制静态框架
        self.draw_static_frame()
        
        # 绘制初始电梯
        self.draw_elevator(0, 1)
        self.draw_elevator(1, LEVEL_NUM)
        
        # 创建消息队列用于线程间通信
        self.message_queue = []
        self.queue_lock = threading.Lock()
        
    def draw_static_frame(self):
        """绘制静态框架，包括电梯井道和楼层标记"""
        canvas_width = self.canvas.winfo_width() or 1000
        canvas_height = self.canvas.winfo_height() or 600
        
        # 计算电梯位置
        elevator_a_x = canvas_width * 0.25
        elevator_b_x = canvas_width * 0.55
        self.elevator_positions = [elevator_a_x, elevator_b_x]
        
        # 绘制楼层线和标记
        for floor in range(1, LEVEL_NUM + 1):
            y = self.floor_to_y(floor)
            
            # 楼层线
            self.canvas.create_line(50, y, canvas_width - 50, y, fill=FLOOR_LINE_COLOR)
            
            # 楼层标记
            self.canvas.create_text(30, y - 15, text=f"{floor}F", font=self.normal_font, fill=TEXT_COLOR)
            
            # 等待队列标签
            self.canvas.create_text(canvas_width - 200, y - 15, text="等待：", font=self.normal_font, fill=TEXT_COLOR)
            
            # 创建等待队列文本标签
            queue_text = tk.StringVar()
            queue_text.set("")
            queue_label = tk.Label(self.canvas, textvariable=queue_text, font=self.small_font, bg=BG_COLOR, justify=tk.LEFT)
            queue_window = self.canvas.create_window(canvas_width - 150, y - 15, window=queue_label, anchor=tk.W)
            
            # 存储队列文本变量的引用
            self.waiting_queues[floor]["label"] = queue_text
        
        # 绘制电梯井道
        for i, x in enumerate(self.elevator_positions):
            # 井道左边界
            self.canvas.create_line(
                x - ELEVATOR_WIDTH/2 - 5, self.floor_to_y(LEVEL_NUM) - 50,
                x - ELEVATOR_WIDTH/2 - 5, self.floor_to_y(1) + 50,
                fill=ELEVATOR_BORDER, width=2
            )
            
            # 井道右边界
            self.canvas.create_line(
                x + ELEVATOR_WIDTH/2 + 5, self.floor_to_y(LEVEL_NUM) - 50,
                x + ELEVATOR_WIDTH/2 + 5, self.floor_to_y(1) + 50,
                fill=ELEVATOR_BORDER, width=2
            )
            
            # 电梯标签
            self.canvas.create_text(x, self.floor_to_y(LEVEL_NUM) - 70, 
                                   text=f"电梯{chr(65+i)}", font=self.normal_font, fill=TEXT_COLOR)
    
    def floor_to_y(self, floor):
        """将楼层转换为画布y坐标"""
        canvas_height = self.canvas.winfo_height() or 600
        base_y = canvas_height - 50
        return base_y - (floor - 1) * FLOOR_HEIGHT
    
    def draw_elevator(self, id, floor, people=None):
        """绘制电梯"""
        if not hasattr(self, 'elevator_positions'):
            return
            
        x = self.elevator_positions[id]
        y = self.floor_to_y(floor)
        
        # 存储电梯矩形的引用
        if hasattr(self, f'elevator_{id}_rect') and getattr(self, f'elevator_{id}_rect'):
            self.canvas.delete(getattr(self, f'elevator_{id}_rect'))
            self.canvas.delete(getattr(self, f'elevator_{id}_text'))
        
        # 绘制电梯矩形
        elevator_rect = self.canvas.create_rectangle(
            x - ELEVATOR_WIDTH/2, y - 30,
            x + ELEVATOR_WIDTH/2, y + 10,
            fill=ELEVATOR_COLOR, outline=ELEVATOR_BORDER, width=2
        )
        
        # 准备电梯内文本
        direction_symbol = ""
        if self.elevators[id]["direction"] == "UP":
            direction_symbol = "↑"
            direction_color = UP_COLOR
        elif self.elevators[id]["direction"] == "DOWN":
            direction_symbol = "↓"
            direction_color = DOWN_COLOR
        else:
            direction_symbol = "■"
            direction_color = STILL_COLOR
        
        # 乘客信息
        people_text = ""
        if people:
            for i, person in enumerate(people):
                if i < 3:  # 限制显示的乘客数量
                    people_text += f" {person['pnum']}({person['pend_level']}F)"
                else:
                    people_text += "..."
                    break
        
        # 绘制电梯内文本
        elevator_text = self.canvas.create_text(
            x, y - 10,
            text=f"[{direction_symbol}{people_text}]",
            font=self.normal_font,
            fill=TEXT_COLOR
        )
        
        # 存储引用
        setattr(self, f'elevator_{id}_rect', elevator_rect)
        setattr(self, f'elevator_{id}_text', elevator_text)
    
    def update_elevator_position(self, id, old_floor, new_floor, people=None):
        """更新电梯位置（动画）"""
        self.elevators[id]["floor"] = new_floor
        
        # 计算起始和结束位置
        start_y = self.floor_to_y(old_floor)
        end_y = self.floor_to_y(new_floor)
        steps = 10  # 动画步数
        
        # 执行动画
        for step in range(steps + 1):
            progress = step / steps
            current_y = start_y + (end_y - start_y) * progress
            
            # 更新电梯位置
            x = self.elevator_positions[id]
            if hasattr(self, f'elevator_{id}_rect') and getattr(self, f'elevator_{id}_rect'):
                self.canvas.coords(
                    getattr(self, f'elevator_{id}_rect'),
                    x - ELEVATOR_WIDTH/2, current_y - 30,
                    x + ELEVATOR_WIDTH/2, current_y + 10
                )
                
                # 更新文本位置
                self.canvas.coords(getattr(self, f'elevator_{id}_text'), x, current_y - 10)
            
            self.root.update()
            time.sleep(0.05)  # 控制动画速度
        
        # 最终更新
        self.draw_elevator(id, new_floor, people)
    
    def update_elevator_status(self, id, status):
        """更新电梯状态信息"""
        self.elevators[id]["status"] = status
        if id == 0:
            self.elevator_a_status.config(text=f"电梯A：{status}")
        else:
            self.elevator_b_status.config(text=f"电梯B：{status}")
    
    def update_elevator_people(self, id, floor, people):
        """更新电梯内乘客信息"""
        self.elevators[id]["people"] = people
        self.draw_elevator(id, floor, people)
    
    def update_waiting_queue(self, floor, up_queue, down_queue):
        """更新等待队列显示"""
        queue_text = ""
        
        # 上行队列
        for i, person in enumerate(up_queue):
            if i < MAX_WAIT_DISPLAY:
                queue_text += f" {person['pnum']}↑({person['pend_level']}F)"
        
        # 下行队列
        for i, person in enumerate(down_queue):
            if i < MAX_WAIT_DISPLAY - len(up_queue):
                queue_text += f" {person['pnum']}↓({person['pend_level']}F)"
        
        # 更新显示
        if floor in self.waiting_queues and "label" in self.waiting_queues[floor]:
            self.waiting_queues[floor]["label"].set(queue_text)
    
    def update_time(self, time_value):
        """更新时间显示"""
        self.time_label.config(text=f"当前时间：{time_value}")
    
    def update_general_status(self, message):
        """更新通用状态信息"""
        self.status_label.config(text=f"提示信息：{message}")
    
    def process_message(self, message_type, **kwargs):
        """处理来自主程序的消息"""
        if message_type == "update_time":
            self.update_time(kwargs.get("time", 0))
        
        elif message_type == "update_elevator_position":
            self.update_elevator_position(
                kwargs.get("id", 0),
                kwargs.get("old_floor", 1),
                kwargs.get("new_floor", 1),
                kwargs.get("people", [])
            )
        
        elif message_type == "update_elevator_status":
            self.update_elevator_status(
                kwargs.get("id", 0),
                kwargs.get("status", "")
            )
        
        elif message_type == "update_elevator_people":
            self.update_elevator_people(
                kwargs.get("id", 0),
                kwargs.get("floor", 1),
                kwargs.get("people", [])
            )
        
        elif message_type == "update_waiting_queue":
            self.update_waiting_queue(
                kwargs.get("floor", 1),
                kwargs.get("up_queue", []),
                kwargs.get("down_queue", [])
            )
        
        elif message_type == "update_general_status":
            self.update_general_status(kwargs.get("message", ""))
    
    def add_message(self, message_type, **kwargs):
        """添加消息到队列"""
        with self.queue_lock:
            self.message_queue.append((message_type, kwargs))
    
    def process_message_queue(self):
        """处理消息队列"""
        with self.queue_lock:
            if self.message_queue:
                message_type, kwargs = self.message_queue.pop(0)
                self.process_message(message_type, **kwargs)
        
        # 继续处理队列
        self.root.after(10, self.process_message_queue)

# 以下是与C++程序交互的接口函数

def ani_show_title():
    """显示标题动画"""
    root = tk.Tk()
    root.title("电梯模拟系统")
    root.geometry("500x300")
    
    frame = tk.Frame(root, bg="white")
    frame.pack(fill=tk.BOTH, expand=True)
    
    title_label = tk.Label(
        frame, 
        text="欢迎使用电梯管理系统", 
        font=("Arial", 20, "bold"),
        bg="white"
    )
    title_label.pack(pady=50)
    
    subtitle_label = tk.Label(
        frame,
        text="按任意键开始模拟",
        font=("Arial", 14),
        bg="white"
    )
    subtitle_label.pack(pady=20)
    
    def close_window(event=None):
        root.destroy()
    
    root.bind("<Key>", close_window)
    root.mainloop()

# 全局UI实例
ui_instance = None

def ani_init():
    """初始化动画"""
    global ui_instance
    root = tk.Tk()
    ui_instance = ElevatorUI(root)
    
    # 开始处理消息队列
    ui_instance.process_message_queue()
    
    # 启动UI线程
    threading.Thread(target=root.mainloop, daemon=True).start()
    return ui_instance

def ani_update_time(current_time):
    """更新时间"""
    if ui_instance:
        ui_instance.add_message("update_time", time=current_time)

def ani_update_elevator_position(id, old_floor, new_floor, elevator):
    """更新电梯位置"""
    if ui_instance:
        # 从elevator结构中提取乘客信息
        people = []
        # 这里需要根据C++程序中的数据结构进行适配
        # 假设elevator.peoplelist是一个链表，需要遍历获取乘客信息
        
        ui_instance.add_message(
            "update_elevator_position",
            id=id,
            old_floor=old_floor,
            new_floor=new_floor,
            people=people
        )

def ani_update_elevator_people(id, current_floor, elevator):
    """更新电梯内乘客"""
    if ui_instance:
        # 从elevator结构中提取乘客信息
        people = []
        # 这里需要根据C++程序中的数据结构进行适配
        
        ui_instance.add_message(
            "update_elevator_people",
            id=id,
            floor=current_floor,
            people=people
        )

def ani_update_elevator_status(id, message):
    """更新电梯状态"""
    if ui_instance:
        ui_instance.add_message(
            "update_elevator_status",
            id=id,
            status=message
        )

def ani_update_waiting_queue(floor, up_queue, down_queue):
    """更新等待队列"""
    if ui_instance:
        # 从队列中提取乘客信息
        up_people = []
        down_people = []
        # 这里需要根据C++程序中的数据结构进行适配
        
        ui_instance.add_message(
            "update_waiting_queue",
            floor=floor,
            up_queue=up_people,
            down_queue=down_people
        )

def ani_update_general_person_status(message):
    """更新通用状态信息"""
    if ui_instance:
        ui_instance.add_message(
            "update_general_status",
            message=message
        )

def ani_clean_up():
    """清理资源"""
    global ui_instance
    if ui_instance:
        # 在这里可以添加清理代码
        ui_instance = None

# 测试代码 - 如果直接运行此文件，将显示一个演示
if __name__ == "__main__":
    ani_show_title()
    ui = ani_init()
    
    # 模拟一些电梯动作
    time.sleep(1)
    ani_update_time(10)
    ani_update_general_person_status("模拟开始运行")
    
    # 电梯A上行
    ani_update_elevator_status(0, "上行中")
    ui.elevators[0]["direction"] = "UP"
    time.sleep(1)
    
    # 模拟电梯移动
    for floor in range(1, 5):
        ani_update_elevator_position(0, floor, floor + 1, None)
        ani_update_time(10 + floor * 20)
        time.sleep(1)
    
    # 电梯A开门
    ani_update_elevator_status(0, "开门中...")
    time.sleep(1)
    ani_update_elevator_status(0, "门已开")
    
    # 添加一些等待的乘客
    ui.update_waiting_queue(3, [{"pnum": 1, "pend_level": 1}], [{"pnum": 2, "pend_level": 1}])
    
    # 电梯B下行
    ani_update_elevator_status(1, "下行中")
    ui.elevators[1]["direction"] = "DOWN"
    time.sleep(1)
    
    for floor in range(5, 1, -1):
        ani_update_elevator_position(1, floor, floor - 1, None)
        ani_update_time(10 + (5 - floor) * 20)
        time.sleep(1)
    
    # 电梯B开门
    ani_update_elevator_status(1, "开门中...")
    time.sleep(1)
    ani_update_elevator_status(1, "门已开")
    
    # 更新一些状态
    ani_update_general_person_status("模拟结束")
    ani_update_time(100)
    
    # 保持窗口打开
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        pass