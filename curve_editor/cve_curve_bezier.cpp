//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ベジェモードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		数値を生成
//---------------------------------------------------------------------
int cve::Curve_Bezier::create_number()
{
	int result;
	aului::Point<float> ptf[2];
	POINT pt[2];

	ptf[0].x = std::clamp(
		ctpts[0].pt_right.x / (float)CVE_GRAPH_RESOLUTION,
		0.f, 1.f
	);

	ptf[0].y = std::clamp(
		ctpts[0].pt_right.y / (float)CVE_GRAPH_RESOLUTION,
		MIN_Y, MAX_Y
	);

	ptf[1].x = std::clamp(
		ctpts[1].pt_left.x / (float)CVE_GRAPH_RESOLUTION,
		0.f, 1.f
	);

	ptf[1].y = std::clamp(
		ctpts[1].pt_left.y / (float)CVE_GRAPH_RESOLUTION,
		MIN_Y, MAX_Y
	);

	for (int i = 0; i < 2; i++) {
		pt[i].x = (int)std::round(ptf[i].x * 100);
		pt[i].y = (int)std::round(ptf[i].y * 100);
	}
	// 計算
	result = 6600047 * (pt[1].y + 273) + 65347 * pt[1].x + 101 * (pt[0].y + 273) + pt[0].x - 2147483647;
	if (result >= -12368442)
		result += 24736885;
	return result;
}



//---------------------------------------------------------------------
//		パラメータを生成(","で区切る)
//---------------------------------------------------------------------
std::string cve::Curve_Bezier::create_parameters()
{
	aului::Point<float> pt;
	std::string str_x, str_y, result;

	pt.x = std::clamp(
		(float)(std::round(ctpts[0].pt_right.x * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		0.f, 1.f
	);
	pt.y = std::clamp(
		(float)(std::round(ctpts[0].pt_right.y * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		MIN_Y, MAX_Y
	);

	str_x = std::to_string(pt.x);
	str_y = std::to_string(pt.y);
	str_x.erase(4);

	if (ctpts[0].pt_right.y < 0)
		str_y.erase(5);
	else
		str_y.erase(4);
	result += str_x + ", " + str_y + ", ";

	pt.x = std::clamp(
		(float)(std::round(ctpts[1].pt_left.x * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		0.f, 1.f
	);
	pt.y = std::clamp(
		(float)(std::round(ctpts[1].pt_left.y * 100 / (double)CVE_GRAPH_RESOLUTION) * 0.01f),
		MIN_Y, MAX_Y
	);

	str_x = std::to_string(pt.x);
	str_y = std::to_string(pt.y);
	str_x.erase(4);

	if (ctpts[1].pt_left.y < 0)
		str_y.erase(5);
	else
		str_y.erase(4);
	result += str_x + ", " + str_y;

	return result;
}



//---------------------------------------------------------------------
//		パラメータを読み取り
//---------------------------------------------------------------------
bool cve::Curve_Bezier::read_parameters(LPCTSTR param, Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	const std::regex regex_param(R"(^(\s*(\d|-|\.)+\s*,){3}\s*(\d|-|\.)+\s*$)");

	if (std::regex_match(param, regex_param)) {
		std::string str_param(param);
		str_param.erase(std::remove_if(str_param.begin(), str_param.end(), ::isspace), str_param.end());
		std::vector<std::string> vec = aului::split(str_param, ',');

		float values[4];

		try {
			values[0] = std::clamp(std::stof(vec[0]), 0.f, 1.f);
			values[1] = std::clamp(std::stof(vec[1]), MIN_Y, MAX_Y);
			values[2] = std::clamp(std::stof(vec[2]), 0.f, 1.f);
			values[3] = std::clamp(std::stof(vec[3]), MIN_Y, MAX_Y);
		}
		catch (...) {
			return false;
		}

		points[0].pt_right.x = (int)(values[0] * CVE_GRAPH_RESOLUTION);
		points[0].pt_right.y = (int)(values[1] * CVE_GRAPH_RESOLUTION);
		points[1].pt_left.x = (int)(values[2] * CVE_GRAPH_RESOLUTION);
		points[1].pt_left.y = (int)(values[3] * CVE_GRAPH_RESOLUTION);

		return true;
	}
	else
		return false;
}



//---------------------------------------------------------------------
//		スクリプトに渡す値を生成
//---------------------------------------------------------------------
double cve::Curve_Bezier::create_result(int number, double ratio, double st, double ed)
{
	// -2147483647 ~  -12368443：ベジェが使用
	//   -12368442 ~         -1：IDが使用
	//           0             ：不使用
	//           1 ~   12368442：IDが使用
	//    12368443 ~ 2147483646：ベジェが使用
	//  2147483647             ：不使用
	
	Static_Array<Curve_Points, CVE_POINT_MAX> ctpts_buffer;

	ratio = std::clamp(ratio, 0., 1.);

	if (!read_number(number, ctpts_buffer))
		return st + (ed - st) * ratio;

	return st + get_bezier_value(ratio, ctpts_buffer) * (ed - st) / (double)CVE_GRAPH_RESOLUTION;
}