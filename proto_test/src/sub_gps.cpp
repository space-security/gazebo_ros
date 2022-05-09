//
// Created by lmx on 22-5-4.
//
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>

#include <iostream>
#include <deque>
// 处理消息需要的头文件
#include "SITLGps.pb.h"
#include "attackGPS.pb.h"
// 将gps攻击相关代码包含进来
#include "gps_attack.h"

#define DEQMAXSIZE 100

using std::cout;
using std::cin;
using std::endl;
using std::fixed;
using std::setprecision;

typedef const boost::shared_ptr<const sensor_msgs::msgs::SITLGps> GpsPtr;
typedef const boost::shared_ptr<const sensor_msgs::msgs::attackGPS> AttackPtr;

std::deque<sensor_msgs::msgs::SITLGps> gps_msg_que;

gazebo::transport::PublisherPtr pub;
gazebo::transport::SubscriberPtr sub_gps;
gazebo::transport::SubscriberPtr sub_attack_msg;

// 攻击参数，choice表示攻击方式，deviation表示偏制直
int choice = 0;
double deviation_time = 0.0;

int time_length = 0;
int time_count = 1;
sensor_msgs::msgs::SITLGps miss_gps;

void attackMsgCallback(AttackPtr& attack_msg)
{
    choice = attack_msg->attack_id();
    if (choice == 4)
        miss_gps = gps_msg_que.back();

    deviation_time = attack_msg->deviation_time();
    time_length = int(attack_msg->last_time() * 5);
    if (time_length == 0)
        time_length++;

    time_count = 1;
}

void gpsCallback(GpsPtr& gps_msg)
{
    // 将传入的指针里面的内容保存下来
    sensor_msgs::msgs::SITLGps temp_gps;
    temp_gps.set_time_usec(gps_msg->time_usec());
    temp_gps.set_time_utc_usec(gps_msg->time_utc_usec());
    temp_gps.set_latitude_deg(gps_msg->latitude_deg());
    temp_gps.set_longitude_deg(gps_msg->longitude_deg());
    temp_gps.set_altitude(gps_msg->altitude());
    temp_gps.set_eph(gps_msg->eph());
    temp_gps.set_epv(gps_msg->epv());
    temp_gps.set_velocity(gps_msg->velocity());
    temp_gps.set_velocity_east(gps_msg->velocity_east());
    temp_gps.set_velocity_north(gps_msg->velocity_north());
    temp_gps.set_velocity_up(gps_msg->velocity_up());
    // 将获取的的gps_msg存储，并将队列最后的元素作为欺骗的消息
    if (gps_msg_que.size() < DEQMAXSIZE) {
        gps_msg_que.push_back(temp_gps);
    } else {
        gps_msg_que.pop_front();
        gps_msg_que.push_back(temp_gps);
    }
    // gps attack
    sensor_msgs::msgs::SITLGps fake_gps;
    if (time_count <= time_length) {
        if (choice == 1) {
            // 1. 加入固定偏制
            fake_gps = fixed_bias(gps_msg_que.back(), deviation_time);
        } else if (choice == 2) {
            // 2. 每次加入的偏制都增加一倍
            fake_gps = change_bias(gps_msg_que.back(), deviation_time, time_count);
        } else if (choice == 3) {
            // 3. 噪声攻击：标准差为deviation
            fake_gps = noise_att(gps_msg_que.back(), deviation_time);
        } else if (choice == 4) {
            // 4. miss
            fake_gps = miss_gps;
        } else if (choice == 5) {
            // 5. 重放攻击
            fake_gps = re_att(gps_msg_que, deviation_time);
        } else {
            fake_gps = gps_msg_que.back();
        }
        time_count++;
    }
    else
        fake_gps = gps_msg_que.back();

    // publish fake gps
    pub->Publish(fake_gps);

    cout << fixed << setprecision(7) << gps_msg->latitude_deg() << ' ' << gps_msg->longitude_deg() << endl;
}

int main(int argc, char *argv[])
{
    // Load gazebo
    gazebo::client::setup(argc, argv);

    // 创建用于通信的节点,该节点提供发布者和订阅者功能
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    sub_attack_msg = node->Subscribe("~/attack_choice", attackMsgCallback);

    // 发布fake gps(伪装成真实的gps消息）
    pub = node->Advertise<sensor_msgs::msgs::SITLGps>("~/iris/link/gps0", 10);

    // 订阅Gazebo发布的gps话题（在gazebo_gps_plugin.cpp中修改发布话题名称为~/iris/link/gps0/true
    sub_gps = node->Subscribe("~/iris/gps0/true", gpsCallback);

    // 订阅攻击方式的话题名称，并在callback中修改choice值

    // 消息循环，此处为简单的循环等待
    while (true)
        gazebo::common::Time::MSleep(10);

    // Make sure to shut everything down.
    gazebo::client::shutdown();
}
