//
// Created by lmx on 22-5-5.
//

#ifndef SRC_GPS_ATTACK_H
#define SRC_GPS_ATTACK_H

#include <random>
#include <chrono>
#include <iostream>
#include <deque>
#include "SITLGps.pb.h"

double _myRand(int minValue, int maxValue);
double nor_dist(double a, double b);

// 1. 加入固定偏制
sensor_msgs::msgs::SITLGps fixed_bias(const sensor_msgs::msgs::SITLGps& temp_gps, double deviation);

// 2. 每次加入的偏制都增加一倍
sensor_msgs::msgs::SITLGps change_bias(const sensor_msgs::msgs::SITLGps& temp_gps, double deviation, int time_count);

// 3. 噪声攻击：标准差为deviation
sensor_msgs::msgs::SITLGps noise_att(const sensor_msgs::msgs::SITLGps& temp_gps, double deviation);

// 4/5. miss/重放攻击：每次发出的gps都是1秒(deviation)前的gps_msg
sensor_msgs::msgs::SITLGps re_att(const std::deque<sensor_msgs::msgs::SITLGps>& gps_msg_que, int deviation_time);

#endif //SRC_GPS_ATTACK_H
