# 电梯模拟系统 Tkinter UI 界面

## 项目概述

本项目为电梯模拟系统添加了一个基于Tkinter的图形用户界面，替代原有的命令行界面。新界面保持了与原命令行界面相同的功能和视觉布局，同时提供了更现代化的用户体验。

## 文件说明

- `elevator_ui.py`: Tkinter实现的电梯UI界面
- `python_ui_interface.cpp`: C++与Python之间的接口代码

## 功能特点

1. **与原系统功能一致**：完全实现了原命令行界面的所有功能
2. **实时动画效果**：电梯移动有平滑的动画效果
3. **多线程设计**：UI运行在独立线程，不影响主程序性能
4. **消息队列机制**：确保UI更新的线程安全

## 使用方法

### 编译设置

要使用Python UI界面，需要进行以下设置：

1. 确保安装了Python 3.6或更高版本
2. 安装必要的Python库：`tkinter`（通常随Python一起安装）
3. 编译时需要链接Python库：

```bash
# 示例编译命令（Windows环境）
g++ -o elevator.exe main.cpp func.cpp animation.cpp python_ui_interface.cpp peoplelist.cpp levelqueue.cpp eventlist.cpp -I"C:\Python39\include" -L"C:\Python39\libs" -lpython39
```

> 注意：请根据您的Python安装路径和版本调整上述命令中的路径和库名称

### 集成到现有代码

1. 将`elevator_ui.py`和`python_ui_interface.cpp`文件放入项目目录
2. 在项目中包含`python_ui_interface.cpp`而不是原来的`animation.cpp`
3. 或者修改`animation.cpp`，将其实现替换为调用`python_ui_interface.cpp`中的函数

### 运行程序

运行编译后的程序，UI界面将自动启动并显示电梯模拟过程。

## 界面说明

界面主要包含以下元素：

1. **时间显示**：显示当前模拟时间
2. **状态信息**：显示系统提示信息
3. **电梯状态**：显示电梯A和电梯B的当前状态
4. **电梯井道**：显示电梯的位置和移动
5. **楼层标记**：显示楼层编号
6. **等待队列**：显示各楼层的等待乘客

## 开发说明

### 消息传递机制

C++程序通过Python C API调用Python函数，将电梯状态信息传递给UI界面。主要函数对应关系：

| C++函数 | Python函数 | 功能 |
|---------|------------|------|
| `Ani_Show_Title()` | `ani_show_title()` | 显示标题动画 |
| `Ani_Init()` | `ani_init()` | 初始化界面 |
| `Ani_Update_Time()` | `ani_update_time()` | 更新时间显示 |
| `Ani_Update_Elevator_Position()` | `ani_update_elevator_position()` | 更新电梯位置 |
| `Ani_Update_Elevator_Status()` | `ani_update_elevator_status()` | 更新电梯状态 |
| `Ani_Update_Waiting_Queue()` | `ani_update_waiting_queue()` | 更新等待队列 |
| `Ani_Update_General_Person_Status()` | `ani_update_general_status()` | 更新通用状态信息 |
| `Ani_Clean_Up()` | `ani_clean_up()` | 清理资源 |

### 独立测试

可以直接运行`elevator_ui.py`进行界面测试：

```bash
python elevator_ui.py
```

这将启动一个演示模式，模拟电梯运行的基本动作。

## 注意事项

1. 确保Python路径正确设置，使C++程序能够找到Python解释器
2. 如果遇到中文显示问题，请确保使用UTF-8编码
3. 在Windows环境下，可能需要调整控制台设置以正确显示中文

## 扩展与改进

1. 可以添加更多交互功能，如手动控制电梯
2. 可以增加统计信息显示，如平均等待时间、服务效率等
3. 可以优化UI布局，适应不同屏幕尺寸