#include "stdafx.h"
#include "DataProcess.h"
#include <pcl/io/ply_io.h>
#include <vector>
#include "FileIOP.h"


DataProcess::DataProcess()
{
	_gisptsPath = "E:\\3d\\data\\controlpoints\\PtsGIS.ply";
	_3dptsPath = "E:\\3d\\data\\controlpoints\\pts3D.ply";
	_scale = 0.0;
}


DataProcess::~DataProcess()
{
}
//double CrossProduction(const pcl::PointXYZ &pointa, const pcl::PointXYZ &pointb)
//{
//	return pointa.x* pointb.y - pointa.y*pointb.x;
//}
//void Get2dBox(const pcl::PointXYZ &pointa, const pcl::PointXYZ &pointb, const pcl::PointXYZ &pointc, 
//	pcl::PointXYZ &minp, pcl::PointXYZ &maxp)
//{
//	double maxX = pointa.x > pointb.x? pointa.x :pointb.x;
//	maxX = maxX > pointc.x ? maxX : pointc.x;
//	double maxY = pointa.y > pointb.y ? pointa.y : pointb.y;
//	maxY = maxY > pointb.y ? maxY : pointb.y;
//
//	double minX = pointa.x < pointb.x ? pointa.x : pointb.x;
//	minX = maxX < pointc.x ? maxX : pointc.x;
//	double minY = pointa.y < pointb.y ? pointa.y : pointb.y;
//	minY = minY < pointb.y ? maxY : pointb.y;
//
//	minp.x = minX;
//	minp.y = minY;
//	maxp.x = maxX;
//	maxp.y = maxY;
//}
//bool IsInbox(const pcl::PointXYZ &minp, const pcl::PointXYZ &maxp, const pcl::PointXYZ &p)
//{
//	if (p.x >= minp.x && p.x <= maxp.x)
//	{
//		if (p.y >= minp.y && p.y <= maxp.y)
//		{
//			return true;
//		}
//	}
//	return false;
//}
//bool IsInTriangle(const pcl::PointXYZ &pointa, const pcl::PointXYZ &pointb, const pcl::PointXYZ &pointc, const pcl::PointXYZ &p)
//{
//	pcl::PointXYZ ab(pointa.x - pointb.x, pointa.y - pointb.y, 0.0);
//	pcl::PointXYZ ac(pointa.x - pointc.x, pointa.y - pointc.y, 0.0);
//	pcl::PointXYZ ap(pointa.x - p.x, pointa.y - p.y, 0.0);
//
//	//Vertex2D<float> ab(pointb - pointa), ac(pointc - pointa), ap(p - pointa);
//	//用矢量积计算面积，下面4个值的绝对值，是对应的三角形的面积的两倍，
//	double abc = CrossProduction(ab, ac);//ab.cross(ac);
//	double abp = CrossProduction(ab, ap);//ab.cross(ap);
//	double apc = CrossProduction(ap, ac); //ap.cross(ac);
//	double pbc = abc - abp - apc;
//	//等于pb.cross(pc)			
//	//面积法：4个三角形的面积差 等于 0
//	double delta = fabs(abc) - fabs(abp) - fabs(apc) - fabs(pbc);
//	double dbl_EPSILON = 0.00000001;
//	return fabs(delta) < dbl_EPSILON;
//}

