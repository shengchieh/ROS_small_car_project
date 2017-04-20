// ROS CAR
#include "mbed.h"
#include "motor_uart.h"
#include <ros.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Twist.h>

float rpm_2_rps = 0.104719755;        // 1 rpm = 2pi / 60 rps 
float rps_2_rpm = 9.549296586;        // 1 rps = 60 / 2pi rpm

Ticker timer1;

void init_TIMER(void);
void timer1_interrupt(void);

ros::NodeHandle nh;

geometry_msgs::Twist vel_msg;
ros::Publisher p("feedback_wheel_angularVel", &vel_msg);

void messageCb(const geometry_msgs::Vector3& msg)
{
   VelocityRef_1 = msg.x * rps_2_rpm * (-1);  
   VelocityRef_2 = msg.y * rps_2_rpm;
}

ros::Subscriber<geometry_msgs::Vector3> s("cmd_wheel_angularVel",messageCb);

int main() 
{
    init_TIMER();
    init_PWM();
    init_CN();
    
    nh.initNode();
    nh.subscribe(s);
    nh.advertise(p);
    
    while(1) 
    {
        vel_msg.linear.x = VelocityRef_1;       // command 1
        vel_msg.linear.y = Velocity_1;          // respond 1
        vel_msg.linear.z = 0;
        
        vel_msg.angular.x = VelocityRef_2;      // command 2
        vel_msg.angular.y = Velocity_2;         // respond 2
        vel_msg.angular.z = 0;
        
        p.publish(&vel_msg);
        
        nh.spinOnce();
        wait_ms(50);
    }
}

void timer1_interrupt()
{    
    motor_PI();
}

void init_TIMER()
{
    timer1.attach_us(&timer1_interrupt, 10000);    // 10 ms interrupt period (100 Hz)
}

