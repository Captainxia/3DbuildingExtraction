#include "stdafx.h"
#include <pcl/io/ply_io.h>
#include "pcl\point_types.h"
#include "pcl\point_cloud.h"
#include <pcl/visualization/pcl_visualizer.h>

#include "pcl\registration\icp.h"
#include <string>
#include <iostream>
#include <vector>
#include "DataProcess.h"
#include "Extraction.h"
#include "VStream.h"
void TransCor()
{
	//std::string file1 = "E:\\3d\\data\\controlpoints\\sourcePt1.ply";
	//std::string file1 = "E:\\3d\\Data171017\\Tile_+001_+002\\Tile_+001_+002cutp1.ply";
	std::string sourcefile = "E:\\3d\\data\\controlpoints\\pts3D.ply";
	pcl::PointCloud<pcl::PointXYZ>::Ptr source_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(sourcefile, *source_cloud);
	
	std::string targetfile = "E:\\3d\\data\\controlpoints\\newGispts.ply";
	//std::string file2 = "E:\\3d\\data\\controlpoints\\targetPt1.ply";
	pcl::PointCloud<pcl::PointXYZ>::Ptr target_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(targetfile, *target_cloud);

	pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ> Test;
	pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ>::Matrix4 transformation;
	Test.estimateRigidTransformation(*source_cloud, *target_cloud, transformation);

	pcl::PointCloud<pcl::PointXYZ>::Ptr out_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::transformPointCloud(*source_cloud, *out_cloud, transformation);
	std::string outfile = "E:\\3d\\data\\controlpoints\\outPt.ply";
	pcl::PLYWriter plyw;
	plyw.write(outfile, *out_cloud);

	//tranform 3d data
	std::string file3 = "E:\\3d\\Data171017\\Tile_+001_+002\\Tile_+001_+002.ply";
	pcl::PointCloud<pcl::PointXYZ>::Ptr input_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(file3, *input_cloud);

	pcl::PointCloud<pcl::PointXYZ>::Ptr out_cloud1(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::transformPointCloud(*input_cloud, *out_cloud1, transformation);

	std::string outfile1 = "E:\\3d\\data\\controlpoints\\outPt1.ply";
	plyw.write(outfile1, *out_cloud1);

	std::string file4 = "E:\\3d\\Data171017\\Tile_+002_+001\\Tile_+002_+001.ply";
	pcl::PointCloud<pcl::PointXYZ>::Ptr input_cloud2(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(file4, *input_cloud2);

	pcl::PointCloud<pcl::PointXYZ>::Ptr out_cloud2(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::transformPointCloud(*input_cloud2, *out_cloud2, transformation);

	std::string outfile2 = "E:\\3d\\data\\controlpoints\\outPt2.ply";
	plyw.write(outfile2, *out_cloud2);

}

void dataprocess()
{
	DataProcess dp;
	dp.DealGISPoints();
}

//计算刚体变换矩阵，从点云到GIS坐标
void GetTransCorMat(pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ>::Matrix4 &transformation)
{
	std::string sourcefile = "E:\\3d\\data\\controlpoints\\pts3D.ply";
	pcl::PointCloud<pcl::PointXYZ>::Ptr source_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(sourcefile, *source_cloud);

	std::string targetfile = "E:\\3d\\data\\controlpoints\\newGispts.ply";
	pcl::PointCloud<pcl::PointXYZ>::Ptr target_cloud(new pcl::PointCloud<pcl::PointXYZ>());
	pcl::io::loadPLYFile(targetfile, *target_cloud);

	pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ> Test;
	//pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ>::Matrix4 transformation;
	Test.estimateRigidTransformation(*source_cloud, *target_cloud, transformation);
}
//变换点云然后输出
inline void transformPt(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr in_cloud, pcl::PointCloud<pcl::PointXYZRGBA>::Ptr out_cloud,
	const pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ>::Matrix4 &transformation)
{
	pcl::transformPointCloud(*in_cloud, *out_cloud, transformation);
}
//正式处理点云转换的函数，实现将点云通过刚体变换到新的坐标系并写成新的文件
void ConvertPtcloudsFile(const std::string &fileroot, const std::string &fileoutroot)
{
	VStream vs;
	FileIOP fio;
	std::vector<std::string> inputfiles;
	fio.getFiles(fileroot,inputfiles);
	pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ>::Matrix4 transformation;
	GetTransCorMat(transformation);
	int file_num = inputfiles.size();
	int fileroot_len = fileroot.size();
	for (int i = 0; i < file_num; ++i)
	{
		std::string curfile = inputfiles[i];
		std::vector< std::string> headerStrVec;
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr inPts_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
		std::vector< std::string> faceVec;
		fio.read3dmesh(curfile, headerStrVec, inPts_cloud, faceVec);
		int new_size = curfile.size();
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr outPts_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
		transformPt(inPts_cloud, outPts_cloud, transformation);
		std::vector< std::string> ptVec;
		fio.convertPtToStrVec(outPts_cloud, ptVec);
		//pcl::io::loadPLYFile(curfile, *newPts_cloud);
		std::string newoutfile;
		for (int k = fileroot_len; k < new_size; k++)
		{
			newoutfile += curfile[k];
		}
		newoutfile = fileoutroot + newoutfile;
		fio.writeNewPly(headerStrVec, ptVec, faceVec, newoutfile);
	}
}
void ExtractPtclouds(const std::string &fileroot, const std::string &bboxfilepath, const std::string &extractFileroot)
{
	VStream vs;
	FileIOP fio;
	Extraction extract;
	std::vector<std::string> inputfiles;
	fio.getFiles(fileroot, inputfiles);
	int file_num = inputfiles.size();
	
	for (int i = 0; i < file_num; ++i)
	{
		std::string curfile = inputfiles[i];
		std::vector< std::string> headerStrVec;
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr inPts_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
		std::vector< std::string> faceVec;
		fio.read3dmesh(curfile, headerStrVec, inPts_cloud, faceVec);
		
		std::vector<std::string> outStrVec;
		std::vector<std::string> bboxfiles;
		fio.getFiles(bboxfilepath, bboxfiles);
		int bboxfile_num = bboxfiles.size();
		int fileroot_len = bboxfilepath.size();
		for (int j = 0; j < bboxfile_num; ++j)
		{
			pcl::PointCloud<pcl::PointXYZRGBA>::Ptr bbox_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
			pcl::io::loadPLYFile(bboxfiles[j], *bbox_cloud);
			std::vector<std::string> outPtStrVec;
			std::vector<std::string> outFaceStrVec;
			extract.ExtractBuidlingClouds(inPts_cloud, bbox_cloud, faceVec, outPtStrVec, outFaceStrVec);
			std::string newoutfile;
			int new_size = bboxfiles[j].size();
			for (int k = fileroot_len; k < new_size; k++)
			{
				newoutfile += bboxfiles[j][k];
			}
			newoutfile = extractFileroot + newoutfile; 
			fio.writeNewPly(headerStrVec, outPtStrVec, outFaceStrVec, newoutfile);
		}
	}
}
void SingleExtractPtclouds(const std::string &meshfilePath, const std::string &bboxfilepath, const std::string &extractFileroot)
{
	VStream vs;
	FileIOP fio;
	Extraction extract;
	std::vector< std::string> headerStrVec;
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr inPts_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
	std::vector< std::string> faceVec;
	fio.read3dmesh(meshfilePath, headerStrVec, inPts_cloud, faceVec);
	std::vector<std::string> outStrVec;
	std::vector<std::string> bboxfiles;
	fio.getFiles(bboxfilepath, bboxfiles);
	int bboxfile_num = bboxfiles.size();
	int fileroot_len = bboxfilepath.size();
	for (int j = 0; j < bboxfile_num; ++j)
	{
		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr bbox_cloud(new pcl::PointCloud<pcl::PointXYZRGBA>());
		pcl::io::loadPLYFile(bboxfiles[j], *bbox_cloud);
		std::vector<std::string> outPtStrVec;
		std::vector<std::string> outFaceStrVec;
		extract.ExtractBuidlingClouds(inPts_cloud, bbox_cloud, faceVec, outPtStrVec, outFaceStrVec);
		std::string newoutfile;
		int new_size = bboxfiles[j].size();
		for (int k = fileroot_len; k < new_size; k++)
		{
			newoutfile += bboxfiles[j][k];
		}
		newoutfile = extractFileroot + newoutfile;
		fio.writeNewPly(headerStrVec, outPtStrVec, outFaceStrVec, newoutfile);
	}	
}
void extractiontest()
{
	std::string inputmeshfile = "E:\\3d\\output\\ply_convert\\01\\Tile_+002_+002.ply";
	std::string bboxfile = "E:\\3d\\data\\boundingbox\\output\\02\\2_2";
	std::string extractFileroot = "E:\\3d\\output\\extraction\\2_2";
	SingleExtractPtclouds(inputmeshfile, bboxfile, extractFileroot);
	//ex.ExtractBuidlingClouds(objinputfile, objoutputfile);
}
void converttest()
{
	std::string fileroot = "E:\\3d\\ply\\left" ;
	std::string fileout  = "E:\\3d\\output\\ply_convert";
	ConvertPtcloudsFile(fileroot, fileout);
}
int main()
{
	//TransCor();
	//dataprocess();
	extractiontest();
	//converttest();
}