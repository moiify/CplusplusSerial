/**
  * @file sbus_handle.h
  * @brief 打开、关闭以及设置串口，接收串口数据并对齐，解码串口数据，将通道值转化为机器狗指令
  * @details 
  * @author david
  * @date  2021.12.28
  * @version 1.0
  * @copyright Copyright (c) 2021, Unitree Robotics.Co.Ltd. All rights reserved.
  */
#ifndef SBUS_HANDLE_H
#define SBUS_HANDLE_H
#include "sbus_handle_msg.h"
// #include "unitree_legged_sdk/comm.h"

// using namespace UNITREE_LEGGED_SDK;

long long getSystemTime();  // 获得系统时间

class SbusHandle
{
public:
    SbusHandle(bool &RUNNING);
    int open_set(const char* dev_name); // 打开并设置串口
    void close_port();      // 关闭串口
    int sbus_receive();     // 接收串口数据
    void channel_command(); // 将通道值转化为机器狗指令
    void data_align();      // 串口数据对齐
    
    bool ALIGN = false;     // 用于判断串口数据是否对齐
    bool &RUNNING;          // 用于判断程序是否运行
    int _direction=1;        // 用于解决mode=2时原地踏步问题
    UserCommand _userCmd;   // 用户指令
    HANDLE _handleCmd;       // 按键指令
    
    int locomotionORclimb; //用于区分爬楼梯还是
    bool stopWhenlocomotion; // 用于在运动状态下是否停下来
    bool lock  = true;
private:
    int fd;                 // 串口文件标识符
    int _readLen;           // 读取到的字节长度
    uint8_t _buff1[1];      // 用于对齐手柄数据 
    uint8_t _buff24[24];    // 用于对齐手柄数据 
    uint8_t _buff25[25];	  // 接收完整手柄数据

    // HighState &_state;      // 接收机器狗状态
    uint16_t _CH[16];  	    // 通道值 初始值：992 992 992 992 272 272 272 272 992 992 992 992 992 992 992 992
    uint16_t _CHBuff[16];   // 通道值buff，解决复用按键状态切换过快问题
    
    void sbus_decode();     // 解码串口数据

    bool testprintf = false;
    bool mode1Tmode2 = true;
    bool stand_l = false;
    bool climb  = false;
};





#endif