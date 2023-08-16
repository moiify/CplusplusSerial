/**
  * @file sbus_handle_msg.h
  * @brief 定义SBUS无线手柄按键和摇杆数据结构，用户指令枚举类，以及归一化函数
  * @details 
  * @author david
  * @date  2021.12.28
  * @version 1.0
  * @copyright Copyright (c) 2021, Unitree Robotics.Co.Ltd. All rights reserved.
  */
#ifndef SBUS_HANDLE_MSG_H
#define SBUS_HANDLE_MSG_H
#include <stdint.h>

// 手柄按键结构体
struct KEY{
    bool A    ;
    bool B    ;
    bool C    ;
    bool D    ;
    bool L1   ;
    bool L2   ;
    bool R1   ;
    bool R2   ;
    bool START;
    bool STARTx2;
    bool START_RST;
    bool SELECT;
    char M;
    KEY(){
        setKeyFalse();
    }
    void setKeyFalse(){
        A=false ;
        B=false ;
        C=false ;
        D=false ;
        L1=false;
        L2=false;
        R1=false;
        R2=false;
        START=false;
        STARTx2=false;
        START_RST=false;
        SELECT=false;
        M=1;
    }
};

// 手柄结构体（包含按键和摇杆变量）
struct HANDLE {
    float ry;   // Z轴旋转-yawSpeed:[1.0,-1.0]    / Z轴原地旋转-euler[2]:[0.6,-0.6]     //通道1 右手左右打  移动时:  Move  z vyaw  [-0.6~0.6]
    float rx;   // X轴旋转-euler[1]:[-0.4,0.4]    / Y轴原地旋转-euler[1]:[-0.4,0.4]     //通道2 右手上下打  移动时:  Euler y pitch [-0.75~0.75]
    float lx;   // X轴移动-velocity[0]:[-0.9,1.0] / Z轴原地移动-bodyHeight:[-0.15,0.05] //通道3 左手上下打  移动时:  Move  x vx    [-2.5~5]
    float ly;   // Y轴移动-velocity[1]:[1.0,-1.0] / X轴原地旋转-euler[0]:[-0.75,0.05]   //通道4 左手左右打  移动时:  Move  y vy    [-1~1]

    KEY key;
    HANDLE(){
        setJoystickZero();
    }
    void setJoystickZero(){
        ry = 0.0;
        rx = 0.0;
        lx = 0.0;
        ly = 0.0;
    }
};

// // 用户指令枚举类
// enum class UserCommand{
//     NONE,
//     D1,     // 力控站立                            
//     D2,     // 移动                              
//     D3,     // 持续移动                             
//     A,      // 爬楼梯 (当状态为持续移动时)       
//     B,      // 恢复移动状态 (当状态为爬楼梯时)  
//     C1,     // 避障 (当状态为持续移动时)   
//     C2,     // 关闭避障 (当状态为避障时)
//     L2_A1,  // 位控卧倒     
//     L2_A2,  // 位控站立
//     L2_B,   // 阻尼状态                             
//     L2_C,   // 恢复力控站立                           
//     L2_D,   // 小跑移动
// };

enum class UserCommand{
    NONE,
    // L1_START,   // 解锁狗
    L2_A1,      // 锁定关节 ， 并进入卧倒姿态
    L2_A2,      // 恢复站立姿态 (关节锁定)
    L2_B,       // 阻尼状态
    L2_C,       // 恢复力控站立 
    MODE2,      // 移动模式                                                       
    MODE3,      // 持续移动模式
    AVOID_ON,     // 避障 (当状态为持续移动时)   
    AVOID_OFF,     // 关闭避障 (当状态为避障时)
    STOPMOVE,     //停下当前动作，将绝大多数指令恢复成默认值

    SPEED_SLOW,     // 设置移动速度 慢       
    SPEED_NORMAL,   // 设置移动速度 正常
    SPEED_FAST,     // 设置移动速度 快

    A,      // 爬楼梯 (当状态为持续移动时)       
    B,      // 恢复移动状态 (当状态为爬楼梯时)
    L2_D,   // 小跑移动
};

// 归一化函数
#define DEAD_ZONE (200)

template<typename T0, typename T1>
inline T0 normalize(const int value, const T0 minLim, const T1 maxLim, const int min = -1, const int max = 1){
    double ret = 0.00;
    if(value < (992 - DEAD_ZONE)){
        ret =  (value-992+DEAD_ZONE)*(0-minLim)/(992-min-DEAD_ZONE);
    }else if (value > (992 + DEAD_ZONE)){
        ret =  (value-992-DEAD_ZONE)*(maxLim-0)/(max-992-DEAD_ZONE);
    }else{
        return 0.00;
    }
    return static_cast<double>(static_cast<int>(ret * 100)) / 100;
}

#endif