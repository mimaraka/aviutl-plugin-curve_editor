//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (バウンスモードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void cve::Curve_Bounce::init()
{
	coef_bounce = DEF_COEF_BOUNCE;
	coef_time = DEF_COEF_TIME;
}



//---------------------------------------------------------------------
//		バウンスの関数
//---------------------------------------------------------------------
double func1(double ratio, double e)
{
	return std::floor(std::log((e - 1) * ratio + 1) / std::log(e));
}

double func2(double ratio, double e)
{
	return ratio + 0.5 + 1. / (e - 1.) - (e + 1.) * std::pow(e, func1(ratio + 0.5, e)) / (2. * e - 2.);
}

double cve::Curve_Bounce::func_bounce(double ratio, double e, double t, double st, double ed)
{
	ratio = std::clamp(ratio, 0., 1.);
	const double lim_val = t * (1 / (1. - e) - 0.5);
	double result;
	e = std::min(e, 0.999);

	if (lim_val > 1.) {
		int n = (int)(std::log(1 + (e - 1.) * (1 / t + 0.5)) / std::log(e));
		if (ratio < t * ((std::pow(e, n) - 1.) / (e - 1.) - 0.5))
			result = (ed - st) * (4 * std::pow(func2(ratio / t, e), 2) - std::pow(e, 2 * func1(ratio / t + 0.5, e))) + ed;
		else
			result = ed;
	}
	else {
		if (ratio < lim_val)
			result = (ed - st) * (4 * std::pow(func2(ratio / t, e), 2) - std::pow(e, 2 * func1(ratio / t + 0.5, e))) + ed;
		else
			result = ed;
	}
	return result;
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve_Bounce::pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	POINT pt;

	param_to_pt(&pt);
	// 振幅を調整するハンドル(左)
	const RECT rect_point = {
		(LONG)to_client(aului::Point<LONG>(pt)).x - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).x + POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y + POINT_BOX_WIDTH
	};

	pt_address->index = NULL;

	if (::PtInRect(&rect_point, pt_client))
		pt_address->position = Point_Address::Center;
	else
		pt_address->position = Point_Address::Null;
}



//---------------------------------------------------------------------
//		カーブを反転
//---------------------------------------------------------------------
void cve::Curve_Bounce::reverse_curve()
{
	reverse = !reverse;
}



//---------------------------------------------------------------------
//		ポイントを移動
//---------------------------------------------------------------------
void cve::Curve_Bounce::move_handle(const POINT& pt_graph)
{
	coef_bounce = pt_to_param(pt_graph, 0);
	coef_time = pt_to_param(pt_graph, 1);
}



//---------------------------------------------------------------------
//		パラメータ -> ポイント
//---------------------------------------------------------------------
void cve::Curve_Bounce::param_to_pt(POINT* pt_graph)
{
	pt_graph->x = (int)((coef_time * (coef_bounce + 1) * 0.5) * CVE_GRAPH_RESOLUTION);
	pt_graph->y = (int)((1. - std::pow(coef_bounce, 2)) * CVE_GRAPH_RESOLUTION);
	if (reverse) {
		pt_graph->x = CVE_GRAPH_RESOLUTION - pt_graph->x;
		pt_graph->y = CVE_GRAPH_RESOLUTION - pt_graph->y;
	}
}



