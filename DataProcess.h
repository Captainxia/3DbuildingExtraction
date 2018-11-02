#pragma once
#include <string>
#include "pcl\point_types.h"
#include "pcl\point_cloud.h"
class DataProcess
{
public:
	DataProcess();
	~DataProcess();
public:
	void DealGISPoints();
	void GisptNormalization(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr gispts_cloud,
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr newpts_cloud);
	void GisptNormalization(const pcl::PointCloud<pcl::PointXYZ>::Ptr gispts_cloud,
		pcl::PointCloud<pcl::PointXYZ>::Ptr newpts_cloud);
	void GisptNormalization(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr gispts_cloud,
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr newpts_cloud, bool isdisplay);

private:
	std::string _gisptsPath;
	std::string _3dptsPath;
	float _scale;
	const double BaseLatitude = 22.59;//坐标基准值
	const double BaseLongitude = 113.97;

};

