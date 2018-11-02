#pragma once
#include "FileIOP.h"
#include "pcl\point_cloud.h"
#include "pcl\point_types.h"
#include "pcl\IO\ply_io.h"
#include "pcl\IO\obj_io.h"
#include <string>
class Extraction
{
public:
	Extraction();
	~Extraction();
public:
	void ExtractBuidlingClouds(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &input_cloud_pt,
		const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &bbox_pt,
		const std::vector<std::string> &inputface, std::vector<std::string> &newpointCloudStr,
		std::vector<std::string> &faceOutStrVec);

	//void ExtractMesh();
};