//---------------------------------------------------------------------
//		ポイント -> パラメータ
//		idx_param : パラメータのインデックス(0: coef_bounce, 1: coef_time)
//---------------------------------------------------------------------
double cve::Curve_Bounce::pt_to_param(const POINT& pt_graph, int idx_param)
{
	POINT pt;
	if (reverse) {
		pt.x = CVE_GRAPH_RESOLUTION - pt_graph.x;
		pt.y = CVE_GRAPH_RESOLUTION - pt_graph.y;
	}
	else
		pt = pt_graph;

	double e = std::sqrt(1. - std::clamp((int)pt.y, 1, CVE_GRAPH_RESOLUTION) / (double)CVE_GRAPH_RESOLUTION);
	double t = 2. * std::clamp((int)pt.x, 1, CVE_GRAPH_RESOLUTION) / ((e + 1.) * CVE_GRAPH_RESOLUTION);
	if (idx_param == 0)
		return e;
	else
		return t;
}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve_Bounce::draw_curve(
	aului::Direct2d_Paint_Object* paint_object,
	const RECT& rect_wnd,
	int drawing_mode)
{
	aului::Color handle_color;
	aului::Color curve_color;
	POINT pt_graph;

	if (drawing_mode == DRAW_CURVE_NORMAL) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_config.curve_color;
	}
	else if (drawing_mode == DRAW_CURVE_TRACE) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_theme[g_config.theme].curve_trace;
	}
	else {
		handle_color = g_theme[g_config.theme].handle_preset;
		curve_color = g_theme[g_config.theme].curve_preset;
	}

	float left_side = to_client(0.f, 0.f).x;
	float right_side = to_client((float)CVE_GRAPH_RESOLUTION, 0.f).x;

	// カーブを描画
	if (right_side < 0 || left_side > rect_wnd.right) {
		return;
	}
	if (left_side < 0) {
		left_side = 0;
	}
	if (right_side > rect_wnd.right) {
		right_side = (float)rect_wnd.right;
	}

	paint_object->brush->SetColor(D2D1::ColorF(curve_color.d2dcolor()));

	double y1, y2;

	for (double x = left_side; x < right_side; x += DRAW_GRAPH_STEP) {
		if (reverse) {
			y1 = func_bounce(1. - to_graph(x, 0.).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 1., 0.);
			y2 = func_bounce(1. - to_graph(x + DRAW_GRAPH_STEP, 0.).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 1., 0.);
		}
		else {
			y1 = func_bounce(to_graph(x, 0.).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 0., 1.);
			y2 = func_bounce(to_graph(x + DRAW_GRAPH_STEP, 0.).x / (double)CVE_GRAPH_RESOLUTION, coef_bounce, coef_time, 0., 1.);
		}
		paint_object->p_render_target->DrawLine(
			D2D1::Point2F(
				(float)x,
				(to_client(0.f, (float)(y1 * CVE_GRAPH_RESOLUTION)).y)),
			D2D1::Point2F(
				(float)(x + DRAW_GRAPH_STEP),
				(to_client(0.f, (float)(y2 * CVE_GRAPH_RESOLUTION)).y)),
			paint_object->brush, CURVE_THICKNESS
		);
	}

	// ハンドル・ポイントを描画
	if (g_config.show_handle) {
		paint_object->brush->SetColor(D2D1::ColorF(handle_color.d2dcolor()));

		param_to_pt(&pt_graph);

		// 振動数・減衰を調整するハンドル
		draw_handle(
			paint_object,
			to_client(aului::Point<float>(pt_graph)),
			to_client(aului::Point<float>(pt_graph)),
			drawing_mode, DRAW_HANDLE_ONLY
		);

		// 始点
		draw_handle(
			paint_object,
			to_client(0.f, 0.f),
			to_client(0.f, 0.f),
			drawing_mode, DRAW_POINT_ONLY
		);
		// 終点
		draw_handle(
			paint_object,
			to_client((float)CVE_GRAPH_RESOLUTION, (float)CVE_GRAPH_RESOLUTION),
			to_client((float)CVE_GRAPH_RESOLUTION, (float)CVE_GRAPH_RESOLUTION),
			drawing_mode, DRAW_POINT_ONLY
		);
	}
}



//---------------------------------------------------------------------
//		数値を生成
//---------------------------------------------------------------------
int cve::Curve_Bounce::create_number()
{
	int result;
	int x = (int)((coef_time * (coef_bounce + 1) * 0.5) * 1000);
	int y = (int)((1. - std::pow(coef_bounce, 2)) * 1000);
	result = y + 1001 * x + 10211202;
	if (reverse)
		result *= -1;
	return result;
}



//---------------------------------------------------------------------
//		数値を読み取り
//---------------------------------------------------------------------
bool cve::Curve_Bounce::read_number(int number, double* e, double* t, bool* rev)
{
	// -2147483647 ~  -11213203：不使用
	//   -11213202 ~  -10211202：バウンスが使用
	//   -10211201 ~         -1：振動が使用
	//           0             ：不使用
	//           1 ~   10211201：振動が使用
	//    10211202 ~   11213202：バウンスが使用
	//    11213203 ~ 2147483647：不使用
	int num;

	if (aului::in_range(number, -11213202, -10211202, true)) {
		*rev = true;
		num = -number - 10211202;
	}
	else if (aului::in_range(number, 10211202, 11213202, true)) {
		*rev = false;
		num = number - 10211202;
	}
	else return false;

	int x, y;

	x = num / 1001;
	y = num - x * 1001;
	*e = std::sqrt(1. - y * 0.001);
	*t = 2. * 0.001 * x / (*e + 1.);

	return true;
}



//---------------------------------------------------------------------
//		Luaに渡すやつ
//---------------------------------------------------------------------
double cve::Curve_Bounce::create_result(int number, double ratio, double st, double ed)
{
	double e, t;
	bool rev;

	ratio = std::clamp(ratio, 0., 1.);

	if (!read_number(number, &e, &t, &rev))
		return st + (ed - st) * ratio;

	if (rev)
		return func_bounce(1. - ratio, e, t, ed, st);
	else
		return func_bounce(ratio, e, t, st, ed);
}