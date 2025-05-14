# func.cpp 伪代码

## 函数: Open_Up
**作用**: 初始化整个电梯系统，包括电梯状态、楼层队列和事件链表。

```pseudocode
FUNCTION Open_Up():
  // 初始化两部电梯
  FOR i FROM 0 TO 1:
    e[i].当前人数 = 0
    e[i].预留人数 = 0
    e[i].方向 = 静止
    e[i].乘客列表 = 初始化乘客列表()
    e[i].状态 = 空闲
    e[i].上次动作时间 = 0
    e[i].空闲开始时间 = 0
    e[i].id = i
    // 清空停靠楼层列表
    FOR j FROM 0 TO LEVEL_NUM:
      e[i].停靠楼层[j] = 否
    // 设置初始楼层
    IF i == 0 THEN
      e[i].当前楼层 = 1
    ELSE IF i == 1 THEN
      e[i].当前楼层 = LEVEL_NUM
    END IF
  END FOR

  初始化楼层队列()
  初始化事件链表()
  显示标题动画()
  初始化动画()

  RETURN 成功状态
END FUNCTION
```

## 函数: Close_Down
**作用**: 释放系统资源，包括销毁电梯的乘客列表、事件链表和楼层队列。

```pseudocode
FUNCTION Close_Down():
  FOR i FROM 0 TO 1:
    销毁乘客列表(e[i].乘客列表)
  END FOR
  销毁事件链表(事件链表)
  销毁楼层队列()
  清理动画资源()
  RETURN 成功状态
END FUNCTION
```

## 函数: Generate_New_People
**作用**: 生成新的乘客，并将其到达事件加入事件链表。

```pseudocode
FUNCTION Generate_New_People():
  IF 已生成人数 >= 最大人数 THEN
    RETURN 溢出状态
  END IF
  已生成人数 = 已生成人数 + 1
  当前乘客 = people[已生成人数]
  当前乘客.编号 = 已生成人数
  当前乘客.到达间隔时间 = 随机生成(最小间隔, 最大间隔)
  当前乘客.到达时间 = 当前时间 + 当前乘客.到达间隔时间
  当前乘客.最大等待时间 = 随机生成(最小等待, 最大等待)
  当前乘客.所在电梯编号 = -1 // 表示不在任何电梯内
  当前乘客.起始楼层 = 随机生成(1, LEVEL_NUM)
  REPEAT
    当前乘客.目标楼层 = 随机生成(1, LEVEL_NUM)
  UNTIL 当前乘客.起始楼层 != 当前乘客.目标楼层
  当前乘客.状态 = 等待中

  插入事件(当前乘客.到达时间, 乘客到达事件, -1, 当前乘客.编号, 当前乘客.起始楼层) 到 事件链表

  RETURN 成功状态
END FUNCTION
```

## 函数: Get_Next_Event
**作用**: 从事件链表中获取下一个待处理的事件。

```pseudocode
FUNCTION Get_Next_Event(current_event):
  IF current_event IS NULL THEN
    RETURN NULL
  END IF
  next_event = current_event.next
  RETURN next_event
END FUNCTION
```

## 函数: Process_Event
**作用**: 处理单个事件，根据事件类型执行相应的操作。

