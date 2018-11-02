#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

	//内部指针类
class VStreamThe;
//数据读取类
//实现obj,ply文件的读取和写入
class  VStream
{
public:
	VStream(void);
	~VStream(void);
public:
	//读取文本文件
	void ReadTextFile(const std::string &path, std::vector<std::string>& context);
	//写入文本文件
	void WriteTextFile(const std::string &path, const std::vector<std::string> &context, bool isNew = true);
	//读取点云数据，从文件
	//void ReadObjFile(const std::string& path, Lm3dSpace::VertexList<float>& pts);
	////读取点云数据，从文件
	//void ReadPlyFile(const std::string& path, Lm3dSpace::VertexList<float>& pts);
	////写入点云数据，到文件
	//void WriteObjFile(const std::string& path, const Lm3dSpace::VertexList<float>& pts);
	////写入点云数据，到文件
	//void WritePlyFile(const std::string& path, const Lm3dSpace::VertexList<float>& pts);
	////读取彩色点云数据，从文件
	//void ReadPlyFile(const std::string& path, Lm3dSpace::Points<float>& pts);
	////写入彩色点云数据，到文件
	//void WritePlyFile(const std::string& path, const Lm3dSpace::Points<float>& pts);
public:
	//拆分字符串
	void SplitString(const std::string &context, std::vector<std::string>& texts, const char separator) const;
	//融合字符串
	std::string MergeString(const std::vector<std::string> &inputvector, const char separator) const;
	//写入点云数据，到文件,带颜色
	//void WritePtsImage(const std::string& imageFile, 
	//	const Lm3dSpace::Points<float>& pts,
	//	int &imageWidth, 
	//	int &imageHeight, 
	//	std::vector<int> &pointIndexes);
	////保存图片
	//void WriteBmpImage(const std::string& imageFile, VColor *imageData, int width, int height);
public://template function
	//类型数据转换为字符串
	template<typename T>
	static std::string ToString(T value)
	{
		std::stringstream  ss;
		ss << value;
		std::string to_string;
		ss >> to_string;
		return to_string;
	}
	//字符串转换为类型数据
	template<typename T>
	static  T FromString(std::string& value)
	{
		std::stringstream  ss;
		ss << value;
		T to_value;
		ss >> to_value;
		return to_value;
	}
public:	
	void ReadDoubleVecList(const std::string &path, std::vector< std::vector<double>>&  vecList);

	void WriteDoubleVecList(const std::string &path, const std::vector<std::vector<double>>& vecList);

	void ReadDoubleVector(const std::string &path, std::vector<double>&  vec);

	void WriteDoubleVector(const std::string &path, const std::vector<double>&  dVector);

	void ReadIntVector(const std::string &path, std::vector<int>&  vec);

	void WriteIntVector(const std::string &path, const std::vector<int>&  iVector);

	//从文件中读取数据
	//template<typename Tx>
	//void ReadVecList(const std::string &path, std::vector< std::vector<Tx>>&  vecList)
	//{
	//	std::vector<std::string>  context;
	//	this->ReadTextFile(path, context);
	//	int size = (int)context.size();
	//	for (int i = 0; i < size; i++)
	//	{
	//		std::vector<std::string> texts;
	//		vecList.push_back(std::vector<Tx>());
	//		std::vector<Tx>& vec = vecList[vecList.size() - 1];
	//		this->SplitString(context[i], texts, ',');
	//		for (int j = 0; j < (int)texts.size(); j++)
	//		{
	//			vec.push_back(FromString<Tx>(texts[j]));
	//		}
	//	}
	//}
	//从文件中读取数据
	//template<typename Tx>
	//void ReadVector(const std::string &path, std::vector<Tx>& txVec)
	//{
	//	std::vector<std::string>  context;
	//	this->ReadTextFile(path, context);
	//	int size = (int)context.size();
	//	for (int i = 0; i < size; i++)
	//	{
	//		std::vector<std::string> texts;

	//		this->SplitString(context[i], texts, ',');
	//		for (int j = 0; j < (int)texts.size(); j++)
	//		{
	//			Tx value = FromString<Tx>(texts[j]);
	//			txVec.push_back(value);
	//		}
	//	}
	//}
	//向文件中写入数据
	//template<typename Tx>
	//void WriteVecList(const std::string &path, std::vector<std::vector<Tx>>& txVecList)
	//{
	//	int pos = 0;
	//	int listSize = int(txVecList.size());
	//	std::string nextLine("\r\n");
	//	for (int i = 0; i < listSize; i++)
	//	{
	//		int vecSize = int(txVecList[i].size());
	//		for (int j = 0; j < vecSize; j++)
	//		{
	//			Tx value = txVecList[i][j];
	//			std::string text = ToString<Tx>(value) + ",";
	//			this->WriteToTempBuffer(text, pos);
	//		}
	//		//换行字符
	//		this->WriteToTempBuffer(nextLine, pos);
	//	}
	//	this->WriteFileFromBuffer(path, pos);
	//}
	//向文件中写入数据
	//template<typename Tx>
	//void WriteVector(const std::string &path, const std::vector<Tx>&  txVec)
	//{
	//	int pos = 0;
	//	int size = int(txVec.size());
	//	for (int i = 0; i < size; i++)
	//	{
	//		Tx value = txVec[i];
	//		std::string text = ToString<Tx>(value) + ",";
	//		this->WriteToTempBuffer(text,pos);
	//	}
	//	this->WriteFileFromBuffer(path, pos);
	//}
	//void WriteFileFromBuffer(const std::string &fileName, int length);

	//void WriteToTempBuffer(const std::string &text,int& startPos);
private:
	VStreamThe * the;
};	
