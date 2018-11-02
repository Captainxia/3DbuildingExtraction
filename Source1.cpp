//#define _SCL_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
// PCL_3d.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <iostream>
#include <vector>
typedef pcl::PointXYZRGBA PointT;
typedef pcl::PointCloud<PointT> PointCloudT;

pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());
boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("viewer"));
pcl::PointCloud<pcl::PointXYZ>::Ptr clicked_points_3d(new pcl::PointCloud<pcl::PointXYZ>);
int num = 0;

void pp_callback(const pcl::visualization::AreaPickingEvent& event, void* args)
{
	std::vector< int > indices;
	if (event.getPointsIndices(indices) == -1)
		return;

	for (int i = 0; i < indices.size(); ++i)
	{
		clicked_points_3d->points.push_back(cloud->points.at(indices[i]));
	}

	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> red(clicked_points_3d, 255, 0, 0);

	std::stringstream ss;
	std::string cloudName;
	ss << num++;
	ss >> cloudName;
	cloudName += "_cloudName";

	viewer->addPointCloud(clicked_points_3d, red, cloudName);
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, cloudName);
}

void main1()
{
	if (pcl::io::loadPLYFile("E:\\my_projects\\PCL_3d\\data\\01.ply", *cloud))
	{
		std::cerr << "ERROR: Cannot open file " << std::endl;
		return;
	}

	viewer->addPointCloud(cloud, "bunny");
	viewer->setCameraPosition(0, 0, -450, 0, -1, 0, 0);
	viewer->registerAreaPickingCallback(pp_callback, (void*)&cloud);

	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}


// Mutex: //
boost::mutex cloud_mutex;

struct callback_args {
	// structure used to pass arguments to the callback function
	PointCloudT::Ptr clicked_points_3d;
	pcl::visualization::PCLVisualizer::Ptr viewerPtr;
};

void
pp_callback(const pcl::visualization::PointPickingEvent& event, void* args)
{
	struct callback_args* data = (struct callback_args *)args;
	if (event.getPointIndex() == -1)
		return;
	PointT current_point;
	event.getPoint(current_point.x, current_point.y, current_point.z);
	data->clicked_points_3d->points.push_back(current_point);
	// Draw clicked points in red:
	pcl::visualization::PointCloudColorHandlerCustom<PointT> red(data->clicked_points_3d, 255, 0, 0);
	data->viewerPtr->removePointCloud("clicked_points");
	data->viewerPtr->addPointCloud(data->clicked_points_3d, red, "clicked_points");
	data->viewerPtr->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
	std::cout << current_point.x << " " << current_point.y << " " << current_point.z << std::endl;
}

void main231()
{
	std::string filename("bunny.pcd");
	//visualizer
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>());
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("viewer"));

	if (pcl::io::loadPLYFile("E:\\my_projects\\PCL_3d\\data\\01.ply", *cloud))
	{
		std::cerr << "ERROR: Cannot open file " << std::endl;
		return;
	}
	std::cout << cloud->points.size() << std::endl;

	//viewer->addPointCloud(cloud, "bunny");

	cloud_mutex.lock();    // for not overwriting the point cloud

						   // Display pointcloud:
	viewer->addPointCloud(cloud, "bunny");
	viewer->setCameraPosition(0, 0, -450, 0, -1, 0, 0);

	// Add point picking callback to viewer:
	struct callback_args cb_args;
	PointCloudT::Ptr clicked_points_3d(new PointCloudT);
	cb_args.clicked_points_3d = clicked_points_3d;
	cb_args.viewerPtr = pcl::visualization::PCLVisualizer::Ptr(viewer);
	viewer->registerPointPickingCallback(pp_callback, (void*)&cb_args);
	std::cout << "Shift+click on three floor points, then press 'Q'..." << std::endl;

	// Spin until 'Q' is pressed:
	viewer->spin();
	std::cout << "done." << std::endl;

	cloud_mutex.unlock();

	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
}