```pseudocode
FUNCTION Process_Event(event):
  当前时间 = event.time
  当前电梯 = NULL
  当前乘客 = NULL

  IF event.elevator_id IS NOT -1 THEN
    当前电梯 = &e[event.elevator_id]
  END IF
  IF event.person_id IS NOT -1 THEN
    当前乘客 = &people[event.person_id]
  END IF

  更新动画时间(当前时间)

  SWITCH event.type:
    CASE 乘客到达事件 (PEOPLE_ARRIVAL):
      IF 当前乘客 IS NULL THEN EXIT(1) END IF
      IF 当前乘客.状态 IS NOT 等待中 THEN EXIT(2) END IF

      将乘客加入对应楼层和方向的等待队列(当前乘客.起始楼层, 判断方向(当前乘客.起始楼层, 当前乘客.目标楼层), 当前乘客)
      更新等待队列动画(当前乘客.起始楼层, 上行队列, 下行队列)
      更新乘客状态信息动画(格式化信息: 当前时间, 乘客编号, 起始楼层, 目标楼层)

      // 将乘客放弃事件加入事件链表
      插入事件(当前乘客.到达时间 + 当前乘客.最大等待时间, 乘客放弃事件, -1, 当前乘客.编号, 当前乘客.起始楼层) 到 事件链表

      // 呼叫电梯逻辑
      suitable_elevator_id = -1
      乘客需求方向 = 判断方向(当前乘客.起始楼层, 当前乘客.目标楼层)
      电梯0到乘客方向 = 判断方向(e[0].当前楼层, 当前乘客.起始楼层)
      电梯1到乘客方向 = 判断方向(e[1].当前楼层, 当前乘客.起始楼层)

      // 选择合适的电梯 (详细逻辑省略，参考原代码中的多条件判断)
      // ... 根据电梯状态(空闲/停靠)、容量、方向、距离等选择 suitable_elevator_id ...

      IF suitable_elevator_id IS NOT -1 THEN
        响应电梯 = &e[suitable_elevator_id]
        IF 响应电梯.当前楼层 IS NOT 当前乘客.起始楼层 THEN // 电梯与人不在同层
          响应电梯.停靠楼层[当前乘客.起始楼层] = 是
          IF 响应电梯.状态 IS 空闲 OR 响应电梯.状态 IS 泊车中 THEN
            响应电梯.状态 = 运行中
            更新电梯状态动画(suitable_elevator_id, "收到请求")
            响应电梯.方向 = 判断方向(响应电梯.当前楼层, 当前乘客.起始楼层)
            执行LOOK算法(响应电梯)
          END IF
        ELSE // 电梯与人在同层
          SWITCH 响应电梯.状态:
            CASE 泊车中, 空闲, 关门中:
              响应电梯.状态 = 开门中
              更新电梯状态动画(suitable_elevator_id, "开门中...")
              响应电梯.上次动作时间 = 当前时间
              插入事件(当前时间 + 开关门时间, 门已开事件, suitable_elevator_id, -1, 当前乘客.起始楼层) 到 事件链表
              BREAK
            CASE 开门状态, 服务中:
              IF (响应电梯.方向 == 乘客需求方向 OR 响应电梯.当前人数 + 响应电梯.预留人数 == 0) AND 响应电梯.当前人数 + 响应电梯.预留人数 < 最大容量 THEN
                IF 响应电梯.当前人数 + 响应电梯.预留人数 == 0 THEN
                  响应电梯.方向 = 乘客需求方向
                END IF
                响应电梯.状态 = 服务中
                更新电梯状态动画(suitable_elevator_id, "门已开")
                响应电梯.预留人数 = 响应电梯.预留人数 + 1
                当前乘客.状态 = 进入电梯中
                people[当前乘客.编号].状态 = 进入电梯中
                当前乘客.所在电梯编号 = 响应电梯.id
                people[当前乘客.编号].所在电梯编号 = 响应电梯.id

                // 计算乘客进入电梯的精确时间 (考虑已开门时间和之前乘客进出情况)
                // ... 逻辑省略，参考原代码 ...
                精确进入时间 = 计算得到的进入时间

                插入事件(精确进入时间 + 人进出时间, 乘客进入事件, 响应电梯.id, 当前乘客.编号, 响应电梯.当前楼层) 到 事件链表
                插入事件(精确进入时间 + 人进出时间 + 门检查间隔, 门检查事件, 响应电梯.id, -1, 响应电梯.当前楼层) 到 事件链表
              ELSE
                EXIT(10) // 逻辑错误
              END IF
              BREAK
            CASE 开门中:
              // 无需特殊处理，等待门已开事件
              BREAK
          END SWITCH
        END IF
      END IF
      生成新的乘客()
      BREAK

    CASE 电梯到达事件 (ELEVATOR_ARRIVAL):
      IF 当前电梯 IS NULL THEN EXIT(4) END IF
      IF 当前电梯.状态 IS NOT 运行中 AND 当前电梯.状态 IS NOT 泊车中 THEN BREAK END IF

      旧楼层 = 当前电梯.当前楼层
      当前电梯.当前楼层 = event.level
      当前电梯.上次动作时间 = 当前时间
      更新电梯位置动画(当前电梯.id, 旧楼层, 当前电梯.当前楼层, 当前电梯)
      判断是否停靠(当前电梯)
      BREAK

    CASE 门已开事件 (DOOR_OPENED):
      IF 当前电梯 IS NULL THEN EXIT(5) END IF

      当前电梯.状态 = 开门状态
      当前电梯.上次动作时间 = 当前时间
      当前电梯.空闲开始时间 = 当前时间
      更新电梯状态动画(当前电梯.id, "门已开")

      乘客出电梯指针 = 当前电梯.乘客列表.next
      等待队列指针 = queues[当前电梯.当前楼层][当前电梯.方向].front.next
      进出乘客标记方向 = 静止
      当前电梯.预留人数 = 0
      人进出计时器 = 当前时间
      有乘客活动标记 = 0

      // 处理下电梯乘客
      WHILE 乘客出电梯指针 IS NOT NULL:
        IF 乘客出电梯指针.乘客.目标楼层 == 当前电梯.当前楼层 THEN
          有乘客活动标记 = 1
          插入事件(人进出计时器 + 人进出时间, 乘客离开事件, 当前电梯.id, 乘客出电梯指针.乘客.编号, 当前电梯.当前楼层) 到 事件链表
          人进出计时器 = 人进出计时器 + 人进出时间
        END IF
        乘客出电梯指针 = 乘客出电梯指针.next
      END WHILE

      // 处理上电梯乘客 (更新电梯方向)
      // ... 逻辑省略，根据电梯内请求和外部等待队列更新电梯方向 (flag) ...

      // 如果方向改变，重置等待队列指针
      IF 进出乘客标记方向 IS NOT 静止 THEN
        等待队列指针 = queues[当前电梯.当前楼层][进出乘客标记方向].front.next
      END IF

      // 处理上电梯乘客 (加入事件)
      WHILE 进出乘客标记方向 IS NOT 静止 AND 等待队列指针 IS NOT NULL AND 当前电梯.当前人数 + 当前电梯.预留人数 < 最大容量:
        IF 人进出计时器 - 等待队列指针.乘客.到达时间 <= 等待队列指针.乘客.最大等待时间 AND 等待队列指针.乘客.状态 == 等待中 THEN
          IF 等待队列指针.乘客.所在电梯编号 IS NOT -1 THEN // 已被其他电梯预定
            等待队列指针 = 等待队列指针.next
            CONTINUE
          END IF
          有乘客活动标记 = 1
          插入事件(人进出计时器 + 人进出时间, 乘客进入事件, 当前电梯.id, 等待队列指针.乘客.编号, 当前电梯.当前楼层) 到 事件链表
          等待队列指针.乘客.状态 = 进入电梯中
          people[等待队列指针.乘客.编号].状态 = 进入电梯中
          等待队列指针.乘客.所在电梯编号 = 当前电梯.id
          people[等待队列指针.乘客.编号].所在电梯编号 = 当前电梯.id
          人进出计时器 = 人进出计时器 + 人进出时间
          当前电梯.预留人数 = 当前电梯.预留人数 + 1
        END IF
        等待队列指针 = 等待队列指针.next
      END WHILE

      IF 有乘客活动标记 THEN
        当前电梯.状态 = 服务中
      END IF
      插入事件(人进出计时器 + 门检查间隔, 门检查事件, 当前电梯.id, -1, 当前电梯.当前楼层) 到 事件链表
      BREAK

    CASE 门检查事件 (DOOR_CHECK):
      IF 当前电梯 IS NULL THEN EXIT(8) END IF
      IF 当前电梯.状态 IS NOT 开门状态 AND 当前电梯.状态 IS NOT 服务中 THEN BREAK END IF

      // 检查是否还有乘客未出电梯 (理论上不应发生)
      // ... 逻辑省略 ...

      IF 当前电梯.预留人数 == 0 THEN // 没有预留要进入的乘客了
        当前电梯.状态 = 关门中
        当前电梯.上次动作时间 = 当前时间
        更新电梯状态动画(当前电梯.id, "关门中...")
        插入事件(当前时间 + 开关门时间, 门已关事件, 当前电梯.id, -1, 当前电梯.当前楼层) 到 事件链表
      END IF
      BREAK

    CASE 门已关事件 (DOOR_CLOSED):
      IF 当前电梯 IS NULL THEN EXIT(13) END IF
      IF 当前电梯.状态 IS NOT 关门中 THEN BREAK END IF

      更新电梯状态动画(当前电梯.id, "门已关")

      IF 当前电梯.当前人数 IS NOT 0 THEN // 电梯内有人
        当前电梯.状态 = 运行中
        执行LOOK算法(当前电梯) // 根据内部请求决定方向和下一站
      ELSE // 电梯空了
        是否有需求 = 检查是否有楼层请求或等待队列(STATUS_FALSE)
        // ... 遍历停靠列表和所有楼层等待队列检查需求 ...
        IF 有需求 IS STATUS_TRUE THEN
          新方向 = 改变方向算法(当前电梯)
          IF 新方向 IS NOT 静止 THEN
            当前电梯.状态 = 运行中
            当前电梯.方向 = 新方向
            执行LOOK算法(当前电梯)
          ELSE
            当前电梯.状态 = 空闲
            当前电梯.方向 = 静止
            当前电梯.空闲开始时间 = 当前时间
            更新电梯状态动画(当前电梯.id, "门已关(停留)")
            插入事件(当前时间 + 自动指令时间, 空闲检查事件, 当前电梯.id, -1, 当前电梯.当前楼层) 到 事件链表
          END IF
        ELSE // 完全没有需求
          当前电梯.状态 = 空闲
          当前电梯.方向 = 静止
          当前电梯.空闲开始时间 = 当前时间
          更新电梯状态动画(当前电梯.id, "门已关(停留)")
          插入事件(当前时间 + 自动指令时间, 空闲检查事件, 当前电梯.id, -1, 当前电梯.当前楼层) 到 事件链表
        END IF
      END IF
      BREAK

    CASE 乘客进入事件 (PEOPLE_IN):
      IF 当前电梯 IS NULL OR 当前乘客 IS NULL THEN EXIT(14) END IF
      IF 当前乘客.状态 == 放弃 THEN
        当前电梯.预留人数 = 当前电梯.预留人数 - 1
        BREAK
      END IF
      IF 当前乘客.所在电梯编号 IS NOT 当前电梯.id THEN // 乘客上了别的电梯
        当前电梯.预留人数 = 当前电梯.预留人数 - 1
        BREAK
      END IF

      乘客需求方向 = 判断方向(当前乘客.起始楼层, 当前乘客.目标楼层)
      IF 从等待队列删除乘客(当前电梯.当前楼层, 乘客需求方向, 当前乘客.编号) IS 错误状态 THEN EXIT(11) END IF

      当前电梯.当前人数 = 当前电梯.当前人数 + 1
      当前电梯.预留人数 = 当前电梯.预留人数 - 1
      当前电梯.上次动作时间 = 当前时间
      当前乘客.状态 = 在电梯内
      将乘客信息插入电梯内乘客列表(当前电梯.乘客列表, 当前乘客)
      当前电梯.停靠楼层[当前乘客.目标楼层] = 是

      更新电梯人数动画(当前电梯.id, 当前电梯.当前楼层, 当前电梯)
      更新等待队列动画(当前电梯.当前楼层, 上行队列, 下行队列)
      更新乘客状态信息动画(格式化信息: 乘客编号, 当前楼层, 电梯ID)
      更新电梯状态动画(当前电梯.id, "上客中...")
      BREAK

    CASE 乘客离开事件 (PEOPLE_OUT):
      IF 当前电梯 IS NULL OR 当前乘客 IS NULL THEN EXIT(15) END IF
      IF 当前乘客.状态 == 放弃 THEN EXIT(16) END IF // 不应发生

      IF 从电梯内乘客列表删除乘客(当前电梯.乘客列表, 当前乘客.编号) IS 错误状态 THEN EXIT(6) END IF

      当前电梯.当前人数 = 当前电梯.当前人数 - 1
      当前电梯.上次动作时间 = 当前时间
      当前乘客.状态 = 已到达
      已到达人数 = 已到达人数 + 1

      更新电梯人数动画(当前电梯.id, 当前电梯.当前楼层, 当前电梯)
      更新乘客状态信息动画(格式化信息: 乘客编号, 当前楼层, 电梯ID)
      更新电梯状态动画(当前电梯.id, "下客中...")
      BREAK

    CASE 空闲检查事件 (IDLE_CHECK):
      IF 当前电梯 IS NULL THEN EXIT(17) END IF
      目标待命楼层 = (当前电梯.id == 0) ? 1 : LEVEL_NUM

      IF 当前电梯.状态 == 空闲 OR 当前时间 - 当前电梯.空闲开始时间 >= 自动指令时间 THEN
        IF 当前电梯.当前楼层 IS NOT 目标待命楼层 THEN
          当前电梯.状态 = 泊车中
          当前电梯.方向 = 判断方向(当前电梯.当前楼层, 目标待命楼层)
          更新电梯状态动画(当前电梯.id, "返回待机")
          执行LOOK算法(当前电梯)
        ELSE
          当前电梯.方向 = 静止
          当前电梯.状态 = 空闲
          更新电梯状态动画(当前电梯.id, "待机")
        END IF
      ELSE IF 当前电梯.状态 == 空闲 THEN // 未到自动指令时间，但仍空闲
        剩余等待时间 = 自动指令时间 - (当前时间 - 当前电梯.上次动作时间)
        插入事件(当前时间 + 剩余等待时间, 空闲检查事件, 当前电梯.id, -1, 当前电梯.当前楼层) 到 事件链表
      END IF
      BREAK

    CASE 乘客放弃事件 (PEOPLE_GIVE_UP):
      IF 当前乘客 IS NULL THEN EXIT(18) END IF
      IF 当前乘客.状态 == 等待中 THEN // 仅当仍在等待时才放弃
        当前乘客.状态 = 放弃
        放弃人数 = 放弃人数 + 1
        乘客需求方向 = 判断方向(当前乘客.起始楼层, 当前乘客.目标楼层)
        从等待队列删除乘客(当前乘客.起始楼层, 乘客需求方向, 当前乘客.编号)
        更新等待队列动画(当前乘客.起始楼层, 上行队列, 下行队列)
        更新乘客状态信息动画(格式化信息: 乘客编号, 起始楼层, "放弃等待")
      END IF
      BREAK

  END SWITCH
  RETURN 成功状态
END FUNCTION
```

