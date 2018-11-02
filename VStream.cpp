#include "StdAfx.h"
#include "VStream.h"
#include <fstream>
#include <iostream>
#include <cstdio>
using namespace std;

	//内部指针类		
	class VStreamThe
	{
	private:
		const static int BigLineSize = 1 * 1024 * 1024;
		const static int BufferSize = 500 * 1024 * 1024;
	public:
		//const int VStreamThe::BufferSize = 50 * 1024 * 1024;
		//int _bufferSize = 50 * 1024 * 1024;
		//数据存储数组
		float _tempValues[10];
		//临时变量，缓冲数据
		char* _pTempBuffer = NULL;
		//临时数组，表示整行行字符
		char* _pLineText = new char[255];
		//临时数组，表示单数据字符
		char* _pWordText = new char[25];
		//读取超长的一行字符串
		char* _pBigLineText = NULL;
		//临时对象指针
		//Lm3dSpace::VertexList<float> * pTempVList;
		//临时对象指针
		//Lm3dSpace::Points<float> * pTempPoints;

	public:
		VStreamThe():_pTempBuffer(NULL)
		{
			_pTempBuffer = new char[VStreamThe::BufferSize];

			_pBigLineText = new char[VStreamThe::BigLineSize];
		}
		~VStreamThe()
		{
			if (_pTempBuffer != NULL)
			{
				delete[] _pTempBuffer;
				_pTempBuffer = NULL;
			}
			if (_pBigLineText != NULL)
			{
				delete[] _pBigLineText;
				_pBigLineText = NULL;
			}
			delete[] _pLineText;
			delete[] _pWordText;
		}
	public:
		//字符串写入临时缓冲区
		void WriteToTempBuffer(const std::string &text, int& startPos)
		{
			int size = (int)text.size();
			for (int i = 0; i < size; i++)
			{
				*(this->_pTempBuffer + startPos + i) = text[i];
			}
			startPos += size;
		}
		//字符串写入临时缓冲区
		void WriteToTempBuffer(char * targetText,char * sorceText, int sorceSize, int& startPos)
		{
			for (int i = 0; i < sorceSize; i++)
			{
				*(targetText + startPos + i) = sorceText[i];
			}
			startPos += sorceSize;
		}

	/*	void WriteToLineBuffer(const Vertex3D<float> &pt, int & lineSize,char splitChar)
		{
			int floatWidth = 0;
			int linePos = 0;
			this->ftoa(pt.X, this->_pWordText, 20, floatWidth);
			this->WriteToTempBuffer(this->_pLineText, this->_pWordText, floatWidth, linePos);
			*(this->_pLineText + linePos) = splitChar;
			linePos++;

			this->ftoa(pt.Y, this->_pWordText, 20, floatWidth);
			this->WriteToTempBuffer(this->_pLineText, this->_pWordText, floatWidth, linePos);
			*(this->_pLineText + linePos) = splitChar;
			linePos++;
			
			this->ftoa(pt.Z, this->_pWordText, 20, floatWidth);
			this->WriteToTempBuffer(this->_pLineText, this->_pWordText, floatWidth, linePos);
			*(this->_pLineText + linePos) = '\r';
			*(this->_pLineText + linePos + 1) = '\n';
			linePos += 2;

			lineSize = linePos;
		}*/

		void WriteBigLineBuffer(const std::vector<double> &vList, int &lineSize, char splitChar)
		{
			int dWidth = 0;
			int linePosIndex = 0;
			int vSize = (int)vList.size();

			for (int i = 0; i < vSize; i++)
			{
				this->dtoa(vList[i], this->_pWordText, 20, dWidth);
				this->WriteToTempBuffer(this->_pBigLineText, this->_pWordText, dWidth, linePosIndex);
				if(i < (vSize -1))
				{
					*(this->_pBigLineText + linePosIndex) = splitChar;
					linePosIndex++;
				}
				else
				{
					*(this->_pBigLineText + linePosIndex) = '\r';
					*(this->_pBigLineText + linePosIndex + 1) = '\n';
					linePosIndex += 2;
				}
			}
			lineSize = linePosIndex;
		}


		//将文件中的数据，读入缓冲区
		void ReadFileToBuffer(const std::string &fileName, int& length)
		{
			FILE *pFile = fopen(fileName.c_str(), "rb");
			length = (int)fread(this->_pTempBuffer, 1, BufferSize, pFile);
			fclose(pFile);
		}
		//将缓冲区中的数据，写入文件
		void WriteFileFromBuffer(const std::string &fileName, int length)
		{
			FILE *pFile = fopen(fileName.c_str(), "wb");
			fwrite(this->_pTempBuffer, 1, length, pFile);
			fclose(pFile);
		}
		void BigLineToValue(char* pBigLineText,
			int lineSize, vector<double>& values)
		{
			int vStart = 0;			
			int dwidth = 0;
			int dstart = vStart;
			for (int i = vStart; i < lineSize; i++)
			{
				char c1 = pBigLineText[i];
				//转换数据
				if (   c1 == ' '
					|| c1 == ','
					|| c1 == '\r'
					|| i == (lineSize - 1))
				{
					dwidth = i - dstart;
					if (dwidth < 1)
					{
						//起始数据位置
						dstart = i + 1;
						continue;
					}
					//如果ｍ表示是一行的结尾
					//那么需要dwidth宽度加１
					if (i == (lineSize - 1))
					{
						dwidth++;
					}
					this->ClearCharBuffer(_pWordText, 25, '\0');
					memcpy(_pWordText, pBigLineText + dstart, dwidth);
					//_pWordText[dwidth] = '\0';
					//获取数值
					values.push_back(atof(_pWordText));
					//起始数据位置
					dstart = i + 1;				
				}
			}
		}
		// 01
		// 将一行转换为数值
		// vFlag 表示是否有 v 字符开头
		void LineToValue(char* _pLineText,
			int lineSize, 			
			bool vFlag)
		{			
			if (vFlag && _pLineText[0] != 'v')
			{
				return;
			}
			int vStart = vFlag ? 2 : 0;
			int	findex = 0;
			int dwidth = 0;
			int dstart = vStart;
			
			for (int i = vStart; i < lineSize; i++)
			{
				//转换数据
				if (_pLineText[i] == ' ' 
					|| _pLineText[i] == ',' 
					|| _pLineText[i] == '\r'
					|| i == (lineSize - 1))
				{
					dwidth = i - dstart;
					if (dwidth < 1)
					{
						continue;
					}
					//如果ｍ表示是一行的结尾
					//那么需要dwidth宽度加１
					if (i == (lineSize - 1))
					{
						dwidth++;
					}
					this->ClearCharBuffer(_pWordText, 25, '\0');
					memcpy(_pWordText, _pLineText + dstart, dwidth);
					//_pWordText[dwidth] = '\0';
					dstart = i + 1;
					//获取数值
					if(findex < 10)
					{
						_tempValues[findex] = (float)atof(_pWordText);
					}
					findex++;
				}
			}			
		}
		//读取ply文件
		//void ReadPlyFile(const std::string& plyFile, bool isVertexList)
		//{
		//	int length = 0;
		//	this->ReadFileToBuffer(plyFile, length);
		//	this->_pTempBuffer[length] = '\0';

		//	int startIndex = 0;
		//	int curIndex = 0;
		//	bool dataFlag = false;
		//	bool sizeFlag = false;
		//	int ptsSize = 0;
		//	int ptsIndex = 0;
		//	while (curIndex < length)
		//	{
		//		char c1 = this->_pTempBuffer[curIndex];
		//		if (c1 == '\n')
		//		{
		//			int lineSize = curIndex - startIndex;
		//			memcpy(this->_pLineText, this->_pTempBuffer + startIndex, lineSize);
		//			this->_pLineText[lineSize] = '\0';
		//			//读取数据坐标
		//			if (lineSize > 1
		//				&& ptsIndex < ptsSize
		//				&& dataFlag)
		//			{
		//				this->LineToValue(this->_pLineText, lineSize, false);

		//				if (isVertexList)
		//				{
		//					pTempVList->AddVertex(this->_tempValues[0],
		//						this->_tempValues[1],
		//						this->_tempValues[2]);
		//				}
		//				else
		//				{
		//					pTempPoints->AddPoint(this->_tempValues[0],
		//						this->_tempValues[1],
		//						this->_tempValues[2],
		//						this->_tempValues[3],
		//						this->_tempValues[4],
		//						this->_tempValues[5]);
		//				}
		//				//pts.AddVertex(this->_tempValues[0], this->_tempValues[1], this->_tempValues[2]);
		//				ptsIndex++;
		//			}
		//			//element vertex 7548
		//			if (!sizeFlag
		//				&& this->_pLineText[0] == 'e'
		//				&& this->_pLineText[1] == 'l'
		//				&& this->_pLineText[2] == 'e'
		//				&& this->_pLineText[3] == 'm'
		//				&& this->_pLineText[7] == ' '
		//				&& this->_pLineText[8] == 'v'
		//				&& this->_pLineText[9] == 'e'
		//				&& this->_pLineText[10] == 'r'
		//				&& this->_pLineText[11] == 't')
		//			{
		//				sizeFlag = true;
		//				this->ClearCharBuffer(this->_pWordText, 25, '\0');
		//				memcpy(this->_pWordText, this->_pLineText + 14, lineSize - 1);
		//				ptsSize = atoi(this->_pWordText);
		//			}
		//			//判断数据行，是否开始
		//			if (!dataFlag
		//				&& this->_pLineText[0] == 'e'
		//				&& this->_pLineText[1] == 'n'
		//				&& this->_pLineText[2] == 'd'
		//				&& this->_pLineText[3] == '_')
		//			{
		//				dataFlag = true;
		//			}
		//			//换到下一行，起始位置
		//			startIndex = curIndex + 1;
		//		}//c1  \n 换行结束 end if 
		//		curIndex++;
		//	}
		//}		
	public:
		//清空字符串
		void ClearCharBuffer(char* pChar, int size, char word)
		{
			for (int n = 0; n < size; n++)
			{
				pChar[n] = word;
			}
		}
		//float转换字符串
		int ftoa(float value, char* buffer, int bufferSize, int & length)
		{
			this->ClearCharBuffer(buffer, bufferSize, '\0');
			sprintf(buffer, "%.4lf", value);
			size_t size = strlen(buffer);
			//buffer[size] = ' ';
			buffer[size] = '\0';
			length = (int)size;
			return length;
		}
		//double转换字符串
		int dtoa(double value, char* buffer, int bufferSize, int & length)
		{
			this->ClearCharBuffer(buffer, bufferSize, '\0');
			sprintf(buffer, "%.8lf", value);
			size_t size = strlen(buffer);
			//buffer[size] = ' ';
			buffer[size] = '\0';
			length = (int)size;
			return length;
		}

	};	

	//******************************************************
	// 01
	VStream::VStream(void)
		:the(NULL)
	{
		the = new VStreamThe();
		
	}
	// 02
	VStream::~VStream(void)
	{
		if (the == NULL)
		{
			delete the;
			the = NULL;
		}
	}
	// 03
	void  VStream::ReadTextFile(const std::string &path, std::vector<std::string>&  context)
	{
		ifstream readtext;
		readtext.open(path);
		context.clear();
		string line;

		while (getline(readtext, line))
		{
			context.push_back(line);
		}
		readtext.close();
	}	
	// 04
	void VStream::WriteTextFile(const std::string &path, const std::vector<std::string> &context, bool isNew)
	{
		int vectorsize = (int)context.size();
		ofstream ofs;
		if (isNew)
		{
			ofs.open(path, ios::out | ios::trunc);
		}
		else
		{
			ofs.open(path, ios::app);
		}
		for (int i = 0; i < vectorsize; i++)
		{
			bool openFlag = ofs.is_open();
			if (openFlag)
			{
				ofs << context[i] << endl;
			}
		}
		ofs.close();
	}
	// 05
	void VStream::SplitString(const std::string &context,
		std::vector<std::string>& texts,
		const char separator) const
	{
		texts.clear();
		size_t pos = 0;
		size_t len = context.length();
		while (pos < len)
		{
			size_t width = 0;
			size_t start_pos = pos;
			int find_pos = (int)context.find(separator, pos);

			if (find_pos < (int)0 || find_pos > len)
			{
				width = len - pos;
				pos = len;
			}
			else
			{
				width = find_pos - pos;
				pos = find_pos + 1;
			}
			if (width <= 0)
			{
				continue;
			}
			std::string text = context.substr(start_pos, width);
			texts.push_back(text);
		}
	}
	// 06
	std::string VStream::MergeString(const std::vector<std::string> &inputvector,
		const char separator)const
	{
		int vectorsize = (int)inputvector.size();
		std::string mergestring;
		for (int i = 0; i < vectorsize - 1; i++)
		{
			std::string constring = inputvector[i] + separator;
			mergestring += constring;
		}
		mergestring += inputvector[vectorsize - 1];
		return mergestring;
	}
	// 11
	//void VStream::ReadObjFile(const std::string& objFile, Lm3dSpace::VertexList<float>& pts)
	//{
	//	int length = 0;
	//	the->ReadFileToBuffer(objFile, length);
	//	the->_pTempBuffer[length] = '\0';

	//	int startIndex = 0;
	//	int curIndex = 0;
	//	bool dataFlag = true;
	//	
	//	while (curIndex < length)
	//	{
	//		char c1 = the->_pTempBuffer[curIndex];
	//		if (c1 == '\n')
	//		{
	//			int lineSize = curIndex - startIndex;
	//			memcpy(the->_pLineText, the->_pTempBuffer + startIndex, lineSize);
	//			the->_pLineText[lineSize] = '\0';

	//			if (the->_pLineText[0] == 'v'
	//				&& lineSize > 1 					
	//				&& dataFlag)
	//			{
	//				the->LineToValue(the->_pLineText, lineSize, true);
	//				pts.AddVertex(the->_tempValues[0], the->_tempValues[1], the->_tempValues[2]);
	//			} // line end if
	//			  //换到下一行，起始位置
	//			startIndex = curIndex + 1;
	//		}//c1  \n 换行结束 end if 
	//		curIndex++;
	//	}	
	//}
	//// 12
	//void VStream::ReadPlyFile(const std::string& plyFile, Lm3dSpace::VertexList<float>& pts)
	//{
	//	the->pTempVList = &pts;
	//	the->ReadPlyFile(plyFile,true);		
	//}
	//// 13
	//void VStream::ReadPlyFile(const std::string& plyFile, Lm3dSpace::Points<float>& pts)
	//{
	//	the->pTempPoints = &pts;
	//	the->ReadPlyFile(plyFile, false);		
	//}
	//// 14
	//void VStream::WriteObjFile(const std::string& objFile,const Lm3dSpace::VertexList<float>& pts)
	//{
	//	int pos = 0;
	//	int size = pts.Size();
	//	std::string head1 = "# Candide-3.1.6\r\n";
	//	std::string head2 = "# VERTEX LIST:\r\n";		
	//	std::string head3 = std::string("#") + ToString<int>(size) + "\r\n";

	//	the->WriteToTempBuffer(head1, pos);
	//	the->WriteToTempBuffer(head2, pos);
	//	the->WriteToTempBuffer(head3, pos);
	//	for (int i = 0; i < size; i++)
	//	{
	//		the->_pTempBuffer[pos] = 'v';
	//		the->_pTempBuffer[pos + 1] = ' ';
	//		pos += 2;
	//		Vertex3D<float> * pt = pts[i];
	//		std::string text1 = ToString<float>(pt->X) + " ";
	//		std::string text2 = ToString<float>(pt->Y) + " ";
	//		std::string text3 = ToString<float>(pt->Z) + "\r\n";
	//		the->WriteToTempBuffer(text1, pos);
	//		the->WriteToTempBuffer(text2, pos);
	//		the->WriteToTempBuffer(text3, pos);			
	//	}
	//	the->WriteFileFromBuffer(objFile, pos);
	//}
	// 15
	//void VStream::WritePlyFile(const std::string& plyFile, const Lm3dSpace::VertexList<float>& pts)
	//{
	//	int posIndex = 0;
	//	int ptSize = pts.Size();
	//	std::string head1 = "ply\r\n";
	//	std::string head2 = "format ascii 1.0\r\n";
	//	std::string head3 = std::string("element vertex ") + ToString<int>(ptSize) + "\r\n";
	//	std::string head21 = "property float x\r\n";
	//	std::string head22 = "property float y\r\n";
	//	std::string head23 = "property float z\r\n";
	//	std::string head41 = "end_header\r\n";
	//	the->WriteToTempBuffer(head1, posIndex);
	//	the->WriteToTempBuffer(head2, posIndex);
	//	the->WriteToTempBuffer(head3, posIndex);
	//	the->WriteToTempBuffer(head21, posIndex);
	//	the->WriteToTempBuffer(head22, posIndex);
	//	the->WriteToTempBuffer(head23, posIndex);
	//	the->WriteToTempBuffer(head41, posIndex);	

	//	for (int i = 0; i < ptSize; i++)
	//	{
	//		Vertex3D<float> * pPoint = pts[i];
	//		int lineSize = 0;
	//		the->WriteToLineBuffer(*pPoint, lineSize, ' ');
	//		the->WriteToTempBuffer(the->_pTempBuffer, the->_pLineText, lineSize, posIndex);
	//	}
	//	the->WriteFileFromBuffer(plyFile, posIndex);
	//}
	// 16
	/*void VStream::WritePlyFile(const std::string& plyFile, const Lm3dSpace::Points<float>& pts)
	{
		int pos = 0;
		int size = pts.Size();
		std::string head01 = "ply\r\n";
		std::string head02 = "format ascii 1.0\r\n";
		std::string head03 = std::string("element vertex ") + ToString<int>(size) + "\r\n";
		std::string head21 = "property float x\r\n";
		std::string head22 = "property float y\r\n";
		std::string head23 = "property float z\r\n";
		std::string head31 = "property uchar red\r\n";
		std::string head32 = "property uchar green\r\n";
		std::string head33 = "property uchar blue\r\n";
		std::string head34 = "property uchar alpha\r\n";
		std::string head41 = "end_header\r\n";
		the->WriteToTempBuffer(head01,  pos);
		the->WriteToTempBuffer(head02,  pos);
		the->WriteToTempBuffer(head03,  pos);
		the->WriteToTempBuffer(head21,  pos);
		the->WriteToTempBuffer(head22,  pos);
		the->WriteToTempBuffer(head23,  pos);
		the->WriteToTempBuffer(head31,  pos);
		the->WriteToTempBuffer(head32,  pos);
		the->WriteToTempBuffer(head33,  pos);
		the->WriteToTempBuffer(head34,  pos);
		the->WriteToTempBuffer(head41,  pos);
		for (int i = 0; i < size; i++)
		{
			Lm3dSpace::Point<float> * pt = pts[i];
			std::string text1 = ToString<float>(pt->X) + " ";
			std::string text2 = ToString<float>(pt->Y) + " ";
			std::string text3 = ToString<float>(pt->Z) + " ";
			std::string text4 = ToString<float>(pt->Red) + " ";
			std::string text5 = ToString<float>(pt->Green) + " ";
			std::string text6 = ToString<float>(pt->Blue) + " 255\r\n";
			the->WriteToTempBuffer(text1,  pos);
			the->WriteToTempBuffer(text2,  pos);
			the->WriteToTempBuffer(text3,  pos);
			the->WriteToTempBuffer(text4,  pos);
			the->WriteToTempBuffer(text5,  pos);
			the->WriteToTempBuffer(text6,  pos);
		}
		the->WriteFileFromBuffer(plyFile, pos);
	}*/
	// 17
	//void VStream::WriteBmpImage(const std::string& imageFile,
	//	VColor *imageData, 
	//	int width, 
	//	int height)
	//{
	//	BITMAPFILEHEADER strHead;
	//	RGBQUAD strPla[256];//256色调色板  
	//	BITMAPINFOHEADER strInfo;

	//	strHead.bfType = 0x4d42;
	//	strHead.bfSize = width * height + 54;//文件大小
	//	strHead.bfReserved1 = 0;
	//	strHead.bfReserved2 = 0;
	//	strHead.bfOffBits = 54;//像素数据的偏移

	//	strInfo.biSize = 40;		//  4 此结构体的大小  
	//	strInfo.biWidth = width;	//  4  图像的宽 
	//	strInfo.biHeight = height;	//  4 图像的高  
	//	strInfo.biPlanes = 1;		//  2 图像的帧数，一般为1  
	//	strInfo.biBitCount = 24;		//  2 一像素所占的位数，一般是24 
	//	strInfo.biCompression = 0;	//  4 一般为0  
	//	strInfo.biSizeImage = width*height;//  4 像素数据所占大小，即上面结构体中文件大小减去偏移(bfSize-bfOffBits)  
	//	strInfo.biXPelsPerMeter = 0;	//  4  一般为0 
	//	strInfo.biXPelsPerMeter = 0;	//  4   一般为0 
	//	strInfo.biClrUsed = 0;		//  4   一般为0 
	//	strInfo.biClrImportant = 0;	//  4   一般为0 

	//	FILE *fpw;
	//	//保存bmp图片  
	//	if ((fpw = fopen(imageFile.c_str(), "wb")) == NULL)
	//	{
	//		//cout<<"create the bmp file error!"<<endl;  
	//		return;
	//	}
	//	//WORD bfType_w=0x4d42;  //文件头
	//	//fwrite(&bfType_w,1,sizeof(WORD),fpw);  
	//	//fpw +=2;  
	//	fwrite(&strHead, 1, sizeof(tagBITMAPFILEHEADER), fpw);
	//	//strInfo.biWidth = 2 * width;  
	//	//strInfo.biHeight = 2 * height;  
	//	fwrite(&strInfo, 1, sizeof(tagBITMAPINFOHEADER), fpw);
	//	//保存调色板数据  
	//	for (unsigned int nCounti = 0; nCounti<strInfo.biClrUsed; nCounti++)
	//	{
	//		fwrite(&strPla[nCounti].rgbBlue, 1, sizeof(BYTE), fpw);
	//		fwrite(&strPla[nCounti].rgbGreen, 1, sizeof(BYTE), fpw);
	//		fwrite(&strPla[nCounti].rgbRed, 1, sizeof(BYTE), fpw);
	//		fwrite(&strPla[nCounti].rgbReserved, 1, sizeof(BYTE), fpw);
	//	}
	//	//保存像素数据  
	//	for (int i = height - 1; i >= 0; --i)
	//	{
	//		for (int j = 0; j < width; ++j)
	//		{
	//			fwrite(&((*(imageData + i * width + j)).r), 1, sizeof(BYTE), fpw);
	//			fwrite(&((*(imageData + i * width + j)).g), 1, sizeof(BYTE), fpw);
	//			fwrite(&((*(imageData + i * width + j)).b), 1, sizeof(BYTE), fpw);
	//		}
	//	}
	//	fclose(fpw);
	//}
	// 18
	//void VStream::WritePtsImage(const std::string& imageFile,
	//	const Lm3dSpace::Points<float>& points,
	//	int &imageWidth,
	//	int &imageHeight,
	//	std::vector<int> &pointIndexes)
	//{
	//	Lm3dSpace::VBox3D<float> box = points.GetBox();
	//	float minX = box.minVt.X;
	//	float maxX = box.maxVt.X;
	//	float minY = box.minVt.Y;
	//	float maxY = box.maxVt.Y;
	//	float step = 1.7f;
	//	float width = (maxX - minX) / step;
	//	float height = (maxY - minY) / step;
	//	//bmp图像的行宽必须是4的倍数
	//	imageWidth = int( ceil(width/4.0) * 4.0);
	//	imageHeight = int(height);
	//	VColor * pImage = new VColor[size_t(imageWidth * height)];
	//	pointIndexes.clear();
	//	for (int i = 0; i < imageWidth * imageHeight; i++)
	//	{
	//		pointIndexes.push_back(-1);
	//	}

	//	for (int i = 0; i < points.Size(); i++)
	//	{
	//		Lm3dSpace::Point<float>* pt = points[i];
	//		//根据点坐标,求解图像位置
	//		int xIndex = int((pt->X - minX) / step);
	//		int yIndex = int((maxY - pt->Y) / step);
	//		if (yIndex == imageHeight)
	//		{
	//			yIndex--;
	//		}
	//		if (xIndex == imageWidth)
	//		{
	//			xIndex--;
	//		}
	//		int imageIndex = yIndex * imageWidth + xIndex;
	//		//记录图像对应的三维点索引
	//		pointIndexes[imageIndex] = i;
	//		VColor * curImage = pImage + imageIndex;
	//		//区别bgr图像,rgb图像
	//		curImage->r =  (unsigned char)pt->Blue;
	//		curImage->g = (unsigned char)pt->Green;
	//		curImage->b = (unsigned char)pt->Red;
	//	}

	//	this->WriteBmpImage(imageFile, pImage, imageWidth,(int)height);
	//}
	// 21
	void VStream::ReadDoubleVecList(const std::string &path,
		std::vector< std::vector<double>>&  vecList)
	{
		int length = 0;
		the->ReadFileToBuffer(path, length);
		the->_pTempBuffer[length] = '\0';

		int startIndex = 0;
		int curIndex = 0;
		while (curIndex < length)
		{
			char c1 = the->_pTempBuffer[curIndex];
			if (c1 == '\n' )
			{
				int lineSize = curIndex - startIndex;
				memcpy(the->_pBigLineText, the->_pTempBuffer + startIndex, lineSize);
				the->_pBigLineText[lineSize] = '\0';
				vecList.push_back(std::vector<double>());
				std::vector<double>& vec = vecList[vecList.size() - 1];
				the->BigLineToValue(the->_pBigLineText, lineSize, vec);
				//换到下一行，起始位置
				startIndex = curIndex + 1;
			}//c1  \n 换行结束 end if 
			curIndex++;
		}
	}
	// 22
	void VStream::ReadDoubleVector(const std::string &path, std::vector<double>&  vec)
	{
		int length = 0;
		the->ReadFileToBuffer(path, length);
		the->_pTempBuffer[length] = '\0';

		int startIndex = 0;
		int curIndex = 0;
		while (curIndex < length)
		{
			char c1 = the->_pTempBuffer[curIndex];

			if (c1 == '\n')
			{
				int lineSize = curIndex - startIndex;
				memcpy(the->_pBigLineText, the->_pTempBuffer + startIndex, lineSize);
				the->_pBigLineText[lineSize] = '\0';
				the->BigLineToValue(the->_pBigLineText, lineSize, vec);
				//换到下一行，起始位置
				startIndex = curIndex + 1;
			}//c1  \n 换行结束 end if 
			curIndex++;
		}
	}
	// 23
	void  VStream::WriteDoubleVector(const std::string &path, const std::vector<double>&  dVector)
	{
		int posIndex = 0;
		int listSize = int(dVector.size());		
		for (int i = 0; i < listSize; i++)
		{		
			int dWidth = 0;			
			the->dtoa(dVector[i], the->_pWordText, 20, dWidth);
			*(the->_pWordText + dWidth) = '\r';
			*(the->_pWordText + dWidth + 1) = '\n';
			dWidth += 2;
			the->WriteToTempBuffer(the->_pTempBuffer, the->_pWordText, dWidth, posIndex);		
		}
		the->WriteFileFromBuffer(path, posIndex);
	}
	// 24
	void VStream::ReadIntVector(const std::string &path, std::vector<int>&  vec)
	{
		int length = 0;
		the->ReadFileToBuffer(path, length);
		the->_pTempBuffer[length] = '\0';

		int startIndex = 0;
		int curIndex = 0;
		while (curIndex < length)
		{
			char c1 = the->_pTempBuffer[curIndex];
			if (c1 == ' ' || c1 == ','|| c1== '\n')
			{
				int lineSize = curIndex - startIndex;
				memcpy(the->_pWordText, the->_pTempBuffer + startIndex, lineSize);
				the->_pWordText[lineSize] = '\0';
				int val = atoi(the->_pWordText);
				vec.push_back(val);
				//换下一，起始位置
				startIndex = curIndex + 1;
			}//c1  \n 换行结束 end if 
			curIndex++;
		}
	}
	// 25
	void VStream::WriteIntVector(const std::string &path, const std::vector<int>&  iVecList)
	{
		int posIndex = 0;
		int listSize = int(iVecList.size());
		int Radix = 10;//十进制
		for (int i = 0; i < listSize; i++)
		{			
			_itoa(iVecList[i], the->_pWordText, Radix);
			int dWidth = (int)strlen(the->_pWordText);			
			*(the->_pWordText + dWidth) = '\r';
			*(the->_pWordText + dWidth + 1) = '\n';
			dWidth += 2;
			the->WriteToTempBuffer(the->_pTempBuffer, the->_pWordText, dWidth, posIndex);
		}
		the->WriteFileFromBuffer(path, posIndex);
	}
	// 26
	void VStream::WriteDoubleVecList(const std::string &path, const std::vector<std::vector<double>>& vecList)
	{	
		int posIndex = 0;
		int listSize = int(vecList.size());
		//std::string nextLine("\r\n");
		for (int i = 0; i < listSize; i++)
		{
			int lineSize = 0;

			the->WriteBigLineBuffer(vecList[i], lineSize, ' ');

			the->WriteToTempBuffer(the->_pTempBuffer,the->_pBigLineText,lineSize, posIndex);
		}
		the->WriteFileFromBuffer(path, posIndex);		
	}
