// Always goes first
#define _CUSTOM_UINT64
// ROS
#include <ros/ros.h>

#include "../include/control_panel.hpp"
//#include <atlbase.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <tf/transform_broadcaster.h>
#include <sensor_msgs/JointState.h>  

#include <phd/cube_msg.h>
#include <cstdio>
// Services
#include "laser_assembler/AssembleScans2.h"
#include "phd/localize_cloud.h"
// Messages
#include "sensor_msgs/PointCloud.h"

#include <pcl/io/pcd_io.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/point_types.h>

#include <ros/ros.h>
#include <ros/time.h>
#include <sensor_msgs/PointCloud2.h>
#include <QVariant>

#include <pcl_conversions/pcl_conversions.h>
#include <pcl/common/transforms.h>

#include <pcl/visualization/pcl_visualizer.h>

#include <pcl/filters/passthrough.h>
#include <pcl/common/common.h>

#include <pcl/filters/impl/box_clipper3D.hpp>


#include <visualization_msgs/Marker.h>

#include <pcl/filters/crop_box.h>
#include <pcl/common/angles.h>
#include <iostream>
#include <std_msgs/Float64.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>


#include "phd/trajectory_service.h"


namespace control_panel{
namespace control_panel_ns{

        double joint1;
laser_assembler::AssembleScans2 srv;
phd::localize_cloud loc_srv;
phd::trajectory_service traj_srv;

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}
void QNode::jointCallback(const sensor_msgs::JointState::ConstPtr& msg)
{
	std::string str1 = "cube_5_joint";
	  std::string str2 =msg->name[0] ;
	if(str1==str2){
	  joint1 = msg->position[0];
	}
}
void QNode::cloudCallback(const sensor_msgs::PointCloud2::ConstPtr& cloud)
{

	current_pc_.reset(new pcl::PointCloud<pcl::PointXYZI>());
	pcl::fromROSMsg(*cloud, *this->current_pc_);
}

bool QNode::init() {
	ROS_INFO( "Control Panel initializing");
	int fargc = 0;
	char** fargv = (char**)malloc(sizeof(char*)*(fargc+1));
	ros::init(fargc,fargv,"ControlPanelNode");
	free(fargv);
	ros::start();
	cmd_pub = nh_.advertise<phd::cube_msg>("joint_cmd", 100);
	pub = nh_.advertise<sensor_msgs::PointCloud2> ("assembled_cloud", 1);
	joint_sub = nh_.subscribe("cube_joint_states", 1, &control_panel::control_panel_ns::QNode::jointCallback,this);
	cloud_sub = nh_.subscribe("marker_selected_points", 1, &control_panel::control_panel_ns::QNode::cloudCallback,this);
    //service client for calling the assembler
	client = nh_.serviceClient<laser_assembler::AssembleScans2>("assemble_scans2");
	loc_client = nh_.serviceClient<phd::localize_cloud>("localize_pcd");
	traj_client = nh_.serviceClient<phd::trajectory_service>("trajectory_gen");
	ros::start();
}
void QNode::run() {
	std::cout << "Control Panel Running" << std::endl;
	
	while ( ros::ok() ) {
		ros::spinOnce();
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;


	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}


void QNode::scan(){

phd::cube_msg cube_cmd;
		joint1 = 1;
	ROS_INFO("Waiting for j1");
	while((joint1 < -.8 || joint1 > .77) && ros::ok()){
	ros::spinOnce();
	//printf("j1: %f\n",joint1);
	} 
	cube_cmd.j1 = -2;
	cube_cmd.j2 = 0;
	cube_cmd.j3 = 0;
	cube_cmd.j4 = 0;
	cube_cmd.j5 = 0;
	cube_cmd.vel = 0.5;
	cube_cmd.acc = 2;
	cube_cmd.pose = true;
		//send the joint state and transform
	    // Populate our service request based on our timer callback times
	cmd_pub.publish(cube_cmd);
	ros::spinOnce();
	printf("j1: %f\n",joint1);
	ROS_INFO("going to start");
	while(joint1 > -1.9 && ros::ok()){
	ros::spinOnce();
	printf("j1is: %f\n",joint1);
	} 
	 srv.request.begin = ros::Time::now();
	cube_cmd.j1 = 0;

	cube_cmd.vel = 0.25;
	cmd_pub.publish(cube_cmd);
	ros::spinOnce();
	ROS_INFO("waiting for end");
	while(joint1 < -.01 && ros::ok()){
	ros::spinOnce();
	} 
	ROS_INFO("done");
	 srv.request.end   = ros::Time::now();
	ros::spinOnce();
	    // Make the service call
	    if (client.call(srv))
	    {
	      ROS_INFO("Published Cloud %d", (uint32_t)(srv.response.cloud.width)) ;
	      pub.publish(srv.response.cloud);
	    }
	    else
	    {
	      ROS_ERROR("Error making service call\n") ;
	    }

}


void QNode::nav_mode(float pos){

phd::cube_msg cube_cmd;
		
	cube_cmd.j1 = pos;
	cube_cmd.j2 = 0;
	cube_cmd.j3 = 0;
	cube_cmd.j4 = 0;
	cube_cmd.j5 = 0;
	cube_cmd.vel = 0.5;
	cube_cmd.acc = 2;
	cube_cmd.pose = true;
		//send the joint state and transform
	    // Populate our service request based on our timer callback times
	cmd_pub.publish(cube_cmd);
	ros::spinOnce();

}


void QNode::fscan(int file){

pcl::PointCloud<pcl::PointXYZI>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZI> );

if(file == 2){
if (pcl::io::loadPCDFile<pcl::PointXYZI> ("/home/mike/marker/refined/dotsf4.pcd", *cloud) == -1) 
{
	PCL_ERROR ("Couldn't read file test_pcd.pcd \n");

}else 
std::cerr << "Sending cloud with: " << cloud->width * cloud->height << " data points." << std::endl;
}

if(file == 3){
if (pcl::io::loadPCDFile<pcl::PointXYZI> ("/home/mike/marker/refined/dotsf5.pcd", *cloud) == -1) 
{
	PCL_ERROR ("Couldn't read file test_pcd.pcd \n");

}else 
std::cerr << "Sending cloud with: " << cloud->width * cloud->height << " data points." << std::endl;
}
if(file == 4){
if (pcl::io::loadPCDFile<pcl::PointXYZI> ("/home/mike/marker/refined/dotsf6.pcd", *cloud) == -1) 
{
	PCL_ERROR ("Couldn't read file test_pcd.pcd \n");

}else 
std::cerr << "Sending cloud with: " << cloud->width * cloud->height << " data points." << std::endl;
}

sensor_msgs::PointCloud2 cloud_msg;
	pcl::toROSMsg(*cloud,cloud_msg);
	//fix the naming discrepancy between ROS and PCL (from "intensities" to "intensity")
	cloud_msg.fields[3].name = "intensities";

	cloud_msg.header.frame_id = "/base_link";
loc_srv.request.cloud_in = cloud_msg;
loc_srv.request.homing = false;
loc_client.call(loc_srv);
	if(loc_client.call(loc_srv)){
	ROS_INFO("Received Cloud %d", (uint32_t)(loc_srv.response.cloud_out.width));
		      pub.publish(loc_srv.response.cloud_out);
	}else ROS_INFO("Service Failed");

}
void QNode::lscan(){

	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZI> );


