/**
  * @file rc.cpp
  * @brief SBUS解析后,利用dds通信控制go2
  * @details 
  * @author neoxin
  * @date  2023.8.2
  * @version 1.0
  * @copyright Copyright (c) 2023, Unitree Robotics.Co.Ltd. All rights reserved.
  */
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "sbus_handle.h"

#include "cJSON.h"

#include "dds/dds_easy_model.hpp"
#include <go2_idl/Request_.hpp>
#include <go2_idl/Response_.hpp>


std::string dds_sportTopic[2] ={
    "rt/api/sport/request",
    "rt/api/sport/response",
};

std::string dds_obstTopic[2] ={
    "rt/api/obstacles_avoid/request",
    "rt/api/obstacles_avoid/response",
};

std::string dev_name = "/dev/ttyS0";  // 串口名称
unitree::common::DdsEasyModel dm;

class RControl
{
public:
    RControl(): sbusHandle(RUNNING)
    {

    };
    void RobotControl();
    float dt = 0.01;     // 0.001~0.01
    SbusHandle sbusHandle;
private:

    bool RUNNING = true;
};


// float ry;   // Z轴旋转-yawSpeed:[1.0,-1.0]    / Z轴原地旋转-euler[2]:[0.6,-0.6]     //通道1 右手左右打  移动时:  Move  z vyaw  [-0.6~0.6]
// float rx;   // X轴旋转-euler[1]:[-0.4,0.4]    / Y轴原地旋转-euler[1]:[-0.4,0.4]     //通道2 右手上下打  移动时:  Euler y pitch [-0.75~0.75]
// float lx;   // X轴移动-velocity[0]:[-0.9,1.0] / Z轴原地移动-bodyHeight:[-0.15,0.05] //通道3 左手上下打  移动时:  Move  x vx    [-2.5~5]
// float ly;   // Y轴移动-velocity[1]:[1.0,-1.0] / X轴原地旋转-euler[0]:[-0.75,0.05]   //通道4 左手左右打  移动时:  Move  y vy    [-1~1]


void dds_vuiApiReq_xyz(std::string topic, int64_t id, int64_t api_id, float vx_f, float vy_f, float vz_f)
{
    unitree_api::msg::dds_::Request_ reqMsg;

    reqMsg.header().identity().id(id);
    reqMsg.header().identity().api_id(api_id);

    cJSON* jsonObj = cJSON_CreateObject();

    cJSON_AddNumberToObject(jsonObj, "x", vx_f);
    cJSON_AddNumberToObject(jsonObj, "y", vy_f);
    cJSON_AddNumberToObject(jsonObj, "z", vz_f);
    reqMsg.parameter() = cJSON_PrintUnformatted(jsonObj);
    cJSON_Delete(jsonObj);
    dm.WriteMessage<unitree_api::msg::dds_::Request_>(topic, reqMsg);
}

void dds_vuiApiReq(std::string topic, int64_t id, int64_t api_id, const char * jsonName, int jsonValue)
{
    unitree_api::msg::dds_::Request_ reqMsg;

    reqMsg.header().identity().id(id);
    reqMsg.header().identity().api_id(api_id);

    if(jsonName)
    {
        cJSON* jsonObj = cJSON_CreateObject();
        cJSON_AddNumberToObject(jsonObj, (char *)jsonName, jsonValue);
        reqMsg.parameter() = cJSON_PrintUnformatted(jsonObj);
        cJSON_Delete(jsonObj);
    }

    dm.WriteMessage<unitree_api::msg::dds_::Request_>(topic, reqMsg);
}

#define KP   0.02  //增大
#define KP_R 0.1    //回中
double nonlinearFunction(double input, unsigned char vtype) {
    static double ot[4] = {0.0,0.0,0.0,0.0};
    if(ot[vtype] == input) return ot[vtype]; //减少CPU负载

    if(input == 0.0) {
        ot[vtype] = 0.0;
    } else if ((input > 0 && input < ot[vtype]) || (input <0 && input > ot[vtype])){
        // ot[vtype] = input;
        ot[vtype] = ot[vtype] + (( input - ot[vtype]) * KP_R);
    } else {

        ot[vtype] = ot[vtype] + (( input - ot[vtype]) * KP);
    }
    return ot[vtype];
}

