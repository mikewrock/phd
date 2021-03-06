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
#include <pcl/octree/octree.h>
#include <vector>
#include <ctime>

void 
cloud_cb (const sensor_msgs::PointCloud2ConstPtr& cloud_msg)
{
ROS_INFO("GOT A CLOUD");
}

int
main (int argc, char** argv)
{
  // Initialize ROS
  ros::init (argc, argv, "my_pcl_tutorial");
  ros::NodeHandle nh;

  // Create a ROS subscriber for the input point cloud
  ros::Subscriber sub = nh.subscribe ("assembled_cloud", 1, cloud_cb);
pcl::PointCloud<pcl::PointXYZI>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZI>);
pcl::PointXYZI p1A;
pcl::PointXYZI p2A;
pcl::PointXYZI p3A;
pcl::PointXYZI p1B;
pcl::PointXYZI p2B;
pcl::PointXYZI p3B;

  // Create a ROS publisher for the output point cloud
if(argc < 2){
//printf("usage: rosrun wrock spatial \"filename\" ");
//return(0);
}



 pcl::octree::OctreePointCloudChangeDetector<pcl::PointXYZI> octree (0.01);

  pcl::PointCloud<pcl::PointXYZI>::Ptr cloudA (new pcl::PointCloud<pcl::PointXYZI> );

  
  if (pcl::io::loadPCDFile<pcl::PointXYZI> ("/home/mike/naked.pcd", *cloudA) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file test_pcd.pcd \n");

return(0);
  }
float x, y, z;
BOOST_FOREACH (const pcl::PointXYZI& pt, cloudA->points){
    //printf ("\t(%f, %f, %f, %f)\n", pt.x, pt.y, pt.z, pt.intensity)
	if(pt.intensity > 800) {
		//printf ("\t(%f, %f, %f, %f)\n", pt.x, pt.y, pt.z, pt.intensity);
}
}
  // Add points from cloudA to octree
  octree.setInputCloud (cloudA);
  octree.addPointsFromInputCloud ();

  // Switch octree buffers: This resets octree but keeps previous tree structure in memory.
  octree.switchBuffers ();

  pcl::PointCloud<pcl::PointXYZI>::Ptr cloudB (new pcl::PointCloud<pcl::PointXYZI> );
   
  
  if (pcl::io::loadPCDFile<pcl::PointXYZI> ("/home/mike/clothed.pcd", *cloudB) == -1) //* load the file
  {
    PCL_ERROR ("Couldn't read file test_pcd.pcd \n");

return(0);
  }
BOOST_FOREACH (const pcl::PointXYZI& pt, cloudB->points){
    //printf ("\t(%f, %f, %f, %f)\n", pt.x, pt.y, pt.z, pt.intensity)
	if(pt.intensity > 800) {
		//printf ("\t(%f, %f, %f, %f)\n", pt.x, pt.y, pt.z, pt.intensity);
}
}
  // Add points from cloudB to octree
  octree.setInputCloud (cloudB);
  octree.addPointsFromInputCloud ();

  std::vector<int> newPointIdxVector;

  // Get vector of point indices from octree voxels which did not exist in previous buffer
  octree.getPointIndicesFromNewVoxels (newPointIdxVector);
pcl::PointCloud<pcl::PointXYZI> cloudO;
std::cout << "Populating Cloud" << std::endl;
cloudO.width    = cloudB->width;
  cloudO.height   = cloudB->height;
  cloudO.is_dense = false;
  cloudO.points.resize (cloudO.width * cloudO.height);

 for (size_t i = 0; i < newPointIdxVector.size(); ++i)
  {
    cloudO.points[i].x = cloudB->points[newPointIdxVector[i]].x;
    cloudO.points[i].y = cloudB->points[newPointIdxVector[i]].y;
    cloudO.points[i].z = cloudB->points[newPointIdxVector[i]].z;
  } 


pcl::io::savePCDFileASCII ("/home/mike/change.pcd", cloudO);

std::cout << "Saved" << std::endl;







}
