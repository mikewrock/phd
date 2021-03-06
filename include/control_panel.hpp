/**
 * @file /include/test_panel/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef control_panel_AQNODE_HPP_
#define control_panel_AQNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/
// Always goes first
#define _CUSTOM_UINT64c
// ROS
#include <ros/ros.h>
# include <ros/node_handle.h>
# include <ros/publisher.h>
# include "rviz/tool.h"
#include "bcap/stdint.h"
//#include <atlbase.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <QThread>
#include <QStringListModel>
# include <QCursor>
# include <QObject>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/JointState.h>

#include "std_msgs/String.h"
#include <sstream>
#include <pcl/point_cloud.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <sensor_msgs/PointCloud2.h>

#include <pcl/filters/extract_indices.h>
// bCAP (Always last)
#include "bcap/bcap_client.h"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace control_panel {
namespace control_panel_ns {


/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread {
    Q_OBJECT
public:

	void jointCallback(const sensor_msgs::JointState::ConstPtr& msg);
	void cloudCallback(const sensor_msgs::PointCloud2::ConstPtr& cloud);
	QNode(){}
	virtual ~QNode();
	void scan();
	void nav_mode(float pos);
	void fscan(int file);
	void cluster(int index);
	void gen_trajectory();
	void lscan();
	void run();
	bool init();
	float P1x, P1y, P1z, P2x, P2y, P2z;


Q_SIGNALS:
	void rosShutdown();

private:
	int init_argc;
	char** init_argv;
	ros::NodeHandle nh_;
	ros::Subscriber joint_sub;
	ros::Subscriber cloud_sub;
	ros::Publisher cmd_pub;
	ros::Publisher pub;
	ros::ServiceClient client;
	ros::ServiceClient loc_client;
	ros::ServiceClient traj_client;
	pcl::PointCloud<pcl::PointXYZI>::Ptr current_pc_;
};
}
}  // namespace 

#endif /* test_panel_QNODE_HPP_ */
