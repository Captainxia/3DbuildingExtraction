#pragma once
#include <vector>
#include <string>
#include "pcl\point_cloud.h"
#include "pcl\point_types.h"
using namespace std;
class FileIOP
{
public:
	FileIOP();
	~FileIOP();
public:
	void getFiles(std::string path, std::vector<std::string>& files);
	//void GetPointAndFaceStrFromFile(const vector<string> & context, const int pointsNum,
	//	vector<string> & pointsStrVector, vector<string> & faceStrVector);

	void read3dmesh(const string &inputfile, vector<string> &headerStrVector,
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds,
		vector<string> &faceStrVector);

	void writeNewPly(const std::vector<std::string>&headerStrVec, const std::vector<std::string> &pointstrvec,
		const std::vector<std::string> &facestrvec,const std::string &outpath);

	void convertPtToStrVec(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds, vector<string> &ptStrVector);

	void getPointsFromStrVec(const vector<string> & ptstrVec,
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds);
};