## 函数: Is_Stop
**作用**: 判断电梯在当前楼层是否需要停下（开门）。

```pseudocode
FUNCTION Is_Stop(elevator):
  需要停靠 = 否
  // 检查是否有人要在此楼层下电梯
  FOR EACH person IN elevator.乘客列表:
    IF person.目标楼层 == elevator.当前楼层 THEN
      需要停靠 = 是
      BREAK
    END IF
  END FOR

  // 检查此楼层是否在电梯的停靠请求列表中
  IF elevator.停靠楼层[elevator.当前楼层] == 是 THEN
    需要停靠 = 是
  END IF

  // 特殊情况：泊车中且到达目标待命楼层
  IF elevator.状态 == 泊车中 AND ((elevator.id == 0 AND elevator.当前楼层 == 1) OR (elevator.id == 1 AND elevator.当前楼层 == LEVEL_NUM)) THEN
    需要停靠 = 是
  END IF

  IF 需要停靠 THEN
    elevator.停靠楼层[elevator.当前楼层] = 否 // 清除停靠请求
    elevator.状态 = 开门中
    更新电梯状态动画(elevator.id, "开门中...")
    elevator.上次动作时间 = 当前时间
    插入事件(当前时间 + 开关门时间, 门已开事件, elevator.id, -1, elevator.当前楼层) 到 事件链表
  ELSE
    // 如果不需要停靠，则继续按原方向行驶或改变方向
    执行LOOK算法(elevator)
  END IF
  RETURN 成功状态
END FUNCTION
```

