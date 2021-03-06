#include <ros/ros.h>
// PCL specific includes
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>
#include <string>
#include <tf/transform_broadcaster.h>
#include <pcl/conversions.h>
#include "pcl_ros/transforms.h"
#include <iostream>
#include <pcl/filters/passthrough.h>
#include <pcl/io/pcd_io.h>
#include "std_msgs/String.h"
#include <sstream>
#include <pcl/filters/extract_indices.h>

std_msgs::String filename;
void 
cloud_cb (const sensor_msgs::PointCloud2ConstPtr& cloud_msg)
{
  
ROS_INFO("GOT A CLOUD");
sensor_msgs::PointCloud2 cloud_msg2 = *cloud_msg;
//fix the naming discrepancy between ROS and PCL (from "intensities" to "intensity")
cloud_msg2.fields[3].name = "intensity";
//Create a PCL pointcloud
pcl::PointCloud<pcl::PointXYZI>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZI>);
//Populate the PCL pointcloud with the ROS message
pcl::fromROSMsg(cloud_msg2,*cloud);
//Create containers for filtered clouds
pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filteredx (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filteredxy (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_filteredxyz (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr garbage_filterx (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr garbage_filterxy (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr garbage_filterxyz (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr final (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr cutx (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointCloud<pcl::PointXYZI>::Ptr cutxy (new pcl::PointCloud<pcl::PointXYZI>);

// Create the filtering object
  pcl::PassThrough<pcl::PointXYZI> pass (true);
// Create the filtering object
  pcl::PassThrough<pcl::PointXYZI> pass2 (true);

/*

  pass.setInputCloud (cloud);
  pass.setFilterFieldName ("x");
  pass.setFilterLimits (0,2);
  //pass.setFilterLimitsNegative (true);
  pass.filter (*cloud_filteredx);

  pass.setInputCloud (cloud_filteredx);
  pass.setFilterFieldName ("y");
 // pass.setFilterLimits (-1.66,.3);
  //pass.setFilterLimitsNegative (true);
  pass.filter (*cloud_filteredxy);
*/
  pass.setInputCloud (cloud);
  pass.setFilterFieldName ("z");
  pass.setFilterLimits (-.1,1.5865);
  //pass.setFilterLimitsNegative (true);
  pass.filter (*cloud_filteredxyz);

  // Create the filtering object
  pcl::ExtractIndices<pcl::PointXYZI> extract (true);  


  // Create the filtering object
  pass2.setInputCloud (cloud_filteredxyz);
  //pass2.setInputCloud (cloud);
  pass2.setFilterFieldName ("x");
  pass2.setFilterLimits (1,100);
  //pass.setFilterLimitsNegative (true);
  pass2.filter (*garbage_filterx);  
/*
    // Extract the inliers
    extract.setInputCloud (cloud_filteredxyz);
    extract.setIndices(pass2.getRemovedIndices());
    extract.setNegative (false);
    extract.filter (*cutx);
*/


  // Create the filtering object
//  pass2.setInputCloud (cloud);
  pass2.setInputCloud (cloud_filteredxyz);
  pass2.setFilterFieldName ("y");
  pass2.setFilterLimits (-100,-1);
  //pass.setFilterLimitsNegative (true);
  pass2.filter (*garbage_filterxy);  
    // Extract the inliers
   /* extract.setInputCloud (cutx);
    extract.setIndices(pass2.getRemovedIndices());
    extract.setNegative (false);
    extract.filter (*cutxy);
*/

  // Create the filtering object
//  pass2.setInputCloud (cloud);
  pass2.setInputCloud (cloud_filteredxyz);
  pass2.setFilterFieldName ("z");
  pass2.setFilterLimits (.75,100);
  //pass.setFilterLimitsNegative (true);
  pass2.filter (*garbage_filterxyz);  
  /*  extract.setInputCloud (cutxy);
    extract.setIndices(pass2.getRemovedIndices());
    extract.setNegative (false);
    extract.filter (*final);
*/
*final += *garbage_filterx;
*final += *garbage_filterxy;
*final += *garbage_filterxyz;

  // Convert to ROS data type
  sensor_msgs::PointCloud2 output;
  pcl::toROSMsg(*cloud_filteredxyz, output);
  pcl::io::savePCDFileASCII (filename.data.c_str(), *final);
ROS_INFO("Saved to %s", filename.data.c_str());
}

int
main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "my_pcl_tutorial");
  ros::NodeHandle nh;

  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe ("assembled_cloud", 1, cloud_cb);

  // Create a ROS publisher for the output point cloud
if(argc < 2){
printf("usage: rosrun wrock savepcd \"filename\" to save to \\home\\mike\\filename.pcd");
return(0);
}
    std::stringstream ss;
    ss << "/home/mike/" << argv[1] << ".pcd";
    filename.data = ss.str();

    ROS_INFO("Writing to %s", filename.data.c_str());
ros::spin();

}
