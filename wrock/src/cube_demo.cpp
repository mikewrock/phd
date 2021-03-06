#include <wrock/cube_msg.h>
#include <ros/ros.h>
#include <string>

int vel = 1.5;
int acc = 2; 
int ret = 0;

wrock::cube_msg cube_cmd;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "cube_demo");
/*
  if (argc != 3)
  {
    ROS_INFO("usage: add_two_ints_client X Y");
    return 1;
  }

  ros::NodeHandle n;
  ros::ServiceClient client = n.serviceClient<husky_wrock::AddTwoInts>("add_two_ints");
  husky_wrock::AddTwoInts srv;
  srv.request.a = atoll(argv[1]);
  srv.request.b = atoll(argv[2]);
*/  
  ros::NodeHandle n;
    ros::Publisher cmd_pub = n.advertise<wrock::cube_msg>("joint_cmd", 100);
cube_cmd.j1 = 0.5;
cube_cmd.j2 = 0.5;
cube_cmd.j3 = 0.5;
cube_cmd.j4 = 0.5;
cube_cmd.j5 = 0.5;
cube_cmd.vel = 1.5;
cube_cmd.acc = 2;
cube_cmd.pose = true;
        //send the joint state and transform


while(ros::ok()){
cmd_pub.publish(cube_cmd);
ros::spinOnce();
ros::Duration(5).sleep();
}
  return 0;
}

