//
// Created by lmx on 22-5-5.
//
#include "gps_attack.h"
// 在指定范围内生成随机float型的数据
double _myRand(int minValue, int maxValue)
{
    float Value = (rand()%(maxValue*10-minValue*10 + 1)) + minValue*10;
    return Value / 10;
}

// 定义正态分布,输入正常值作为分布的期望，输出随机值
double nor_dist(double a, double b)
{
    // a 表示期望，b表示标准差
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(a, b);
    return distribution(generator);
}

// 1. 加入固定偏制
sensor_msgs::msgs::SITLGps fixed_bias(const sensor_msgs::msgs::SITLGps& temp_gps, double deviation)
{
    sensor_msgs::msgs::SITLGps fake_gps = temp_gps;
    fake_gps.set_latitude_deg(temp_gps.latitude_deg() + deviation);
    fake_gps.set_longitude_deg(temp_gps.longitude_deg() + deviation);
    return fake_gps;
}

// 2. 每次加入的偏制都增加一倍
sensor_msgs::msgs::SITLGps change_bias(const sensor_msgs::msgs::SITLGps& temp_gps, double deviation, int time_count)
{
    sensor_msgs::msgs::SITLGps fake_gps = temp_gps;
    fake_gps.set_latitude_deg(temp_gps.latitude_deg() + deviation * time_count);
    fake_gps.set_longitude_deg(temp_gps.longitude_deg() + deviation * time_count);
    return fake_gps;
}

// 3. 噪声攻击：标准差为deviation
sensor_msgs::msgs::SITLGps noise_att(const sensor_msgs::msgs::SITLGps& temp_gps, double deviation)
{
    sensor_msgs::msgs::SITLGps fake_gps = temp_gps;
    fake_gps.set_latitude_deg(nor_dist(temp_gps.latitude_deg(), deviation));
    fake_gps.set_longitude_deg(nor_dist(temp_gps.longitude_deg(), deviation));
    return fake_gps;
}

// 4. miss:每次发出的gps都是t时刻的gps_msg
//sensor_msgs::msgs::SITLGps miss_att(const std::deque<sensor_msgs::msgs::SITLGps>& gps_msg_que, double att_time)
//{
//    int att_time_count = int(att_time * 5);
//    if (att_time_count <= gps_msg_que.size())
//        sensor_msgs::msgs::SITLGps fake_gps = gps_msg_que[gps_msg_que.size() - att_time_count];
//    return fake_gps;
//}

// 4/5. miss/重放攻击：每次发出的gps都是1秒(deviation)前的gps_msg
sensor_msgs::msgs::SITLGps re_att(const std::deque<sensor_msgs::msgs::SITLGps>& gps_msg_que, int deviation_time)
{
    int att_time_count = int(deviation_time * 5);
    sensor_msgs::msgs::SITLGps fake_gps;
    if (deviation_time <= gps_msg_que.size())
        fake_gps = gps_msg_que[gps_msg_que.size() - att_time_count];
    return fake_gps;
}

