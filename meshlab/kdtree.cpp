#include "stdafx.h"
#include "CCore.h"
#include "../nlib/nuadgl.h"

#include "MyMesh.h"

#include <vcg/complex/algorithms/create/ball_pivoting.h>


#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>

#include <iostream>
#include <fstream>

標準ライブラリ
#include<vector>
#include<string>
#include<algorithm>
#include<cassert>
#include<cassert>
#include <iomanip>

#include<Windows.h>


unsafe対策
#pragma warning (disable:4996)


int main()
{

	MyMesh m_mesh_Standard;
	MyMesh m_mesh_Comparison;

	double m_distance;


	//read_fileの内容
	setlocale(LC_ALL, "");

	std::ifstream fs("settings.txt");
	std::string fname;
	//基準ファイルを読み込む
	std::getline(fs, fname);
	vcg::tri::io::ImporterPLY<MyMesh>::Open(m_mesh_Standard, fname.c_str());

	//ゆがんだファイルを読み込む
	std::getline(fs, fname);
	vcg::tri::io::ImporterPLY<MyMesh>::Open(m_mesh_Comparison, fname.c_str());

	std::string value;
	std::getline(fs, value);
	m_distance = atof(value.c_str());


	//kd-treeの内容

		//バウンディングボックスを更新
		//※これは差分表示のために行う
		vcg::tri::UpdateBounding<MyMesh>::Box(m_mesh_Standard);
		vcg::tri::UpdateBounding<MyMesh>::Box(m_mesh_Comparison);

		//面法線計算
		vcg::tri::UpdateNormal<MyMesh>::PerFace(m_mesh_Standard);
		vcg::tri::UpdateNormal<MyMesh>::PerFace(m_mesh_Comparison);

		//距離比較の基準
		//赤==対角線の1/m_distance以上離れた場合
		const float different_red = m_mesh_Standard.bbox.Diag() / m_distance;



		/////////////////////////
		// 基準ファイルの点群のKdTree作成
		vcg::VertexConstDataWrapper<MyMesh> ww_ms(m_mesh_Standard);
		vcg::KdTree<float> tree(ww_ms);

		unsigned int index;
		float dist;

		for (int i = 0; i < m_mesh_Comparison.vert.size(); i++) {

			//ある点に最も近い基準ファイルの点のindexとそこまでの距離を求める
			//※ 距離dist内の点をすべて求める場合tree.doQueryDist()を使う
			tree.doQueryClosest(m_mesh_Comparison.vert[i].cP(), index, dist)

		}

	}