## 函数: Look_Algorithm
**作用**: 电梯调度核心算法（LOOK算法），决定电梯的下一运行方向和目标楼层。

```pseudocode
FUNCTION Look_Algorithm(elevator):
  IF elevator.状态 == 空闲 OR elevator.状态 == 开门状态 OR elevator.状态 == 服务中 OR elevator.状态 == 关门中 OR elevator.状态 == 开门中 THEN
    RETURN // 这些状态下不应由LOOK算法直接驱动
  END IF

  next_target_level = -1

  // 检查当前方向上是否有请求 (电梯内或外部同向请求)
  IF elevator.方向 == 上升 THEN
    FOR level FROM elevator.当前楼层 + 1 TO LEVEL_NUM:
      IF elevator.停靠楼层[level] == 是 OR (获取楼层等待人数(level, 上升) > 0 AND elevator.当前人数 + elevator.预留人数 < 最大容量) THEN
        next_target_level = level
        BREAK
      END IF
    END FOR
  ELSE IF elevator.方向 == 下降 THEN
    FOR level FROM elevator.当前楼层 - 1 DOWNTO 1:
      IF elevator.停靠楼层[level] == 是 OR (获取楼层等待人数(level, 下降) > 0 AND elevator.当前人数 + elevator.预留人数 < 最大容量) THEN
        next_target_level = level
        BREAK
      END IF
    END FOR
  END IF

  IF next_target_level IS NOT -1 THEN // 当前方向有请求
    插入事件(当前时间 + abs(next_target_level - elevator.当前楼层) * 每层运行时间, 电梯到达事件, elevator.id, -1, next_target_level) 到 事件链表
    更新电梯状态动画(elevator.id, (elevator.方向 == 上升 ? "上行中" : "下行中"))
  ELSE // 当前方向无请求，尝试改变方向或处理其他情况
    新方向 = 改变方向算法(elevator)
    IF 新方向 IS NOT 静止 THEN
      elevator.方向 = 新方向
      // 再次使用LOOK逻辑（或简化版）寻找新方向上的目标
      IF elevator.方向 == 上升 THEN
        FOR level FROM elevator.当前楼层 + 1 TO LEVEL_NUM:
          IF elevator.停靠楼层[level] == 是 OR (获取楼层等待人数(level, 上升) > 0 AND elevator.当前人数 + elevator.预留人数 < 最大容量) THEN
            next_target_level = level
            BREAK
          END IF
        END FOR
      ELSE IF elevator.方向 == 下降 THEN
        FOR level FROM elevator.当前楼层 - 1 DOWNTO 1:
          IF elevator.停靠楼层[level] == 是 OR (获取楼层等待人数(level, 下降) > 0 AND elevator.当前人数 + elevator.预留人数 < 最大容量) THEN
            next_target_level = level
            BREAK
          END IF
        END FOR
      END IF
      IF next_target_level IS NOT -1 THEN
        插入事件(当前时间 + abs(next_target_level - elevator.当前楼层) * 每层运行时间, 电梯到达事件, elevator.id, -1, next_target_level) 到 事件链表
        更新电梯状态动画(elevator.id, (elevator.方向 == 上升 ? "上行中" : "下行中"))
      ELSE // 即使改变方向后也无请求 (理论上此时应变为空闲，由门关闭事件处理)
        IF elevator.状态 == 运行中 THEN // 如果之前是运行中，现在没活干了
          elevator.状态 = 空闲
          elevator.方向 = 静止
          elevator.空闲开始时间 = 当前时间
          更新电梯状态动画(elevator.id, "空闲")
          插入事件(当前时间 + 自动指令时间, 空闲检查事件, elevator.id, -1, elevator.当前楼层) 到 事件链表
        END IF
      END IF
    ELSE // 改变方向算法返回静止 (没有其他请求)
      IF elevator.状态 == 运行中 OR elevator.状态 == 泊车中 THEN
        elevator.状态 = 空闲
        elevator.方向 = 静止
        elevator.空闲开始时间 = 当前时间
        更新电梯状态动画(elevator.id, "空闲")
        插入事件(当前时间 + 自动指令时间, 空闲检查事件, elevator.id, -1, elevator.当前楼层) 到 事件链表
      END IF
    END IF
  END IF
  RETURN 成功状态
END FUNCTION
```

