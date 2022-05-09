//
// Created by lmx on 22-5-5.
//
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>
#include <iostream>

#include "attackGPS.pb.h"

int main(int argc, char *argv[])
{
    // Load gazebo
    gazebo::client::setup(argc, argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // Publish to a Gazebo topic
    gazebo::transport::PublisherPtr pub = node->Advertise<sensor_msgs::msgs::attackGPS>("~/attack_choice", 10);

    // Wait for a subscriber to connect
    pub->WaitForConnection();

    while (true) {
        sensor_msgs::msgs::attackGPS choice;
        int id;
        std::cout << "enter id: ";
        std::cin >> id;
        double deviation_time, last_time;
        std::cout << "deviation_time: ";
        std::cin >> deviation_time;
        std::cout << "last_time: ";
        std::cin >> last_time;
        choice.set_attack_id(id);
        choice.set_deviation_time(deviation_time);
        choice.set_last_time(last_time);
        pub->Publish(choice);
        std::cout << "send attack choice[ choice_id: " << choice.attack_id() << " choice_deviation: " << choice.deviation_time() << " ]" << std::endl;
    }
    // Make sure to shut everything down.
    gazebo::client::shutdown();
}