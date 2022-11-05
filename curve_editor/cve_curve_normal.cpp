//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (標準モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		数値を生成
//---------------------------------------------------------------------
int cve::Curve_Normal::create_number()
{
	int result;
	Float_Point ptf[2];
	POINT pt[2];

	ptf[0].x = MINMAX_LIMIT(
		ctpts[0].pt_right.x / (float)CVE_GRAPH_RESOLUTION,
		0, 1
	);

	ptf[0].y = MINMAX_LIMIT(
		ctpts[0].pt_right.y / (float)CVE_GRAPH_RESOLUTION,
		CVE_CURVE_VALUE_MIN_Y,
		CVE_CURVE_VALUE_MAX_Y
	);

	ptf[1].x = MINMAX_LIMIT(
		ctpts[1].pt_left.x / (float)CVE_GRAPH_RESOLUTION,
		0, 1
	);

	ptf[1].y = MINMAX_LIMIT(
		ctpts[1].pt_left.y / (float)CVE_GRAPH_RESOLUTION,
		CVE_CURVE_VALUE_MIN_Y,
		CVE_CURVE_VALUE_MAX_Y
	);

	for (int i = 0; i < 2; i++) {
		pt[i].x = (int)std::round(ptf[i].x * 100);
		pt[i].y = (int)std::round(ptf[i].y * 100);
	}
	// 計算
	result = 6600047 * (pt[1].y + 273) + 65347 * pt[1].x + 101 * (pt[0].y + 273) + pt[0].x - 2147483647;
	return result;
}



//---------------------------------------------------------------------
//		パラメータを生成(","で区切る)
//---------------------------------------------------------------------
std::string cve::Curve::create_parameters()
{
	Float_Point pt;
	std::string strx, stry, result;

	pt.x = MINMAX_LIMIT(
		(float)(std::round(ctpts[0].pt_right.x * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		0, 1
	);
	pt.y = MINMAX_LIMIT(
		(float)(std::round(ctpts[0].pt_right.y * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		CVE_CURVE_VALUE_MIN_Y,
		CVE_CURVE_VALUE_MAX_Y
	);

	strx = std::to_string(pt.x);
	stry = std::to_string(pt.y);
	strx.erase(4);

	if (ctpts[0].pt_right.y < 0)
		stry.erase(5);
	else
		stry.erase(4);
	result += strx + ", " + stry + ", ";

	pt.x = MINMAX_LIMIT(
		(float)(std::round(ctpts[1].pt_left.x * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		0, 1
	);
	pt.y = MINMAX_LIMIT(
		(float)(std::round(ctpts[1].pt_left.y * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		CVE_CURVE_VALUE_MIN_Y,
		CVE_CURVE_VALUE_MAX_Y
	);

	strx = std::to_string(pt.x);
	stry = std::to_string(pt.y);
	strx.erase(4);

	if (ctpts[1].pt_left.y < 0)
		stry.erase(5);
	else
		stry.erase(4);
	result += strx + ", " + stry;

	return result;
}