## 函数: Direction_Change
**作用**: 当电梯在一个方向上没有服务请求时，判断是否需要改变方向以响应其他请求。

```pseudocode
FUNCTION Direction_Change(elevator):
  // 检查电梯内部是否有反向请求
  IF elevator.方向 == 上升 THEN
    FOR level FROM elevator.当前楼层 - 1 DOWNTO 1:
      IF elevator.停靠楼层[level] == 是 THEN RETURN 下降 END IF
    END FOR
  ELSE IF elevator.方向 == 下降 THEN
    FOR level FROM elevator.当前楼层 + 1 TO LEVEL_NUM:
      IF elevator.停靠楼层[level] == 是 THEN RETURN 上升 END IF
    END FOR
  END IF

  // 检查所有楼层是否有外部请求
  // 优先检查与当前电梯位置相关的、能最快响应的请求
  // (此部分逻辑较为复杂，涉及全局扫描和距离判断，简化表示)
  最近上行请求楼层 = -1, 最近下行请求楼层 = -1
  最小上行距离 = 无穷大, 最小下行距离 = 无穷大

  FOR level FROM 1 TO LEVEL_NUM:
    IF 获取楼层等待人数(level, 上升) > 0 THEN
      IF abs(level - elevator.当前楼层) < 最小上行距离 THEN
        最小上行距离 = abs(level - elevator.当前楼层)
        最近上行请求楼层 = level
      END IF
    END IF
    IF 获取楼层等待人数(level, 下降) > 0 THEN
      IF abs(level - elevator.当前楼层) < 最小下行距离 THEN
        最小下行距离 = abs(level - elevator.当前楼层)
        最近下行请求楼层 = level
      END IF
    END IF
  END FOR

  IF elevator.方向 == 上升 THEN // 原方向是上
    IF 最近下行请求楼层 IS NOT -1 THEN RETURN 下降 END IF // 优先响应反向
    IF 最近上行请求楼层 IS NOT -1 THEN RETURN 上升 END IF // 如果没有反向，再看同向远处
  ELSE IF elevator.方向 == 下降 THEN // 原方向是下
    IF 最近上行请求楼层 IS NOT -1 THEN RETURN 上升 END IF
    IF 最近下行请求楼层 IS NOT -1 THEN RETURN 下降 END IF
  ELSE // 原方向是静止 (通常在电梯空闲时调用)
    IF 最小上行距离 <= 最小下行距离 AND 最近上行请求楼层 IS NOT -1 THEN
      RETURN 上升
    ELSE IF 最小下行距离 < 最小上行距离 AND 最近下行请求楼层 IS NOT -1 THEN
      RETURN 下降
    ELSE IF 最近上行请求楼层 IS NOT -1 THEN
      RETURN 上升
    ELSE IF 最近下行请求楼层 IS NOT -1 THEN
      RETURN 下降
    END IF
  END IF

  RETURN 静止 // 没有找到任何请求
END FUNCTION
```

