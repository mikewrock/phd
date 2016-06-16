#include <ros/ros.h>
#include <string>
#include <tf/transform_broadcaster.h>
#include <visualization_msgs/Marker.h>
float px, py, pz; 
int cb_flag;   
    geometry_msgs::TransformStamped odom_trans;
void cmdCallback(const geometry_msgs::Point::ConstPtr& msg)
{
  ROS_INFO("Global trans heard: [%f %f %f]", msg->x, msg->y, msg->z);
	
    // message declarations
    odom_trans.header.frame_id = "base_link";
    odom_trans.child_frame_id = "marker";     
   odom_trans.header.stamp = ros::Time::now();
        odom_trans.transform.translation.x = msg->x;
        odom_trans.transform.translation.y = msg->y;
        odom_trans.transform.translation.z = msg->z;
        odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(0);
cb_flag = 1;
        //

}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "global_trans");

    ros::NodeHandle n;
    ros::Subscriber joint_cmd_sub = n.subscribe("laser_trans", 1000, cmdCallback);

  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);  // Create a ROS publisher for the output point cloud

tf::TransformBroadcaster broadcaster;


cb_flag = 0;
px= 0.861;
py = -0.200;
pz =  0.812;
  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CUBE;

 visualization_msgs::Marker marker;
while(ros::ok()){  


    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "/base_link";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.x = px;
    marker.pose.position.y = py;
    marker.pose.position.z = pz;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = .010;
    marker.scale.y = .010;
    marker.scale.z = .010;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 1.0f;
    marker.color.g = 1.0f;
    marker.color.b = 1.0f;
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();

    // Publish the marker
    
    marker_pub.publish(marker);

	ros::spinOnce();
if(cb_flag == 1){
broadcaster.sendTransform(odom_trans);
cb_flag =0;
}

}
  return 0;
}