void RControl::RobotControl() 
{
    static double prlx,prly,prrx,prry = 0.00;
    double nlx,nly,nrx,nry = 0.00;
    switch(sbusHandle._userCmd)
    {
        case UserCommand::L2_A1:
        {
            dds_vuiApiReq(dds_sportTopic[0], getSystemTime(), 1005, NULL, 0);
        }
        break;
        case UserCommand::L2_A2:
        {
            dds_vuiApiReq(dds_sportTopic[0], getSystemTime(), 1004, NULL, 0);
        }
        break;
        case UserCommand::MODE2:
        {
            dds_vuiApiReq(dds_sportTopic[0], getSystemTime(), 1002, NULL, 0);
        }
        break;
        case UserCommand::L2_B:
        {
            dds_vuiApiReq(dds_sportTopic[0], getSystemTime(), 1001, NULL, 0);
        }
        break;
        case UserCommand::AVOID_ON:
        {
            std::string estr = "enable";
            dds_vuiApiReq(dds_obstTopic[0], getSystemTime(), 1001, estr.c_str(), 1);
        }
        break;        
        case UserCommand::AVOID_OFF:
        {
            std::string estr = "enable";
            dds_vuiApiReq(dds_obstTopic[0], getSystemTime(), 1001, estr.c_str(), 0);
        }
        break;        
        case UserCommand::L2_C:
        {
            dds_vuiApiReq(dds_sportTopic[0], getSystemTime(), 1006, NULL, 0);
        }
        break;
        case UserCommand::STOPMOVE:
        {
            dds_vuiApiReq(dds_sportTopic[0], getSystemTime(), 1003, NULL, 0);
        }
        break;
        default:
        break;
    }
    sbusHandle._userCmd = UserCommand::NONE;


    //运动模式
    // if(!sbusHandle.lock)
    if(1)
    {
        nlx = nonlinearFunction(sbusHandle._handleCmd.lx, 0);
        nly = nonlinearFunction(sbusHandle._handleCmd.ly, 1);
        nrx = nonlinearFunction(sbusHandle._handleCmd.rx, 2);
        nry = nonlinearFunction(sbusHandle._handleCmd.ry, 3);

        if(nlx != prlx || nly != prly || nry != prry || nlx!= 0|| nly!= 0|| nry!= 0)
        {
            dds_vuiApiReq_xyz(dds_sportTopic[0], getSystemTime(), 1008, nlx, nly, nry); //Move
        }

        if(nrx != prrx|| nrx!= 0) dds_vuiApiReq_xyz(dds_sportTopic[0], getSystemTime(), 1007, 0.0, nrx, 0.0); //Euler
       
        if(nlx != prlx || nly != prly || nrx != prrx || nry != prry) {
            printf("_handleCmd: lx:%f ly:%f rx:%f ry:%f\n", sbusHandle._handleCmd.lx, sbusHandle._handleCmd.ly, sbusHandle._handleCmd.rx, sbusHandle._handleCmd.ry);
            printf(" nonlinear: lx:%f ly:%f rx:%f ry:%f\n", nlx, nly, nrx, nry);
        }
        
        prlx = nlx;
        prly = nly;
        prrx = nrx;
        prry = nry;
        
    }
    
}

void dds_obstRespHandler(const void* message)       
{
    const unitree_api::msg::dds_::Response_* pmsg = (const unitree_api::msg::dds_::Response_*)message;
    std::cout << "dds_obstRespHandler : [ id:" << pmsg->header().identity().id() << " - api_id:" << pmsg->header().identity().api_id() << " - data: " << pmsg->data() << "]" << std::endl;
}

void dds_sportRespHandler(const void* message)       
{
//     const unitree_api::msg::dds_::Response_* pmsg = (const unitree_api::msg::dds_::Response_*)message;
//     std::cout << "dds_sportRespHandler : [ id:" << pmsg->header().identity().id() << " - api_id:" << pmsg->header().identity().api_id() << " - data: " << pmsg->data() << "]" << std::endl;
}

long long startTime;    // 每次循环起始时刻
long long costTime;     // 每次循环耗费时间

int main(void) {
    RControl rc;

    rc.sbusHandle.open_set(dev_name.c_str());

    // LoopFunc loop_control("control_loop", rc.dt,  boost::bind(&Custom::RobotControl, &custom));

    dm.Init();
    //运控
    dm.SetTopic<unitree_api::msg::dds_::Request_>(dds_sportTopic[0]);
    dm.SetTopic<unitree_api::msg::dds_::Response_>(dds_sportTopic[1], std::bind(dds_sportRespHandler, std::placeholders::_1));

    //避障
    dm.SetTopic<unitree_api::msg::dds_::Request_>(dds_obstTopic[0]);
    dm.SetTopic<unitree_api::msg::dds_::Response_>(dds_obstTopic[1], std::bind(dds_obstRespHandler, std::placeholders::_1));

    // std::string welcomeMessage = "Unitree";
    // std::string command = "toilet -F gay " + welcomeMessage;
    // int result = system(command.c_str());
    // if (result != 0) {
    //     std::cerr << "Failed to print welcome message using toilet." << std::endl;
    //     return 1;
    // }
    std::cout << "Compilation date: " << __DATE__ << std::endl;
    std::cout << "Compilation time: " << __TIME__ << std::endl;
    std::cout << "Author: neoxin" << std::endl;

    
    while(1){
        // printf("-----------------------\n");
        // startTime = getSystemTime();
        rc.sbusHandle.data_align();         // 数据对齐
		rc.sbusHandle.sbus_receive();       // 接收串口并解码数据
        rc.sbusHandle.channel_command();    // 将通道值转化为用户指令， Custom::RobotControl()将用户指令转化为机器人指令

        rc.RobotControl();
        // 打印调试信息

        // printf("_handleCmd: A:%d B:%d C:%d D:%d L1:%d L2:%d R1:%d R2:%d START:%d SELECT%d STARTx2:%d START_RESET:%d M:%d\n", rc.sbusHandle._handleCmd.key.A,rc.sbusHandle._handleCmd.key.B,rc.sbusHandle._handleCmd.key.C,rc.sbusHandle._handleCmd.key.D, 
        // rc.sbusHandle._handleCmd.key.L1,rc.sbusHandle._handleCmd.key.L2,rc.sbusHandle._handleCmd.key.R1,rc.sbusHandle._handleCmd.key.R2,rc.sbusHandle._handleCmd.key.START,rc.sbusHandle._handleCmd.key.SELECT,rc.sbusHandle._handleCmd.key.STARTx2,rc.sbusHandle._handleCmd.key.START_RST,rc.sbusHandle._handleCmd.key.M);

		// costTime = getSystemTime()-startTime;
		// printf("cost time:%lld\n",costTime);
        usleep(20);
    };
    return 0;
}