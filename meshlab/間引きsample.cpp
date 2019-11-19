//間引きのサンプルソースです

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
	std::getline(set, radf);//半径

	// ply 入力
	tri::io::ImporterPLY<MyMesh>::Open(inMesh, inf.c_str());

	// kdtree作成
	VertexConstDataWrapper<MyMesh> fww(inMesh);//ラッパーをつくって放り込む
	vcg::KdTree<float> holetree(fww);
	std::vector<unsigned int> points;//頂点
	std::vector<float> squareDists;//点と点の距離
	double distance = atof(radf.c_str());//atofはdouble型に変換した数値。変換不能文字は0を返す。

	printf("Before VN: %d\n", inMesh.VN());//inMesh.VN、元の頂点数

	// PSR後の各頂点に対し、PSR前のkdtreeと比較
	// distance以内に点があれば points に格納する
	int vc;//カウント
	for (int j = 0; j < inMesh.VN(); j++) {
		holetree.doQueryDist(inMesh.vert[j].cP(), distance, points, squareDists);//ヒットしたindexが入る

		//printf("hit points size %d\n", points.size());
		vc = 0;
		for (int k = 0; k < points.size(); k++) {
			if (inMesh.vert[points[k]].visible == true) { vc++; }//有効である頂点が一つでも存在した場合
		}
		if (vc == 0) { inMesh.vert[j].visible = true; }//点を有効にする
		points.clear();
	}

	MyMesh outMesh;//空

	for (int j = 0; j < inMesh.VN(); j++) {
		if (inMesh.vert[j].visible == true) {
			vcg::tri::Allocator<MyMesh>::AddVertex(outMesh, inMesh.vert[j].P());//有効になっている点だけを加える
		}
	}
	printf("After VN: %d\n", outMesh.VN());

	// ply 出力
	tri::io::PlyInfo pi;
	pi.mask = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTNORMAL;
	tri::io::ExporterPLY<MyMesh>::Save(outMesh, outf.c_str(), pi.mask, true);

	printf("Finished. Push Enter Key...\n");
	getchar();

	return 0;
}