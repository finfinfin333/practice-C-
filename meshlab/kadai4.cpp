//各座標の平均座標と分散を計算するプログラム

// PCL_Test.cpp : アプリケーションのエントリ ポイントを定義します。
//

#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#include <string>
#pragma warning(disable:4996)

// std
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

// vcg コア
#include <vcg/complex/complex.h>

// vcg input output
#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_stl.h>

//平均と分散に使う
#include <math.h>

using namespace vcg;
using namespace std;

/////////////////////////////////////////////////
/////////////////////////////////////////////////
// メッシュデータ型
/////////////////////////////////////////////////
/////////////////////////////////////////////////
class MyFace;
class MyVertex;

struct MyUsedTypes : public
	UsedTypes<
	Use<MyVertex>::AsVertexType,
	Use<MyFace>::AsFaceType>
{};

class MyVertex :
	public Vertex<
	MyUsedTypes,
	vertex::Coord3f,
	vertex::Color4b,
	vertex::Normal3f,
	vertex::BitFlags,
	vertex::Mark>
{};

class MyFace :
	public Face  <
	MyUsedTypes,
	face::VertexRef,
	face::Normal3f,
	face::BitFlags >
{};

class MyMesh :
	public vcg::tri::TriMesh<
	vector<MyVertex>,
	vector<MyFace> >
{};


/////////////////////////////////////////////////
/////////////////////////////////////////////////
// 処理本体
/////////////////////////////////////////////////
/////////////////////////////////////////////////

enum MESH_TYPE{
	MT_ERR,MT_STL,MT_PLY,
};

int main(int argc, char* argv[])
{
	//設定ファイル読み込み
	std::ifstream fs("settings.txt");
	std::string targetfname;
	getline(fs,targetfname);

	MESH_TYPE meshtype = MT_ERR;

	//拡張子チェック
	char ext[4]={};
	strncpy(ext,targetfname.c_str()+targetfname.size()-3,3);

	//拡張子を小文字に
	for(int i = 0;i<3;i++)
		ext[i]=tolower(ext[i]);

	//ファイルの種類特定
	if( strncmp(ext,"stl",3) == 0){
		meshtype = MT_STL;
	}
	else if( strncmp(ext,"ply",3)==0){
		meshtype = MT_PLY;
	}

	int ret;
	//ファイル読み込み
	MyMesh m;

	int mask;
	switch(meshtype){
	case MT_STL:
		ret =vcg::tri::io::ImporterSTL<MyMesh>::Open(m, targetfname.c_str(),mask); 
		break;
	case MT_PLY:
		ret = vcg::tri::io::ImporterPLY<MyMesh>::Open(m, targetfname.c_str());
		break;
	default:
		std::cout << "file type error. needed ply or stl" << std::endl;
		ret = -1;
	}

	//ファイルが読めなかった場合
	if ( ret != 0)
	{
		std::cout << targetfname.c_str() << std::endl;
		exit(0);
	}

	//重複頂点の削除(特にSTLで必要)
	vcg::tri::Clean<MyMesh>::RemoveDuplicateVertex(m);
	vcg::tri::Allocator<MyMesh>::CompactFaceVector(m);

	//メッシュを削除
	for(size_t i = 0;i< m.face.size();i++){
		vcg::tri::Allocator<MyMesh>::DeleteFace(m,m.face[i]);
	}
	vcg::tri::Allocator<MyMesh>::CompactFaceVector(m);


	//データの平均を求める
	float sumx = 0.0f, sumy = 0.0f, sumz = 0.0f;
	float avgx, avgy, avgz;
	for (size_t i = 0; i < m.vert.size(); i++){ //sizeは頂点の数 for分は頂点の数だけ回る
		sumx += m.vert[i].P().X(); //右辺は表示されない
		sumy += m.vert[i].P().Y();
		sumz += m.vert[i].P().Z();
	}
	avgx = sumx/m.vert.size();
	avgy = sumy/m.vert.size();
	avgz = sumz/m.vert.size();
	//重心表示
	printf("average = %f %f %f\n", avgx, avgy, avgz);

	//データの分散を調べる
	float ssx, ssy, ssz;
	float a = 0.0f, b = 0.0f, c = 0.0f;
	for (size_t i = 0; i < m.vert.size(); i++){
		a += powf(m.vert[i].P().X() - avgx, 2);
		b += powf(m.vert[i].P().Y() - avgx, 2);
		c += powf(m.vert[i].P().Z() - avgx, 2);
	}
	ssx = a / m.vert.size();
	ssy = b / m.vert.size();
	ssz = c / m.vert.size();

	printf("dispersion = %f %f %f\n", ssx, ssy, ssz);

	//ファイル出力
	targetfname += ".out.ply";
	vcg::tri::io::PlyInfo pi;
	pi.mask = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTNORMAL;
	vcg::tri::io::ExporterPLY<MyMesh>::Save(m, targetfname.c_str(), pi.mask);


	std::cout << "\nfinished";
	int i;
	std::cin >> i;

	return (0);
}