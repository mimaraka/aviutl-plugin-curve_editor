//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (マルチベジェモードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		スクリプトに渡す値を生成
//---------------------------------------------------------------------
double cve::Curve_Multibezier::create_result(double ratio, double st, double ed)
{
	// 進捗が0~1の範囲外であった場合
	if (!ISINRANGEEQ(ratio, 0, 1))
		return 0;
	else
		return st + get_bezier_value(ratio, ctpts) * (ed - st) / (double)CVE_GRAPH_RESOLUTION;
}