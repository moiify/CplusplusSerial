/**
  * @file sbus_handle.cpp
  * @brief 打开、关闭以及设置串口，接收串口数据并对齐，解码串口数据，将通道值转化为用户指令
  * @details 
  * @author david
  * @date  2021.12.28
  * @version 1.0
  * @copyright Copyright (c) 2021, Unitree Robotics.Co.Ltd. All rights reserved.
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

#include <iostream>
#include "sbus_handle.h"

// 获得系统时间函数
long long getSystemTime() {  
    struct timeval t;  
    gettimeofday(&t, NULL);
    return 1000000 * t.tv_sec + t.tv_usec;  
}

// 手柄指令置零构造函数
SbusHandle::SbusHandle(bool &RUNNING):
    RUNNING(RUNNING)
{  
    _handleCmd.key.setKeyFalse(); 
    _handleCmd.setJoystickZero();
    _userCmd = UserCommand::NONE;

    locomotionORclimb = 0;
    stopWhenlocomotion = false;
}

// 打开并设置串口函数
int SbusHandle::open_set(const char* dev_name){
    int res;
    struct termios options;     // 串口配置结构体
    struct serial_struct sc;

    fd = open(dev_name, O_RDWR | O_NOCTTY );
    if(fd < 0){perror("open serial error"); exit(-1);}

    bzero(&options, sizeof(options));

    //波特率
    //Linux 不支持原生4.8M波特率，首先设置为原生38400波特率（必须38400，原因尚且不明）
    tcgetattr(fd, &options);        // 获取当前位置
    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);
    tcsetattr(fd, TCSANOW, &options);

    res = ioctl(fd, TIOCGSERIAL, &sc);
    if (res < 0)
    {
        perror("Ioctl error on getting serial configure");
        close(fd);
        exit(-1);
    }
    //根据基波特率修改
    //例如ft232h芯片的基波特率为24M，令custom divisor=240， 24/240=0.1
    sc.flags |= ASYNC_SPD_CUST;
    sc.flags |= ASYNC_LOW_LATENCY;
    sc.custom_divisor = (sc.baud_base/100000);

    if((ioctl(fd,TIOCSSERIAL,&sc))<0){
        perror("BAUD: error to set serial_struct%s\n");
        exit(-1);
    }

    options.c_cflag |= (CLOCAL | CREAD);            // 设置波特率，本地连接，接收使能
    options.c_cflag &= ~CSIZE;                      // 屏蔽数据位
    options.c_cflag |= CS8;                         // 数据位为8，CS7 for 7
    options.c_cflag |= PARENB;                      // 无校验位:&= ~PARENB，有校验位时：|= PARENB;
    options.c_cflag &= ~PARODD;                     // 偶校验
    options.c_cflag |= CSTOPB;                      // 一位停止位为 &= ~CSTOPB;，两位停止为 |= CSTOPB
    options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
    options.c_iflag |= IGNPAR;
    options.c_iflag &= ~INPCK;
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(IXON|IXOFF|IXANY);
    // cfsetispeed(&options, B115200);
    // cfsetospeed(&options, B115200);
    options.c_cc[VTIME] = 0;    // 等待时间，单位百毫秒 （读）。
    options.c_cc[VMIN] = 25;    //阻塞模式，接收25字节才结束
    // options.c_cc[VMIN] = 0;    //非阻塞模式

    //刷新发送和收取的缓存
    tcflush(fd, TCIOFLUSH);
    tcsetattr(fd, TCSANOW, &options);

    return fd;
}

// 关闭串口函数
void SbusHandle::close_port(){
    close(fd);
}

// 接收串口并解码数据函数
int SbusHandle::sbus_receive(){
	_readLen = read(fd, _buff25, 25);
    // printf("%d ", _readLen);
    // for(int i=0; i<_readLen; i++){
    //     printf("%02X ", _buff25[i]);
    // }
    // printf("readLen:%d\n",_readLen);
    if(_readLen != 25){
        printf("[ERROR] The received data length %d(25) is wrong！\n", _readLen);
        for(int i=0; i<_readLen; i++){
            printf("%02X ", _buff25[i]);
        }
        printf("\n");
        ALIGN = false;
        return -1;
    }
    else if((_buff25[0]==0x0f) && (_buff25[24]==0x00)){
        // printf("succeeded!\n");
        // for(int i=0; i<_readLen; i++){
        //     printf("%02X ", _buff25[i]);
        // }
        sbus_decode();
        // for(int i=0; i<16; i++){
        //     printf("%d ", _CH[i]);
        // }
        // printf("\n");
        return 0;
    }
    else{
        printf("[ERROR] The receUNITREE_LEGGED_SDK i++");
        // {
        //     printf("%02X ", _buff25[i]);
        // }
        printf("\n");
        ALIGN = false;
        return -1;
    }

    return 0;
}

// 解码串口数据函数
void SbusHandle::sbus_decode(){
	_CH[ 0] = ((int16_t)_buff25[ 1] >> 0 | ((int16_t)_buff25[ 2] << 8 )) & 0x07FF;
	_CH[ 1] = ((int16_t)_buff25[ 2] >> 3 | ((int16_t)_buff25[ 3] << 5 )) & 0x07FF;
	_CH[ 2] = ((int16_t)_buff25[ 3] >> 6 | ((int16_t)_buff25[ 4] << 2 )  | (int16_t)_buff25[ 5] << 10 ) & 0x07FF;
	_CH[ 3] = ((int16_t)_buff25[ 5] >> 1 | ((int16_t)_buff25[ 6] << 7 )) & 0x07FF;
	_CH[ 4] = ((int16_t)_buff25[ 6] >> 4 | ((int16_t)_buff25[ 7] << 4 )) & 0x07FF;
	_CH[ 5] = ((int16_t)_buff25[ 7] >> 7 | ((int16_t)_buff25[ 8] << 1 )  | (int16_t)_buff25[ 9] <<  9 ) & 0x07FF;
	_CH[ 6] = ((int16_t)_buff25[ 9] >> 2 | ((int16_t)_buff25[10] << 6 )) & 0x07FF;
	_CH[ 7] = ((int16_t)_buff25[10] >> 5 | ((int16_t)_buff25[11] << 3 )) & 0x07FF;

	_CH[ 8] = ((int16_t)_buff25[12] << 0 | ((int16_t)_buff25[13] << 8 )) & 0x07FF;
	_CH[ 9] = ((int16_t)_buff25[13] >> 3 | ((int16_t)_buff25[14] << 5 )) & 0x07FF;
	_CH[10] = ((int16_t)_buff25[14] >> 6 | ((int16_t)_buff25[15] << 2 )  | (int16_t)_buff25[16] << 10 ) & 0x07FF;
	_CH[11] = ((int16_t)_buff25[16] >> 1 | ((int16_t)_buff25[17] << 7 )) & 0x07FF;
	_CH[12] = ((int16_t)_buff25[17] >> 4 | ((int16_t)_buff25[18] << 4 )) & 0x07FF;
	_CH[13] = ((int16_t)_buff25[18] >> 7 | ((int16_t)_buff25[19] << 1 )  | (int16_t)_buff25[20] <<  9 ) & 0x07FF;
	_CH[14] = ((int16_t)_buff25[20] >> 2 | ((int16_t)_buff25[21] << 6 )) & 0x07FF;
	_CH[15] = ((int16_t)_buff25[21] >> 5 | ((int16_t)_buff25[22] << 3 )) & 0x07FF;
}

// 将通道值转化为用户指令函数
void SbusHandle::channel_command(){
    // 将通道值转化为按键和摇杆值
    // 通道初始值：992 992 992 992 272 272 272 272 992 992 992 992 992 992 992 992  上报的是实际的值 可设置500-2500  (摇杆按键实际行程)最小1000，最2000 对应192,1792
    // 将通道值转化为摇杆值

    //正常模式
    _handleCmd.ry = normalize(_CH[0],2.5,-2.5,272,1712);
    _handleCmd.rx = normalize(_CH[1],-0.5,0.5,272,1712);
    _handleCmd.lx = normalize(_CH[2],-0.9,1.0,272,1712);
    _handleCmd.ly = normalize(_CH[13],0.8,-0.8,272,1712);
    
    // //满行程狂暴模式(勿用)
    // _handleCmd.ry = normalize(_CH[0],3.5,-3.5,272,1712);
    // _handleCmd.rx = normalize(_CH[1],-0.75,0.75,272,1712);
    // _handleCmd.lx = normalize(_CH[2],-2.5,5,272,1712);
    // _handleCmd.ly = normalize(_CH[3],1.0,-1.0,272,1712);

    // 将通道值转化为按键值  C/D 相当于 X/Y映射GO2
    ((_CHBuff[7] != _CH[7]) && (_CH[7]>1060))?_handleCmd.key.A = true:_handleCmd.key.A = false; // 也可以是>1050
    ((_CHBuff[8] != _CH[8]) && _CH[8]>1060)?_handleCmd.key.B = true:_handleCmd.key.B = false;
    ((_CHBuff[9] != _CH[9]) && _CH[9]>1060)?_handleCmd.key.C = true:_handleCmd.key.C = false;
    ((_CHBuff[10] != _CH[10]) && _CH[10]>1060)?_handleCmd.key.D = true:_handleCmd.key.D = false;

    // L1 L2按键
    if(_CH[4]<500){
        _handleCmd.key.L2 = 1;
    }else if (_CH[4]>1500){
        _handleCmd.key.L1 = 1;
    }else{
        _handleCmd.key.L1 = 0;
        _handleCmd.key.L2 = 0;
    }

    // R1 R2按键
    if(_CH[5]<500){
        _handleCmd.key.R2 = 1;
    }else if (_CH[5]>1500){
        _handleCmd.key.R1 = 1;
    }else{
        _handleCmd.key.R1 = 0;
        _handleCmd.key.R2 = 0;
    }

    // M按键
    if(_CH[6]<500){
        _handleCmd.key.M = 0;
    }else if (_CH[6]>1500){
        _handleCmd.key.M = 2;
    }else{
        _handleCmd.key.M = 1;
    }

    // SELECT按键
    ((_CHBuff[11]==992) && _CH[11]> 992)?_handleCmd.key.SELECT = true:_handleCmd.key.SELECT = false;

    // START按键
    ((_CHBuff[12]==992) && _CH[12] > 992)?_handleCmd.key.START = true:_handleCmd.key.START = false;

    // STARTx2按键
    ((_CHBuff[12] != _CH[12]) && _CH[12]==1712)?_handleCmd.key.STARTx2 = true:_handleCmd.key.STARTx2 = false;

    // START_RST按键
    ((_CHBuff[12]==992) && _CH[12] < 992)?_handleCmd.key.START_RST = true:_handleCmd.key.START_RST = false;

    if(_CHBuff[12]==272)  lock = false; //解锁

    // for(int i = 0; i < 16; i++)
    // {
    //     printf("CH[%d]:%d ", i, _CH[i]);
    //     if(i == 15) printf("\n");
    // }

    if(_handleCmd.key.A && _handleCmd.key.L2)
    {
        if(!stand_l)
        {
            _userCmd = UserCommand::L2_A2; //站起来了
            stand_l = true;
        }
        else
        {
            _userCmd = UserCommand::L2_A1; //卧倒
            stand_l = false;
        }

    }
    else if(!_handleCmd.key.L2 && !_handleCmd.key.L1 && _handleCmd.key.START)
    {
            _userCmd = UserCommand::MODE2;
    }
    else if(_handleCmd.key.L2 && _handleCmd.key.B && !stand_l) //阻尼 加站立保护
    {
            _userCmd = UserCommand::L2_B; 
    }
    else if(!_handleCmd.key.L2 && !_handleCmd.key.L1 && stand_l && _handleCmd.key.D)
    {
            _userCmd = UserCommand::AVOID_ON;
    }
    else if(!_handleCmd.key.L2 && !_handleCmd.key.L1 && stand_l && _handleCmd.key.C)
    {
            _userCmd = UserCommand::AVOID_OFF; 
    }
    else if(_handleCmd.key.L2 && _handleCmd.key.C)
    {
            _userCmd = UserCommand::L2_C; 
    }
    else if(!_handleCmd.key.L2 && !_handleCmd.key.L1 && _handleCmd.key.START_RST)
    {
            _userCmd = UserCommand::STOPMOVE; 
    }

    // 通过检测通道值变化，解决复用按键状态切换过快问题
    for(uint i(0); i<(sizeof(_CH)/sizeof(_CH[0])); i++){  //sizeof(_CH)/sizeof(_CH[0]) = 16
        _CHBuff[i] = _CH[i]; // 这里就是相当于储存该通道的上一时刻的值
    }
}

// 串口数据对齐函数
void SbusHandle::data_align(){
    while(RUNNING && !ALIGN){
        _readLen = read(fd, _buff1, sizeof(_buff1));
        printf("%d \n", _readLen);

        if(_buff1[0]==0x0f){
            _readLen = read(fd, _buff24, sizeof(_buff24));
            printf("%d \n", _readLen);
            if(_buff24[23]==0x00){
                // printf("%02X ", _buff1[0]);
                // printf("\n");
                ALIGN=true;
                break;
            }

            for(uint i=0; i<sizeof(_buff24); i++){
                if(_buff24[i]==0x00){
                    int len = 25-(24-1-i);
                    uint8_t buff[len];
                    _readLen = read(fd, buff, len);
                    printf("%d \n", _readLen);

                    if(buff[len-1]==0x00){
                        ALIGN=true;
                        break;
                    }
                }
            }
        }   // if(_buff1[0]==0x0f)

    } // while(RUNNING && !ALIGN)
}
