#include "stdafx.h"
#include "Extraction.h"
#include <pcl/io/pcd_io.h>
#include "VStream.h"

Extraction::Extraction()
{

}


Extraction::~Extraction()
{
}

//VStream vs;
struct facePtindex
{
	int p1;
	int p2;
	int p3;
};
struct mypt
{
	pcl::PointXYZRGBA point;
	int index;
	bool changed;
	std::vector<std::string> faceStrVec;
	std::vector<facePtindex> faceIndexVec;
};
double CrossProduction(const pcl::PointXYZRGBA &pointa, const pcl::PointXYZRGBA &pointb)
{
	return pointa.x* pointb.y - pointa.y*pointb.x;
}
void Get2dBox(const pcl::PointXYZ &pointa, const pcl::PointXYZ &pointb, const pcl::PointXYZ &pointc,
	pcl::PointXYZ &minp, pcl::PointXYZ &maxp)
{
	double maxX = pointa.x > pointb.x ? pointa.x : pointb.x;
	maxX = maxX > pointc.x ? maxX : pointc.x;
	double maxY = pointa.y > pointb.y ? pointa.y : pointb.y;
	maxY = maxY > pointb.y ? maxY : pointb.y;

	double minX = pointa.x < pointb.x ? pointa.x : pointb.x;
	minX = maxX < pointc.x ? maxX : pointc.x;
	double minY = pointa.y < pointb.y ? pointa.y : pointb.y;
	minY = minY < pointb.y ? maxY : pointb.y;

	minp.x = minX;
	minp.y = minY;
	maxp.x = maxX;
	maxp.y = maxY;
}
void Get2dBox(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &bbox_pt,
	pcl::PointXYZRGBA &minp, pcl::PointXYZRGBA &maxp)
{
	double maxX = bbox_pt->at(0).x;
	double maxY = bbox_pt->at(0).y;
	double minX = bbox_pt->at(0).x;
	double minY = bbox_pt->at(0).y;
	int size = bbox_pt->size();
	for (int i = 1; i < size; ++i)
	{
		minX = minX < bbox_pt->at(i).x ? minX : bbox_pt->at(i).x;
		minY = minY < bbox_pt->at(i).y ? minY : bbox_pt->at(i).y;
		maxX = maxX > bbox_pt->at(i).x ? maxX : bbox_pt->at(i).x;
		maxY = maxY > bbox_pt->at(i).y ? maxY : bbox_pt->at(i).y;

	}
	minp.x = minX;
	minp.y = minY;
	maxp.x = maxX;
	maxp.y = maxY;
}
bool IsInbox(const pcl::PointXYZ &minp, const pcl::PointXYZ &maxp, const pcl::PointXYZ &p)
{
	if (p.x >= minp.x && p.x <= maxp.x)
	{
		if (p.y >= minp.y && p.y <= maxp.y)
		{
			return true;
		}
	}
	return false;
}
inline bool IsInbox(const pcl::PointXYZRGBA &minp, const pcl::PointXYZRGBA &maxp, const pcl::PointXYZRGBA &p)
{
	if (p.x >= minp.x && p.x <= maxp.x)
	{
		if (p.y >= minp.y && p.y <= maxp.y)
		{
			return true;
		}
	}
	return false;
}
bool IsInTriangle(const pcl::PointXYZRGBA &pointa, const pcl::PointXYZRGBA &pointb, const pcl::PointXYZRGBA &pointc,
	const pcl::PointXYZRGBA &p)
{
	pcl::PointXYZRGBA ab;
	ab.x = pointa.x - pointb.x;
	ab.y = pointa.y - pointb.y;

	pcl::PointXYZRGBA ac;
	ac.x = pointa.x - pointc.x;
	ac.y = pointa.y - pointc.y;

	pcl::PointXYZRGBA ap;
	ap.x = pointa.x - p.x;
	ap.y = pointa.y - p.y;

	pcl::PointXYZRGBA pb;
	pb.x = p.x - pointb.x;
	pb.y = p.y - pointb.y;

	pcl::PointXYZRGBA pc;
	pc.x = p.x - pointc.x;
	pc.y = p.y - pointc.y;

	//Vertex2D<float> ab(pointb - pointa), ac(pointc - pointa), ap(p - pointa);
	//用矢量积计算面积，下面4个值的绝对值，是对应的三角形的面积的两倍，
	double abc = CrossProduction(ab, ac);//ab.cross(ac);
	double abp = CrossProduction(ab, ap);//ab.cross(ap);
	double apc = CrossProduction(ap, ac); //ap.cross(ac);
	double pbc = CrossProduction(pb, pc);
	//等于pb.cross(pc)			
	//面积法：4个三角形的面积差 等于 0
	double delta = fabs(abc) - fabs(abp) - fabs(apc) - fabs(pbc);
	double dbl_EPSILON = 0.00000001;
	return fabs(delta) < dbl_EPSILON;
}
/* ply 头文件解析类型 例子
ply
format ascii 1.0
comment VCGLIB generated
comment TextureFile Tile_+001_+002_0.jpg
comment TextureFile Tile_+001_+002_1.jpg
element vertex 372675
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
property uchar alpha
element face 740680
property list uchar int vertex_indices
property list uchar float texcoord
property int texnumber
property uchar red
property uchar green
property uchar blue
property uchar alpha
end_header
*/
void writePly(const std::vector<std::string> &pointstrvec, const std::vector<std::string> &facestrvec,
	const std::string &outpath)
{
	VStream vs;
	std::vector<std::string> fileStrVec;
	std::string head1 = "ply";
	std::string head2 = "format ascii 1.0";
	std::string head3 = "comment VCGLIB generated";
	std::string head4 = "comment TextureFile Tile_ + 001_ + 002_0.jpg";
	std::string head5 = "comment TextureFile Tile_ + 001_ + 002_1.jpg";
	std::string head6 = "element vertex " + std::to_string(pointstrvec.size());
	std::string head7 = "property float x";
	std::string head8 = "property float y";
	std::string head9 = "property float z";
	std::string head10 = "property uchar red";
	std::string head11 = "property uchar green";
	std::string head12 = "property uchar blue";
	std::string head13 = "property uchar alpha";
	std::string head14 = "element face " + std::to_string(facestrvec.size());
	std::string head15 = "property list uchar int vertex_indices";
	std::string head16 = "property list uchar float texcoord";
	std::string head17 = "property int texnumber";
	std::string head18 = "property uchar red";
	std::string head19 = "property uchar green";
	std::string head20 = "property uchar blue";
	std::string head21 = "property uchar alpha";
	std::string head22 = "end_header";

	fileStrVec.push_back(head1);
	fileStrVec.push_back(head2);
	fileStrVec.push_back(head3);
	fileStrVec.push_back(head4);
	fileStrVec.push_back(head5);
	fileStrVec.push_back(head6);
	fileStrVec.push_back(head7);
	fileStrVec.push_back(head8);
	fileStrVec.push_back(head9);
	fileStrVec.push_back(head10);
	fileStrVec.push_back(head11);
	fileStrVec.push_back(head12);
	fileStrVec.push_back(head13);
	fileStrVec.push_back(head14);
	fileStrVec.push_back(head15);
	fileStrVec.push_back(head16);
	fileStrVec.push_back(head17);
	fileStrVec.push_back(head18);
	fileStrVec.push_back(head19);
	fileStrVec.push_back(head20);
	fileStrVec.push_back(head21);
	fileStrVec.push_back(head22);
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

struct triangle
{
	pcl::PointXYZRGBA p1;
	pcl::PointXYZRGBA p2;
	pcl::PointXYZRGBA p3;
};

bool IsInBbox(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &bbox_pt, const mypt *mynewpt)
{
	pcl::PointXYZRGBA minp;
	pcl::PointXYZRGBA maxp;
	Get2dBox(bbox_pt, minp, maxp);
	if (!IsInbox(minp, maxp, mynewpt->point))
	{
		return false;
	}
	int size = bbox_pt->size();
	std::vector<triangle> triangleVec;
	for (int i = 1; i < size - 1; i++)
	{
		if (IsInTriangle(bbox_pt->at(0), bbox_pt->at(i), bbox_pt->at(i + 1), mynewpt->point))
		{
			return true;
		}
	}
	return false;
}
bool pointInPolygon(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &bbox_pt, const mypt *mynewpt) 
{
	int polySides = bbox_pt->size();
	int  i, j = polySides - 1;
	bool  oddNodes = false;
	float y = mynewpt->point.y;
	float x = mynewpt->point.x;
	for (i = 0; i<polySides; i++) {
		if ((bbox_pt->at(i).y< y && bbox_pt->at(j).y >= y
			|| bbox_pt->at(j).y<y && bbox_pt->at(i).y >= y)
			&& (bbox_pt->at(i).x <= x || bbox_pt->at(j).x <= x)) {
			oddNodes ^= (bbox_pt->at(i).x + (y - bbox_pt->at(i).y) / (bbox_pt->at(j).y - bbox_pt->at(i).y)*(bbox_pt->at(j).x- bbox_pt->at(i).x)<x);
		}
		j = i;
	}
	return oddNodes;
}
void Extraction::ExtractBuidlingClouds(const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &input_cloud_pt,
	const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &bbox_pt,
	const std::vector<std::string> &inputface, std::vector<std::string> &newpointCloudStr,
	std::vector<std::string> &faceOutStrVec)
{
	VStream vs;
	int pt_size = input_cloud_pt->size();
	int face_size = inputface.size();
	std::vector<mypt> mypt_cloud;
	for (int i = 0; i < pt_size; i++)
	{
		mypt mynewpt;
		mynewpt.point = input_cloud_pt->at(i);
		mynewpt.index = i;
		mynewpt.changed = false;
		mypt_cloud.push_back(mynewpt);
	}
	for (int i = 0; i < face_size; ++i)
	{
		std::string newfacestr = inputface[i];
		std::vector<std::string> facetext;
		vs.SplitString(newfacestr, facetext, ' ');
		facePtindex mynewface;
		mynewface.p1 = atoi(facetext[1].c_str());
		mynewface.p2 = atoi(facetext[2].c_str());
		mynewface.p3 = atoi(facetext[3].c_str());
		mypt_cloud[mynewface.p1].faceStrVec.push_back(newfacestr);
		mypt_cloud[mynewface.p1].faceIndexVec.push_back(mynewface);
	}

	//get the point clouds that are inside the box, and change their index
	std::vector<int> ptindex;
	int k = 0;
	for (int i = 0; i < pt_size; i++)
	{
		//if (IsInBbox(bbox_pt, &mypt_cloud[i]))
		if(pointInPolygon(bbox_pt, &mypt_cloud[i]))
		{
			ptindex.push_back(i);
			mypt_cloud[i].index = k;
			mypt_cloud[i].changed = true;
			k++;
		}
	}
	// change face index
	// store the face index as string 
	
	int newsize = ptindex.size();
	for (int i = 0; i < newsize; i++)
	{
		int ptface_size = mypt_cloud[ptindex[i]].faceIndexVec.size();
		for (int j = 0; j < ptface_size; j++)
		{
			int p1 = mypt_cloud[ptindex[i]].faceIndexVec[j].p1;
			//mypt_cloud[i].faceIndexVec[j].p1 = mypt_cloud[p1].index;
			int p2 = mypt_cloud[ptindex[i]].faceIndexVec[j].p2;
			//mypt_cloud[i].faceIndexVec[j].p2 = mypt_cloud[p2].index;
			int p3 = mypt_cloud[ptindex[i]].faceIndexVec[j].p3;
			//mypt_cloud[i].faceIndexVec[j].p3 = mypt_cloud[p3].index;
			if (!mypt_cloud[p2].changed || !mypt_cloud[p3].changed)
			{
				continue;
			}
			std::string newfacestrOut = mypt_cloud[ptindex[i]].faceStrVec[j];
			std::vector<std::string> facetextOut;
			vs.SplitString(newfacestrOut, facetextOut, ' ');

			facetextOut[1] = std::to_string(mypt_cloud[p1].index);
			facetextOut[2] = std::to_string(mypt_cloud[p2].index);
			facetextOut[3] = std::to_string(mypt_cloud[p3].index);

			std::string newfaceOutStr = vs.MergeString(facetextOut, ' '); 
			faceOutStrVec.push_back(newfaceOutStr);
		}	
	}
	// store filtered points as a string vector
	
	for (int j = 0; j < newsize; j++)
	{
		std::vector<std::string> pointtextOut;
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.x));
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.y));
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.z));
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.r));
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.g));
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.b));
		pointtextOut.push_back(std::to_string(mypt_cloud[ptindex[j]].point.a));
		std::string newpointtextOut = vs.MergeString(pointtextOut, ' ');
		newpointCloudStr.push_back(newpointtextOut);
	}
}
//void Extraction::ExtractBuidlingClouds(std::string boundingfilePath, std::string outfilePath)    
//{
//	pcl::PointCloud<pcl::PointXYZRGBA>::Ptr input_cloud_pt (new pcl::PointCloud<pcl::PointXYZRGBA>());
//	pcl::TextureMesh::Ptr tm(new pcl::TextureMesh());
//	pcl::PolygonMesh::Ptr pt(new pcl::PolygonMesh());
//	pcl::OBJReader objreader;
//	pcl::io::loadPLYFile(boundingfilePath,*pt);
//	pcl::io::savePLYFile(outfilePath, *pt);
//	//pcl::io::savePLYFile();
//}

