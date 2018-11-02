#include "stdafx.h"
#include "FileIOP.h"
#include <fstream>
#include <io.h>
#include <cctype>
#include <vector>
#include "VStream.h"

using namespace std;

FileIOP::FileIOP()
{
}


FileIOP::~FileIOP()
{
}

VStream vs;
void FileIOP::getFiles(std::string path, std::vector<std::string>& files)
{
	//文件句柄  
	intptr_t  hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

}
void AppendPointsStrVector(vector<string> &pointsStrVector0, const vector<string> &pointsStrVector1)
{
	int size = pointsStrVector1.size();
	for (int i = 0; i < size; i++)
	{
		pointsStrVector0.push_back(pointsStrVector1[i]);
	}
}
void AppendFacesStrVector(vector<string> &facesStrVector0, const int pre_pointNum, const int texture_index,
	const vector<string> &facesStrVector1)
{
	int size = facesStrVector1.size();
	for (int i = 0; i < size; i++)
	{
		vector<string> newLine;
		vs.SplitString(facesStrVector1[i], newLine, ' ');

		int vertex1 = vs.FromString<int>(newLine[1]) + pre_pointNum;
		int vertex2 = vs.FromString<int>(newLine[2]) + pre_pointNum;
		int vertex3 = vs.FromString<int>(newLine[3]) + pre_pointNum;

		newLine[1] = vs.ToString<int>(vertex1);
		newLine[2] = vs.ToString<int>(vertex2);
		newLine[3] = vs.ToString<int>(vertex3);

		newLine[11] = vs.ToString<int>(texture_index);

		vector<string> newLine2;
		for (int j = 0; j < 12; j++)
		{
			newLine2.push_back(newLine[j]);
		}
		newLine2.push_back("1");
		newLine2.push_back("1");
		newLine2.push_back("1");
		newLine2.push_back("1");
		string mergeLine = vs.MergeString(newLine2, ' ');
		facesStrVector0.push_back(mergeLine);
	}
}
int GetNumStartLinePos(const vector<string> & context)
{
	for (int i = 0; i < context.size(); i++)
	{
		if (context[i] == "end_header")
		{
			return (i + 1);
		}
		if (std::isdigit(context[i][0]) || std::isdigit(context[i][1]) || std::isdigit(context[i][2]))
		{
			return (i + 1);
		}
	}
}
void GetPointAndFaceStrFromFile(const vector<string> & context, const int pointsNum,
	vector<string> & headerStrVector, vector<string> & pointsStrVector, vector<string> & faceStrVector)
{

	int startPos = GetNumStartLinePos(context);
	for (int i = 0; i < startPos; ++i)
	{
		headerStrVector.push_back(context[i]);
	}

	for (int i = startPos; i < pointsNum + startPos; i++)
	{
		pointsStrVector.push_back(context[i]);
	}

	for (int i = pointsNum + startPos; i < context.size(); i++)
	{
		faceStrVector.push_back(context[i]);
	}

}