## 函数: Judge_Direction
**作用**: 根据起始楼层和目标楼层判断乘客或电梯的移动方向。

```pseudocode
FUNCTION Judge_Direction(start_level, end_level):
  IF end_level > start_level THEN
    RETURN 上升
  ELSE IF end_level < start_level THEN
    RETURN 下降
  ELSE
    RETURN 静止
  END IF
END FUNCTION
```

## 函数: Distance
**作用**: 计算两个楼层之间的距离。

```pseudocode
FUNCTION Distance(level1, level2):
  RETURN abs(level1 - level2)
END FUNCTION
```

## 函数: Get_Random
**作用**: 生成指定范围内的随机数。

```pseudocode
FUNCTION Get_Random(min_val, max_val):
  RETURN (rand() % (max_val - min_val + 1)) + min_val
END FUNCTION
```

## 函数: Stat
**作用**: 统计并打印模拟运行的最终数据，如总运行时间、到达人数、放弃人数等。

```pseudocode
FUNCTION Stat():
  打印 "模拟结束"
  打印 "总运行时间: ", 当前时间
  打印 "总生成人数: ", 已生成人数
  打印 "成功到达人数: ", 已到达人数
  打印 "放弃等待人数: ", 放弃人数
  // 可以添加更多统计信息，如平均等待时间等
END FUNCTION
```

## 函数: Simulation
**作用**: 主模拟循环，不断获取并处理事件，直到事件链表为空或达到最大人数。

```pseudocode
FUNCTION Simulation():
  current_event = 事件链表.next // 获取第一个事件
  生成新的乘客() // 产生初始乘客

  WHILE current_event IS NOT NULL AND 已到达人数 + 放弃人数 < MAX_PEOPLE:
    处理事件(current_event)
    删除事件(事件链表, current_event) // 处理完后从链表删除
    current_event = 获取下一个事件(事件链表.next) // 获取下一个事件，注意这里应为事件链表的头部的下一个，因为current_event已被删除
                                          // 或者 Get_Next_Event(事件链表.next) 如果事件链表是带头结点的
  END WHILE
  RETURN 成功状态
END FUNCTION
```