#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

	//�ڲ�ָ����
class VStreamThe;
//���ݶ�ȡ��
//ʵ��obj,ply�ļ��Ķ�ȡ��д��
class  VStream
{
public:
	VStream(void);
	~VStream(void);
public:
	//��ȡ�ı��ļ�
	void ReadTextFile(const std::string &path, std::vector<std::string>& context);
	//д���ı��ļ�
	void WriteTextFile(const std::string &path, const std::vector<std::string> &context, bool isNew = true);
	//��ȡ�������ݣ����ļ�
	//void ReadObjFile(const std::string& path, Lm3dSpace::VertexList<float>& pts);
	////��ȡ�������ݣ����ļ�
	//void ReadPlyFile(const std::string& path, Lm3dSpace::VertexList<float>& pts);
	////д��������ݣ����ļ�
	//void WriteObjFile(const std::string& path, const Lm3dSpace::VertexList<float>& pts);
	////д��������ݣ����ļ�
	//void WritePlyFile(const std::string& path, const Lm3dSpace::VertexList<float>& pts);
	////��ȡ��ɫ�������ݣ����ļ�
	//void ReadPlyFile(const std::string& path, Lm3dSpace::Points<float>& pts);
	////д���ɫ�������ݣ����ļ�
	//void WritePlyFile(const std::string& path, const Lm3dSpace::Points<float>& pts);
public:
	//����ַ���
	void SplitString(const std::string &context, std::vector<std::string>& texts, const char separator) const;
	//�ں��ַ���
	std::string MergeString(const std::vector<std::string> &inputvector, const char separator) const;
	//д��������ݣ����ļ�,����ɫ
	//void WritePtsImage(const std::string& imageFile, 
	//	const Lm3dSpace::Points<float>& pts,
	//	int &imageWidth, 
	//	int &imageHeight, 
	//	std::vector<int> &pointIndexes);
	////����ͼƬ
	//void WriteBmpImage(const std::string& imageFile, VColor *imageData, int width, int height);
public://template function
	//��������ת��Ϊ�ַ���
	template<typename T>
	static std::string ToString(T value)
	{
		std::stringstream  ss;
		ss << value;
		std::string to_string;
		ss >> to_string;
		return to_string;
	}
	//�ַ���ת��Ϊ��������
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

	//���ļ��ж�ȡ����
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
	//���ļ��ж�ȡ����
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
	//���ļ���д������
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
	//		//�����ַ�
	//		this->WriteToTempBuffer(nextLine, pos);
	//	}
	//	this->WriteFileFromBuffer(path, pos);
	//}
	//���ļ���д������
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
