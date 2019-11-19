//kdツリーを作成し、ユーザが座標を入力して、その座標に一番近い頂点を検索するプログラム

#include "common.h"

int main()
{
	MyMesh inMesh;

	//読み込み
	std::ifstream set("setting.txt");
	if (!set.is_open()) {
		printf("[ERROR] setting.txt does not exist.\nPlease make setting.txt file.\nPush Enter Key...");
		getchar();
		return -1;
	}

	// パラメータ入力　Setting.txtの中身
	std::string inf, outf, radf;
	// Base Params
	std::getline(set, inf);
	std::getline(set, outf);

	// ply 入力
	tri::io::ImporterPLY<MyMesh>::Open(inMesh, inf.c_str());

	// kdtree作成
	VertexConstDataWrapper<MyMesh> fww(inMesh);//ラッパーをつくって放り込む
	vcg::KdTree<float> tree(fww);
	std::vector<unsigned int> points;//頂点
	std::vector<float> squareDists;//点と点の距離
	double distance = atof(radf.c_str());//atofはdouble型に変換した数値。変換不能文字は0を返す。

	//データの平均を求める

	float sumx = 0.0f, sumy = 0.0f, sumz = 0.0f;
	vcg::Point3f axyz;
	//float ax=0, ay=0, az=0;
	for (size_t i = 0; i < inMesh.vert.size(); i++){ //sizeは頂点の数 for分は頂点の数だけ回る
		sumx += inMesh.vert[i].P().X(); //右辺は表示されない
		sumy += inMesh.vert[i].P().Y();
		sumz += inMesh.vert[i].P().Z();
	}
	axyz.X() = sumx / inMesh.vert.size();
	axyz.Y() = sumy / inMesh.vert.size();
	axyz.Z() = sumz / inMesh.vert.size();
	//重心表示
	printf("average = %f %f %f\n", axyz.X(), axyz.Y(), axyz.Z());

	unsigned int index;
	float dist;
	tree.doQueryClosest(axyz, index, dist);
	vcg::Point3f resp = inMesh.vert[index].P();
	printf("nearest neighber: %f,%f,%f\n", resp.X(), resp.Y(), resp.Z());

	return 0;
}