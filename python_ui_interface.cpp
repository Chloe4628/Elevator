// python_ui_interface.cpp
// 用于连接C++电梯模拟程序与Python Tkinter UI界面

#include <Python.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "base.h"
#include "animation.h"

// Python模块名称
static const char* PYTHON_MODULE = "elevator_ui";
static PyObject* pModule = NULL;

// 初始化Python解释器和加载模块
static int init_python_interface() {
    if (Py_IsInitialized()) {
        return STATUS_OK; // 已经初始化
    }
    
    Py_Initialize();
    if (!Py_IsInitialized()) {
        fprintf(stderr, "Python解释器初始化失败\n");
        return STATUS_ERROR;
    }
    
    // 添加当前目录到Python路径
    PyRun_SimpleString("import sys; sys.path.append('.')");
    
    // 导入模块
    pModule = PyImport_ImportModule(PYTHON_MODULE);
    if (pModule == NULL) {
        PyErr_Print();
        fprintf(stderr, "无法导入Python模块: %s\n", PYTHON_MODULE);
        Py_Finalize();
        return STATUS_ERROR;
    }
    
    return STATUS_OK;
}

// 清理Python资源
static void cleanup_python_interface() {
    if (pModule != NULL) {
        Py_DECREF(pModule);
        pModule = NULL;
    }
    
    if (Py_IsInitialized()) {
        Py_Finalize();
    }
}

// 调用Python函数的通用方法
static PyObject* call_python_function(const char* func_name, PyObject* args) {
    if (pModule == NULL) {
        if (init_python_interface() != STATUS_OK) {
            return NULL;
        }
    }
    
    PyObject* pFunc = PyObject_GetAttrString(pModule, func_name);
    if (pFunc == NULL || !PyCallable_Check(pFunc)) {
        PyErr_Print();
        fprintf(stderr, "无法找到Python函数: %s\n", func_name);
        Py_XDECREF(pFunc);
        return NULL;
    }
    
    PyObject* pResult = PyObject_CallObject(pFunc, args);
    Py_DECREF(pFunc);
    
    if (pResult == NULL) {
        PyErr_Print();
        fprintf(stderr, "调用Python函数失败: %s\n", func_name);
    }
    
    return pResult;
}

// 以下是对应animation.cpp中函数的Python接口实现

// 显示标题动画
void Ani_Show_Title() {
    PyObject* pResult = call_python_function("ani_show_title", PyTuple_New(0));
    Py_XDECREF(pResult);
}

// 初始化动画
void Ani_Init() {
    PyObject* pResult = call_python_function("ani_init", PyTuple_New(0));
    Py_XDECREF(pResult);
}

// 更新时间
void Ani_Update_Time(Time_Type ct) {
    PyObject* pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(ct));
    
    PyObject* pResult = call_python_function("ani_update_time", pArgs);
    
    Py_DECREF(pArgs);
    Py_XDECREF(pResult);
}

// 更新电梯位置
void Ani_Update_Elevator_Position(int id, Level_Type of, Level_Type nf, Elevator *e) {
    PyObject* pArgs = PyTuple_New(4);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(id));
    PyTuple_SetItem(pArgs, 1, PyLong_FromLong(of));
    PyTuple_SetItem(pArgs, 2, PyLong_FromLong(nf));
    
    // 创建一个表示电梯的Python字典对象
    PyObject* pElevator = PyDict_New();
    PyDict_SetItemString(pElevator, "current_people", PyLong_FromLong(e->current_people));
    PyDict_SetItemString(pElevator, "reserved_people", PyLong_FromLong(e->reserved_people));
    PyDict_SetItemString(pElevator, "direction", PyLong_FromLong(e->direction));
    PyDict_SetItemString(pElevator, "status", PyLong_FromLong(e->status));
    PyDict_SetItemString(pElevator, "current_level", PyLong_FromLong(e->current_level));
    
    // 创建乘客列表
    PyObject* pPeopleList = PyList_New(0);
    PL_Node* p = e->peoplelist->next;
    while (p) {
        PyObject* pPerson = PyDict_New();
        PyDict_SetItemString(pPerson, "pnum", PyLong_FromLong(p->people.pnum));
        PyDict_SetItemString(pPerson, "pstart_level", PyLong_FromLong(p->people.pstart_level));
        PyDict_SetItemString(pPerson, "pend_level", PyLong_FromLong(p->people.pend_level));
        PyList_Append(pPeopleList, pPerson);
        Py_DECREF(pPerson);
        p = p->next;
    }
    PyDict_SetItemString(pElevator, "people_list", pPeopleList);
    Py_DECREF(pPeopleList);
    
    PyTuple_SetItem(pArgs, 3, pElevator);
    
    PyObject* pResult = call_python_function("ani_update_elevator_position", pArgs);
    
    Py_DECREF(pArgs);
    Py_XDECREF(pResult);
}

