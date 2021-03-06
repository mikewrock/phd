#include <ros/ros.h>
#include <string>
#include <tf/transform_broadcaster.h>

// ROS includes
#include <urdf/model.h>

// ROS message includes
#include <sensor_msgs/JointState.h>
#include <trajectory_msgs/JointTrajectory.h>
#include <control_msgs/JointTrajectoryControllerState.h>
#include <diagnostic_msgs/DiagnosticArray.h>
#include <brics_actuator/JointPositions.h>
#include <brics_actuator/JointVelocities.h>



//Global Variables
float joint1;

void cubeCallback(const sensor_msgs::JointState::ConstPtr& msg)
{
std::string str1 = "cube_5_joint";
  std::string str2 =msg->name[0] ;
if(str1==str2){
  joint1 = msg->position[0];
}
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "cube_move");

    ros::NodeHandle n;
    ros::Subscriber joint_sub = n.subscribe("cube_joint_states", 1, cubeCallback);
    ros::Publisher joint_pub = n.advertise<sensor_msgs::JointState>("joint_states", 10, true);
    tf::TransformBroadcaster broadcaster;
    sensor_msgs::JointState joint_state;  
	joint1 = 0;
while(ros::ok()){
//////////////////Publish initial values
  joint_state.header.stamp = ros::Time::now();
  joint_state.name.resize(13);
  joint_state.position.resize(13);
  joint_state.name[0] ="joint_back_left_wheel";
  joint_state.position[0] = 0;
  joint_state.name[1] ="joint_back_right_wheel";
  joint_state.position[1] = 0;
  joint_state.name[2] ="joint_front_left_wheel";
  joint_state.position[2] = 0;
  joint_state.name[3] ="joint_front_right_wheel";
  joint_state.position[3] = 0;
  joint_state.name[4] ="cube_5_joint";
  joint_state.position[4] = joint1;
  joint_state.name[5] ="j1";
  joint_state.position[5] = 0;
  joint_state.name[6] ="j2";
  joint_state.position[6] = 0;
  joint_state.name[7] ="j3";
  joint_state.position[7] = 0;
  joint_state.name[8] ="j4";
  joint_state.position[8] = 0;
  joint_state.name[9] ="j5";
  joint_state.position[9] = 0;
  joint_pub.publish(joint_state);

  //check for callbacks
  ros::spinOnce();
}
return 0;
}