void WritePlyText(vector<string> &new_context, const vector<string> &pointsStrVector0,
	const vector<string> &faceStrVector0, const vector<string> &textureFiles)
{
	new_context.push_back("ply");
	new_context.push_back("format ascii 1.0");
	new_context.push_back("comment VCGLIB generated");
	for (int i = 0; i < textureFiles.size(); i++)
	{
		new_context.push_back(textureFiles[i]);
	}
	string new_pointNum =  to_string(pointsStrVector0.size());
	//string new_pointNum = databaseStream.ToString<int>(pointsStrVector0.size());
	new_context.push_back("element vertex " + new_pointNum);

	new_context.push_back("property float x");
	new_context.push_back("property float y");
	new_context.push_back("property float z");
	new_context.push_back("property uchar red");
	new_context.push_back("property uchar green");
	new_context.push_back("property uchar blue");
	new_context.push_back("property uchar alpha");
	string new_faceNum = to_string(faceStrVector0.size());
	//string new_faceNum = databaseStream.ToString<int>(faceStrVector0.size());
	new_context.push_back("element face " + new_faceNum);

	new_context.push_back("property list uchar int vertex_indices");
	new_context.push_back("property list uchar float texcoord");
	new_context.push_back("property int texnumber");
	new_context.push_back("property uchar red");
	new_context.push_back("property uchar green");
	new_context.push_back("property uchar blue");
	new_context.push_back("property uchar alpha");
	new_context.push_back("end_header");
	for (int i = 0; i < pointsStrVector0.size(); i++)
	{
		new_context.push_back(pointsStrVector0[i]);
	}

	for (int i = 0; i < faceStrVector0.size(); i++)
	{
		new_context.push_back(faceStrVector0[i]);
	}
}
//将多个ply文件融合为一个文件
//void MergePlyFiles(const vector<string> &files, const string &outPathFile)
//{
//	vector<string> context0;
//	databaseStream.ReadTextFile(files[0], context0);
//	vector<string> pointsStrVector0;
//	vector<string> faceStrVector0;
//	int startPos = GetNumStartLinePos(context0);
//
//	//收集ply纹理图片信息
//	vector<string> textureFiles;
//	textureFiles.push_back(context0[3]);
//	std::vector<string> textLine;
//	databaseStream.SplitString(context0[4], textLine, ' ');
//	int pointsNum0 = atoi(textLine[2].c_str());
//	//int pointsNum0 = databaseStream.FromString<int>(textLine[2]);
//	GetPointAndFaceStrFromFile(context0, pointsNum0, pointsStrVector0, faceStrVector0);
//
//	vector<string> faceStrVectorSet;
//	AppendFacesStrVector(faceStrVectorSet, 0, 0, faceStrVector0);
//	int file_num = files.size();
//	if (file_num > 1)
//	{
//		for (int i = 1; i < file_num; i++)
//		{
//			int pre_pointsNum = pointsStrVector0.size();
//			vector<string> context;
//			databaseStream.ReadTextFile(files[i], context);
//			textureFiles.push_back(context[3]);
//			vector<string> pointsStrVector;
//			vector<string> faceStrVector;
//			std::vector<string> textLine_new;
//
//			databaseStream.SplitString(context[4], textLine_new, ' ');
//			int pointsNum_new = databaseStream.FromString<int>(textLine_new[2]);
//			GetPointAndFaceStrFromFile(context, pointsNum_new, pointsStrVector, faceStrVector);
//
//			AppendPointsStrVector(pointsStrVector0, pointsStrVector);
//			AppendFacesStrVector(faceStrVectorSet, pre_pointsNum, i, faceStrVector);
//			pointsStrVector.clear();
//		}
//	}
//	vector<string> new_context;
//	WritePlyText(new_context, pointsStrVector0, faceStrVectorSet, textureFiles);
//	databaseStream.WriteTextFile(outPathFile, new_context);
//}
void getPointsFromFile(const vector<string> & context, const int pointsNum,
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds)
{
	bool isPoint = false;
	int pointCount = 0;
	for (int j = 0; j < context.size(); j++)
	{
		if (isPoint && pointCount < pointsNum)
		{
			pcl::PointXYZRGBA newpt;
			vector<string> pointXYZ;
			vs.SplitString(context[j], pointXYZ, ' ');
			newpt.x = atof(pointXYZ[0].c_str());
			newpt.y = atof(pointXYZ[1].c_str());
			newpt.z = atof(pointXYZ[2].c_str());
			newpt.r = atof(pointXYZ[3].c_str());
			newpt.g = atof(pointXYZ[4].c_str());
			newpt.b = atof(pointXYZ[5].c_str());
			newpt.a = atof(pointXYZ[6].c_str());
			//float u = vs.FromString<float>(pointXYZ[3]);
			//float v = vs.FromString<float>(pointXYZ[4]);
			ptClouds->push_back(newpt);
			//facePointsColor.AddVertex(x, y, z);
			//facePointsColorUV.AddVertex(u, v, 0);
			pointCount++;
		}
		if (context[j] == "end_header")
		{
			isPoint = true;
		}
		if (pointCount == pointsNum)
		{
			isPoint = false;
		}
	}
}
//void MergePlyFiles(const vector<string> &files, const string &outPathFile)
//{
//	vector<string> context0;
//	vs.ReadTextFile(files[0], context0);
//	vector<string> pointsStrVector0;
//	vector<string> faceStrVector0;
//	int startPos = GetNumStartLinePos(context0);
//
//	//收集ply纹理图片信息
//	vector<string> textureFiles;
//	textureFiles.push_back(context0[3]);
//	std::vector<string> textLine;
//	vs.SplitString(context0[4], textLine, ' ');
//	int pointsNum0 = vs.FromString<int>(textLine[2]);
//	GetPointAndFaceStrFromFile(context0, pointsNum0, pointsStrVector0, faceStrVector0);
//
//	vector<string> faceStrVectorSet;
//	AppendFacesStrVector(faceStrVectorSet, 0, 0, faceStrVector0);
//	int file_num = files.size();
//	if (file_num > 1)
//	{
//		for (int i = 1; i < file_num; i++)
//		{
//			int pre_pointsNum = pointsStrVector0.size();
//			vector<string> context;
//			vs.ReadTextFile(files[i], context);
//			textureFiles.push_back(context[3]);
//			vector<string> pointsStrVector;
//			vector<string> faceStrVector;
//			std::vector<string> textLine_new;
//
//			vs.SplitString(context[4], textLine_new, ' ');
//			int pointsNum_new = vs.FromString<int>(textLine_new[2]);
//			GetPointAndFaceStrFromFile(context, pointsNum_new, pointsStrVector, faceStrVector);
//
//			AppendPointsStrVector(pointsStrVector0, pointsStrVector);
//			AppendFacesStrVector(faceStrVectorSet, pre_pointsNum, i, faceStrVector);
//			pointsStrVector.clear();
//		}
//	}
//	vector<string> new_context;
//	WritePlyText(new_context, pointsStrVector0, faceStrVectorSet, textureFiles);
//	vs.WriteTextFile(outPathFile, new_context);
//}

