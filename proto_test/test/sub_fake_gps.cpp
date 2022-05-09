//
// Created by lmx on 22-5-4.
//
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>
#include <iostream>
#include <iomanip>
#include "SITLGps.pb.h"

using std::cout;
using std::cin;
using std::endl;
using std::fixed;
using std::setprecision;

typedef const boost::shared_ptr<const sensor_msgs::msgs::SITLGps> GpsPtr;

gazebo::transport::SubscriberPtr sub;


void gpsCallback(GpsPtr& fake_gps)
{
    cout << fixed << setprecision(7) <<  fake_gps->latitude_deg() << ' ' << fake_gps->longitude_deg() << endl;
}

int main(int _argc, char **_argv)
{

    // Load gazebo
    gazebo::client::setup(_argc, _argv);

    // 创建用于通信的节点,该节点提供发布者和订阅者功能
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // 订阅Gazebo发布的fake gps话题
    sub = node->Subscribe("~/iris/link/gps0", gpsCallback);

    // 消息循环，此处为简单的循环等待
    while (true)
        gazebo::common::Time::MSleep(10);

    // Make sure to shut everything down.
    gazebo::client::shutdown();
}
