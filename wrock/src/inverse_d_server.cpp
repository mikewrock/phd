#include "ros/ros.h"
#include "wrock/inverse_d.h"	
#include <tf/transform_datatypes.h>	
#include <math.h>

bool add(wrock::inverse_d::Request  &req,
         wrock::inverse_d::Response &res)
{
//Get these params from a config file
float a = .02;
float b = .4;
float d = .45;
float e = .135;
ROS_INFO("Req: %f %f %f %f %f %f", req.pose.x,req.pose.y,req.pose.z,req.pose.xrot,req.pose.yrot,req.pose.zrot);
//delare variables
//tf::Quaternion q;
//q.setRPY(0,req.pose.pitch,req.pose.yaw);
//tf::Matrix3x3 m(q);
double l[12];
//m.getOpenGLSubMatrix(l);
//tf::Vector3 r1;
//tf::Vector3 r2;
//tf::Vector3 r3;
//r1 = m.getRow(0);
//r2 = m.getRow(1);
//r3 = m.getRow(2);
l[0] = cos(req.pose.zrot)*cos(req.pose.yrot);
l[1] = cos(req.pose.zrot)*sin(req.pose.yrot)*sin(req.pose.xrot)-sin(req.pose.zrot)*cos(req.pose.xrot);
l[2] = cos(req.pose.zrot)*sin(req.pose.yrot)*cos(req.pose.xrot)+sin(req.pose.zrot)*sin(req.pose.xrot);
l[4] =sin(req.pose.zrot)*cos(req.pose.yrot);
l[5] =sin(req.pose.zrot)*sin(req.pose.yrot)*sin(req.pose.xrot)+cos(req.pose.zrot)*cos(req.pose.xrot);
l[6] =sin(req.pose.zrot)*sin(req.pose.yrot)*cos(req.pose.xrot)-cos(req.pose.zrot)*sin(req.pose.xrot);
l[10] =cos(req.pose.yrot)*cos(req.pose.xrot);
ROS_INFO("Rot: %f %f %f %f %f %f %f", l[0], l[1], l[2], l[4], l[5], l[6], l[10] );
ROS_INFO("l[2][6]: %f %f", l[2], l[6]);
//calculate J1
res.joints.j1 = atan2((req.pose.x/e)-l[2],l[6]-(req.pose.y/e))-atan2(sqrt(pow((l[6]-req.pose.y/e),2)+pow((req.pose.x/e-l[2]),2)-pow((a/e),2)),-a/e);
float c1 = cos(res.joints.j1);
float s1 = sin(res.joints.j1);
// ERROR CHECK
if (pow((c1*l[5]-l[1]*s1),2) >= 1){
    ROS_INFO("ERROR XY LIMIT");
}

//Compute Wrist Location
wrock::cube_pose pw;
pw.x = req.pose.x - l[2]*e;
pw.y = req.pose.y - l[6]*e;
pw.z = req.pose.z - l[10]*e;

//Compue J2 - J5
float temp = (1/(2*d*b))*(pow(b,2) + pow(a,2) +pow(d,2)-pow(pw.x,2)-pow(pw.y,2)-pow(pw.z,2));
//ERROR CHECK
if( pow(temp,2)>1){
    
    ROS_INFO("ERROR TEMP");
}

res.joints.j3 = atan2(temp,sqrt(1-pow(temp,2)));
float c3 = cos(res.joints.j3);
float s3 = sin(res.joints.j3);

res.joints.j2 = atan2(s3*d-b,d*c3)+atan2(sqrt(pow((d*c3),2)+pow((s3*d-b),2)-pow(pw.z,2)),pw.z);

res.joints.j4 = atan2(sqrt(1-pow((c1*l[5]-l[1]*s1),2)),c1*l[5]-l[1]*s1);
float s4 = sin(res.joints.j4);
float c4 = cos(res.joints.j4);

res.joints.j5 = atan2(sqrt(1-pow(((c1*l[4]-l[0]*s1)/(s4)),2)),(c1*l[4]-l[0]*s1)/(s4));

ROS_INFO("Res: %f %f %f %f %f", res.joints.j1,res.joints.j2,res.joints.j3,res.joints.j4,res.joints.j5);
return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "inverse_d_server");
  ros::NodeHandle n;

  ros::ServiceServer service = n.advertiseService("inverse_d", add);
  ROS_INFO("Inverse Displacement Online");
  ros::spin();

  return 0;
}