void FileIOP::getPointsFromStrVec(const vector<string> & ptstrVec,
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds)
{
	int pointsNum = ptstrVec.size();
	int pointCount = 0;
	for (int j = 0; j < pointsNum; j++)
	{
		
		pcl::PointXYZRGBA newpt;
		vector<string> pointXYZ;
		vs.SplitString(ptstrVec[j], pointXYZ, ' ');
		newpt.x = atof(pointXYZ[0].c_str());
		newpt.y = atof(pointXYZ[1].c_str());
		newpt.z = atof(pointXYZ[2].c_str());
		newpt.r = atof(pointXYZ[3].c_str());
		newpt.g = atof(pointXYZ[4].c_str());
		newpt.b = atof(pointXYZ[5].c_str());
		newpt.a = atof(pointXYZ[6].c_str());
		ptClouds->push_back(newpt);
	}
}

void FileIOP::read3dmesh(const string &inputfile, vector<string> &headerStrVector,
	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds,
	 vector<string> &faceStrVector)
{
	std::vector<string> context;
	vs.ReadTextFile(inputfile, context);
	int pointsNum = 0;
	for (int i = 0; i < 10; i++)
	{
		std::vector<string> textLine;
		vs.SplitString(context[i], textLine, ' ');\
		if (textLine.size() > 1)
		{
			if (textLine[1] == "vertex")
			{
				pointsNum = std::atoi(textLine[2].c_str());
			}
		}	
	}
	
	//pcl::PointCloud<pcl::PointXYZ>::Ptr ptClouds(new pcl::PointCloud<pcl::PointXYZ>());
	vector<string> pointsStrVector;
	//vector<string> faceStrVector;
	GetPointAndFaceStrFromFile(context, pointsNum, headerStrVector,pointsStrVector, faceStrVector);
	getPointsFromStrVec(pointsStrVector, ptClouds);
}


void write3Dmesh(const vector<string> &inputfile, const string &outputfile)
{
	vs.WriteTextFile(outputfile, inputfile);
}

void FileIOP::writeNewPly(const std::vector<std::string>&headerStrVec, const std::vector<std::string> &pointstrvec,
	const std::vector<std::string> &facestrvec,
	const std::string &outpath)
{
	VStream vs;
	std::vector<std::string> fileStrVec;
	int header_lines = headerStrVec.size();
	for (int i = 0; i < header_lines; i++)
	{
		std::string new_line = headerStrVec[i];
		std::vector<std::string> lineStrVec;
		vs.SplitString(new_line, lineStrVec, ' ');
		if (lineStrVec.size() > 1)
		{
			if (lineStrVec[1] == "vertex")
			{
				lineStrVec[2] = std::to_string(pointstrvec.size());
				new_line = vs.MergeString(lineStrVec, ' ');
			}
			else if (lineStrVec[1] == "face")
			{
				lineStrVec[2] = std::to_string(facestrvec.size());
				new_line = vs.MergeString(lineStrVec, ' ');
			}
		}
		fileStrVec.push_back(new_line);
	}
	int ptsize = pointstrvec.size();
	for (int i = 0; i < ptsize; ++i)
	{
		fileStrVec.push_back(pointstrvec[i]);
	}
	int facesize = facestrvec.size();
	for (int i = 0; i < facesize; ++i)
	{
		fileStrVec.push_back(facestrvec[i]);
	}
	vs.WriteTextFile(outpath, fileStrVec);
}


void FileIOP::convertPtToStrVec(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ptClouds, vector<string> &ptStrVector)
{
	int pt_size = ptClouds->size();
	for (int i = 0; i < pt_size; ++i)
	{
		vector<string> newStrVector;
		newStrVector.push_back(std::to_string(ptClouds->at(i).x));
		newStrVector.push_back(std::to_string(ptClouds->at(i).y));
		newStrVector.push_back(std::to_string(ptClouds->at(i).z));
		newStrVector.push_back(std::to_string(ptClouds->at(i).r));
		newStrVector.push_back(std::to_string(ptClouds->at(i).g));
		newStrVector.push_back(std::to_string(ptClouds->at(i).b));
		newStrVector.push_back(std::to_string(ptClouds->at(i).a));
		string newline = vs.MergeString(newStrVector, ' ');
		ptStrVector.push_back(newline);
	}
}