// 更新电梯内乘客
void Ani_Update_Elevator_People(int id, Level_Type cf, Elevator *e) {
    PyObject* pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(id));
    PyTuple_SetItem(pArgs, 1, PyLong_FromLong(cf));
    
    // 创建一个表示电梯的Python字典对象
    PyObject* pElevator = PyDict_New();
    PyDict_SetItemString(pElevator, "current_people", PyLong_FromLong(e->current_people));
    
    // 创建乘客列表
    PyObject* pPeopleList = PyList_New(0);
    PL_Node* p = e->peoplelist->next;
    while (p) {
        PyObject* pPerson = PyDict_New();
        PyDict_SetItemString(pPerson, "pnum", PyLong_FromLong(p->people.pnum));
        PyDict_SetItemString(pPerson, "pend_level", PyLong_FromLong(p->people.pend_level));
        PyList_Append(pPeopleList, pPerson);
        Py_DECREF(pPerson);
        p = p->next;
    }
    PyDict_SetItemString(pElevator, "people_list", pPeopleList);
    Py_DECREF(pPeopleList);
    
    PyTuple_SetItem(pArgs, 2, pElevator);
    
    PyObject* pResult = call_python_function("ani_update_elevator_people", pArgs);
    
    Py_DECREF(pArgs);
    Py_XDECREF(pResult);
}

// 更新电梯状态
void Ani_Update_Elevator_Status(int id, const char *msg) {
    PyObject* pArgs = PyTuple_New(2);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(id));
    PyTuple_SetItem(pArgs, 1, PyUnicode_DecodeUTF8(msg, strlen(msg), "strict"));
    
    PyObject* pResult = call_python_function("ani_update_elevator_status", pArgs);
    
    Py_DECREF(pArgs);
    Py_XDECREF(pResult);
}

// 更新等待队列
void Ani_Update_Waiting_Queue(Level_Type floor, LQ_Queue *uq, LQ_Queue *dq) {
    PyObject* pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, PyLong_FromLong(floor));
    
    // 创建上行队列
    PyObject* pUpQueue = PyList_New(0);
    LQ_Node* p = uq->front->next;
    while (p) {
        PyObject* pPerson = PyDict_New();
        PyDict_SetItemString(pPerson, "pnum", PyLong_FromLong(p->people.pnum));
        PyDict_SetItemString(pPerson, "pstart_level", PyLong_FromLong(p->people.pstart_level));
        PyDict_SetItemString(pPerson, "pend_level", PyLong_FromLong(p->people.pend_level));
        PyList_Append(pUpQueue, pPerson);
        Py_DECREF(pPerson);
        p = p->next;
    }
    PyTuple_SetItem(pArgs, 1, pUpQueue);
    
    // 创建下行队列
    PyObject* pDownQueue = PyList_New(0);
    LQ_Node* q = dq->front->next;
    while (q) {
        PyObject* pPerson = PyDict_New();
        PyDict_SetItemString(pPerson, "pnum", PyLong_FromLong(q->people.pnum));
        PyDict_SetItemString(pPerson, "pstart_level", PyLong_FromLong(q->people.pstart_level));
        PyDict_SetItemString(pPerson, "pend_level", PyLong_FromLong(q->people.pend_level));
        PyList_Append(pDownQueue, pPerson);
        Py_DECREF(pPerson);
        q = q->next;
    }
    PyTuple_SetItem(pArgs, 2, pDownQueue);
    
    PyObject* pResult = call_python_function("ani_update_waiting_queue", pArgs);
    
    Py_DECREF(pArgs);
    Py_XDECREF(pResult);
}

// 更新通用状态信息
void Ani_Update_General_Person_Status(const char *msg) {
    PyObject* pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_DecodeUTF8(msg, strlen(msg), "strict"));
    
    PyObject* pResult = call_python_function("ani_update_general_person_status", pArgs);
    
    Py_DECREF(pArgs);
    Py_XDECREF(pResult);
}

// Debug显示函数A
void Ani_Debug_A(const char *msg) {
    // 在Python UI中可以忽略或重定向到控制台
    printf("Debug A: %s\n", msg);
}

// Debug显示函数B
void Ani_Debug_B(const char *msg) {
    // 在Python UI中可以忽略或重定向到控制台
    printf("Debug B: %s\n", msg);
}

// 清理资源
void Ani_Clean_Up() {
    PyObject* pResult = call_python_function("ani_clean_up", PyTuple_New(0));
    Py_XDECREF(pResult);
    
    // 清理Python接口
    cleanup_python_interface();
    
    printf("模拟结束\n");
}