double CalDistance(pcl::PointXYZ pt1, pcl::PointXYZ pt2)
{
	double deltx = pt1.x - pt2.x;
	double delty = pt1.y - pt2.y;
	double deltz = pt1.z - pt2.z;
	double sum = deltx*deltx + delty*delty + deltz*deltz;
	double dis = sqrt(sum);
	return dis;
}
void movept(pcl::PointXYZ pt1, const double x, const double y, pcl::PointXYZ &pt2)
{
	pt2.x = pt1.x - x;
	pt2.y = pt1.y - y;
	pt2.z = pt1.z - 0.0;
}
void movept(const pcl::PointXYZRGBA &pt1, const double x, const double y, pcl::PointXYZRGBA &pt2)
{
	pt2.x = pt1.x - x;
	pt2.y = pt1.y - y;
	pt2.z = pt1.z - 0.0;
	pt2.r = pt1.r;
	pt2.g = pt1.g;
	pt2.b = pt1.b;
	pt2.a = pt1.a;

}
void DataProcess::GisptNormalization(const pcl::PointCloud<pcl::PointXYZ>::Ptr gispts_cloud,
	pcl::PointCloud<pcl::PointXYZ>::Ptr newpts_cloud)
{
	if (this->_scale == 0.0)
	{
		return;
	}
	else
	{
		int size = gispts_cloud->size();
		for (int i = 0; i < size; i++)
		{
			pcl::PointXYZ newp;
			movept(gispts_cloud->at(i), this->BaseLatitude, this->BaseLongitude, newp);
			newp.x = newp.x* this->_scale;
			newp.y = newp.y * this->_scale;
			newp.z = newp.z * this->_scale;
			newpts_cloud->push_back(newp);
		}

	}
}
void DataProcess::GisptNormalization(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr gispts_cloud,
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr newpts_cloud)
{
	if (this->_scale == 0.0)
	{
		return;
	}
	else
	{
		int size = gispts_cloud->size();
		for (int i = 0; i < size; i++)
		{
			pcl::PointXYZRGBA newp;
			movept(gispts_cloud->at(i), this->BaseLatitude, this->BaseLongitude, newp);
			newp.x = newp.x* this->_scale;
			newp.y = newp.y * this->_scale;
			newp.z = newp.z * this->_scale;
			
			newpts_cloud->push_back(newp);
			

		}

	}
}
void DataProcess::GisptNormalization(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr gispts_cloud,
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr newpts_cloud, bool isdisplay)
{
	if (this->_scale == 0.0)
	{
		return;
	}
	else
	{
		int size = gispts_cloud->size();
		for (int i = 0; i < size; i++)
		{
			pcl::PointXYZRGBA newp;
			movept(gispts_cloud->at(i), this->BaseLatitude, this->BaseLongitude, newp);
			newp.x = newp.x* this->_scale;
			newp.y = newp.y * this->_scale;
			newp.z = newp.z * this->_scale;

			newpts_cloud->push_back(newp);
			pcl::PointXYZRGBA newp1(newp);
			newp1.z = 2.0;
			newpts_cloud->push_back(newp1);
		}

	}
}
//处理建筑物边界GIS坐标点，进行缩放比例计算，以及（类）均值化处理（都减去一个基准值，定义在类头文件中）
void DataProcess::DealGISPoints()
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr gispts_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(this->_gisptsPath, *gispts_cloud);
	pcl::PointCloud<pcl::PointXYZ>::Ptr threeDpts_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(this->_3dptsPath, *threeDpts_cloud);
	int size = gispts_cloud->size();
	std::vector<double> gisdisSet;
	for (int i = 0; i < size/2; i++)
	{
		double dis = CalDistance(gispts_cloud->at(i), gispts_cloud->at(size - 1 - i));
		gisdisSet.push_back(dis);
	}

	std::vector<double> threeDdisSet;
	for (int i = 0; i < size / 2; i++)
	{
		double dis = CalDistance(threeDpts_cloud->at(i), threeDpts_cloud->at(size - 1 - i));
		threeDdisSet.push_back(dis);
	}
	
	double sum = 0.0;
	for (int i = 0; i < size / 2; i++)
	{
		double scale = threeDdisSet[i] / gisdisSet[i];
		sum += scale;
	}

	this->_scale = sum / (size / 2);
	pcl::PointCloud<pcl::PointXYZ>::Ptr newGispts_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	this->GisptNormalization(gispts_cloud, newGispts_cloud);
	std::string outfile = "E:\\3d\\data\\controlpoints\\newGispts.ply";
	pcl::io::savePLYFile(outfile, *newGispts_cloud);

	//define bbox file root
	//normalize bbox data
	std::string boundingFileRoot = "E:\\3d\\data\\boundingbox\\new";
	int rootlength = boundingFileRoot.size();
	FileIOP fio;
	std::string outfileroot = "E:\\3d\\data\\boundingbox\\output";
	
	std::vector<std::string> bboxfiles;
	fio.getFiles(boundingFileRoot, bboxfiles);
	for (int i =0; i <bboxfiles.size(); i++)
	{
		std::string curfile = bboxfiles[i];
		int new_size = curfile.size();
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr newPts_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
		pcl::io::loadPLYFile(curfile, *newPts_cloud);
		std::string newoutfile;
		for (int k = rootlength; k < new_size; k++)
		{
			newoutfile += curfile[k];
		}
		newoutfile = outfileroot + newoutfile;
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr outPts_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
		this->GisptNormalization(newPts_cloud, outPts_cloud);
		pcl::io::savePLYFile(newoutfile, *outPts_cloud);
	}
}