	if (pcl::io::loadPCDFile<pcl::PointXYZI> ("/home/mike/marker/refined/dotsf3.pcd", *cloud) == -1) 
	{
		PCL_ERROR ("Couldn't read file test_pcd.pcd \n");

	}else 
	std::cerr << "Sending cloud with: " << cloud->width * cloud->height << " data points." << std::endl;

	sensor_msgs::PointCloud2 cloud_msg;
		pcl::toROSMsg(*cloud,cloud_msg);
		//fix the naming discrepancy between ROS and PCL (from "intensities" to "intensity")
		cloud_msg.fields[3].name = "intensities";

	cloud_msg.header.frame_id = "/base_link";
	loc_srv.request.cloud_in = cloud_msg;
	loc_srv.request.homing = true;
	if(loc_client.call(loc_srv)){
	ROS_INFO("Received Cloud %d", (uint32_t)(loc_srv.response.cloud_out.width));
		      pub.publish(loc_srv.response.cloud_out);
	}else ROS_INFO("Service Failed");
}
void QNode::cluster(int index){
	int init_size =  current_pc_->width * current_pc_->height;
	ROS_INFO("Clustering %d, %d points", index,init_size) ;
	//Create containers for filtered clouds
	pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_intensity_filtered (new pcl::PointCloud<pcl::PointXYZI>);
	// Create the filtering object
	pcl::PassThrough<pcl::PointXYZI> pass (true);
	//filter the data for intensity
	pass.setInputCloud (current_pc_);
	pass.setFilterFieldName ("intensity");
	pass.setFilterLimits (550,1100);
	pass.filter (*cloud_intensity_filtered);

	float xs=0;
	float ys=0;
	float zs=0;
	int size = cloud_intensity_filtered->width * cloud_intensity_filtered->height;
	//average all the points
	for(int i = 0; i < size; ++i){
	//ROS_INFO("Adding Point: %f - %f - %f / %d",cloud_intensity_filtered->points[i].x,cloud_intensity_filtered->points[i].y,cloud_intensity_filtered->points[i].z,i);
		xs += cloud_intensity_filtered->points[i].x;
		ys += cloud_intensity_filtered->points[i].y;
		zs += cloud_intensity_filtered->points[i].z;
	}
	xs = xs/size;
	ys = ys/size;
	zs = zs/size;
	ROS_INFO("Saving Point: %f - %f - %f / %d",xs, ys, zs, size);
	ofstream myfile;
	std::stringstream fs;
	fs << "/home/mike/marker/location/" << "filename" << ".dat";
	switch (index){
		case 0:	myfile.open (fs.str().c_str(), std::ios::out|std::ios::trunc);
			P1x = xs;
			P1y = ys;
			P1z = zs;
		case 1: myfile.open (fs.str().c_str(), std::ios::out|std::ios::app);
			P2x = xs;
			P2y = ys;
			P2z = zs;
		case 2: myfile.open (fs.str().c_str(), std::ios::out|std::ios::app);
			rosbag::Bag bag;
			Eigen::Vector3f a,n,c,o;
			Eigen::Matrix4f A_mat;
			float mn = sqrt(pow(xs-P2x,2)+pow(ys-P2y,2)+pow(zs-P2z,2));
			float mc = sqrt(pow(P1x-P2x,2)+pow(P1y-P2y,2)+pow(P1z-P2z,2));
			n(0) = (xs-P2x)/mn;
			n(1) = (ys-P2y)/mn;
			n(2) = (zs-P2z)/mn;
			c(0) = (P1x-P2x)/mc;
			c(1) = (P1y-P2y)/mc;
			c(2) = (P1z-P2z)/mc;
			a(0) = n(1)*c(2)-n(2)*c(1);
			a(1) = n(2)*c(0)-n(0)*c(2);
			a(2) = n(0)*c(1)-n(1)*c(0);
			float m = sqrt(pow(a(0),2)+pow(a(1),2)+pow(a(2),2));
			a(0) = a(0)/m;
			a(1) = a(1)/m;
			a(2) = a(2)/m;	
			o(0) = a(1)*n(2)-a(2)*n(1);
			o(1) = a(2)*n(0)-a(0)*n(2);
			o(2) = a(0)*n(1)-a(1)*n(0);
			A_mat(0,0) = n(0);
			A_mat(0,1) = o(0);
			A_mat(0,2) = a(0);
			A_mat(0,3) = (P1x+P2x+xs)/3;
			A_mat(1,0) = n(1);
			A_mat(1,1) = o(1);
			A_mat(1,2) = a(1);
			A_mat(1,3) = (P1y+P2y+ys)/3;
			A_mat(2,0) = n(2);
			A_mat(2,1) = o(2);
			A_mat(2,2) = a(2);
			A_mat(2,3) = (P1z+P2z+zs)/3;
			A_mat(3,0) = 0;
			A_mat(3,1) = 0;
			A_mat(3,2) = 0;
			A_mat(3,3) = 1;

			bag.open("/home/mike/marker/fiducial.bag", rosbag::bagmode::Write);
			std_msgs::Float64 f;
			for(int fctr = 0; fctr < 4; ++fctr){
				for(int fctr2 = 0; fctr2 < 4; ++fctr2){
					f.data = A_mat(fctr2,fctr);
					bag.write("fiducial", ros::Time::now(), f);
				}
			}
			bag.close();	
	}
	myfile << xs << std::endl << ys << std::endl << zs << std::endl;
	myfile.close();

}

void QNode::gen_trajectory(){
	ROS_INFO("Generating Trajectory") ;
	sensor_msgs::PointCloud2 cloud_msg;
	pcl::toROSMsg(*current_pc_,cloud_msg);
	//fix the naming discrepancy between ROS and PCL (from "intensities" to "intensity")
	cloud_msg.fields[3].name = "intensities";
	cloud_msg.header.frame_id = "/base_link";
	traj_srv.request.cloud_in = cloud_msg;
	int ret = traj_client.call(traj_srv);
	if(ret){
	ROS_INFO("Received Trajectory");
	}else ROS_INFO("Service Failed %d");

		


}




}
};


