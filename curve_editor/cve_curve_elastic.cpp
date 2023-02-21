//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (振動モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void cve::Curve_Elastic::init()
{
	freq = DEF_FREQ;
	dec = DEF_DECAY;
	ampl = DEF_AMP;
}



//---------------------------------------------------------------------
//		振動の関数
//---------------------------------------------------------------------
double func_elastic_1(double ratio, double f, double k)
{
	double decay;
	if (k == 0)
		decay = 1 - ratio;
	else
		decay = (std::exp(-k * ratio) - std::exp(-k)) / (1. - std::exp(-k));

	return 1 - decay * std::cos(std::numbers::pi * f * ratio);
}

double cve::Curve_Elastic::func_elastic(double ratio, double f, double k, double a, double st, double ed)
{
	// 微分係数
	double elastic_der;

	// 二分法によりelasticの導関数の値から極値をとるratioを近似する
	double ratio_border = 1. / (2. * f);
	for (int i = 0; i < 12; i++) {
		if (k == 0)
			elastic_der = -std::cos(std::numbers::pi * f * ratio_border) - std::numbers::pi * f * (1 - ratio_border) * std::sin(std::numbers::pi * f * ratio_border);
		else {
			elastic_der = -k * std::exp(-k * ratio_border) * std::cos(std::numbers::pi * f * ratio_border) - std::numbers::pi * f * (std::exp(-k * ratio_border) - std::exp(-k)) * std::sin(std::numbers::pi * f * ratio_border);
		}

		if (elastic_der == 0)
			break;
		else if (elastic_der < 0)
			ratio_border += 1. / f * std::pow(0.5, i + 2);
		else
			ratio_border -= 1. / f * std::pow(0.5, i + 2);
	}
	double e_border = func_elastic_1(ratio_border, f, k);

	if (ratio < ratio_border) {
		return (ed - st) * (a * (e_border - 1.) + 1.) / e_border * func_elastic_1(ratio, f, k) + st;
	}
	else {
		return (ed - st) * (a * (func_elastic_1(ratio, f, k) - 1.) + 1.) + st;
	}
}



//---------------------------------------------------------------------
//		ポイント -> パラメータ
//		pt_graph_val	: 変換するポイントのグラフ座標(xまたはy)
//		idx_param		: パラメータのインデックス(0: ampl, 1: freq, 2: dec)
//---------------------------------------------------------------------
double cve::Curve_Elastic::pt_to_param(int pt_graph_val, int idx_param)
{
	// Amp
	if (idx_param == 0)
		return (std::clamp(pt_graph_val, CVE_GRAPH_RESOLUTION / 2, CVE_GRAPH_RESOLUTION) - CVE_GRAPH_RESOLUTION * 0.5) / (CVE_GRAPH_RESOLUTION * 0.5);
	// Freq
	else if (idx_param == 1) {
		if (reverse)
			pt_graph_val = CVE_GRAPH_RESOLUTION - pt_graph_val;
		return std::max(2. / (std::max(pt_graph_val, 10) / (double)CVE_GRAPH_RESOLUTION), 2.);
	}
	// Decay
	else
		return -10. * std::log(-(std::clamp(pt_graph_val, 0, (int)(CVE_GRAPH_RESOLUTION * 0.5 - 1)) / (CVE_GRAPH_RESOLUTION * 0.5)) + 1.) + 1.;
}



//---------------------------------------------------------------------
//		パラメータ -> ポイント
//		pt_graph		: 変換するポイントのグラフ座標
//		idx_pt			: ポイントのインデックス(0: 振幅(左), 1: 振幅(右), 2: 振動数・減衰)
//---------------------------------------------------------------------
void cve::Curve_Elastic::param_to_pt(POINT* pt_graph, int idx_pt)
{
	// 振幅のハンドル
	if (idx_pt <= 1) {
		if (idx_pt == 0)
			pt_graph->x = 0;
		else
			pt_graph->x = CVE_GRAPH_RESOLUTION;
		pt_graph->y = (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.));
	}
	else if (idx_pt == 2) {
		if (reverse)
			pt_graph->x = (int)(CVE_GRAPH_RESOLUTION * (1 - 2. / freq));
		else
			pt_graph->x = (int)(2. * CVE_GRAPH_RESOLUTION / freq);
		pt_graph->y = -(int)((std::exp(-(dec - 1.) * 0.1) - 1.) * CVE_GRAPH_RESOLUTION * 0.5);
	}
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve_Elastic::pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	POINT pt;

	param_to_pt(&pt, 0);
	// 振幅を調整するハンドル(左)
	const RECT handle_amp_left = {
		(LONG)to_client(aului::Point<LONG>(pt)).x - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).x + POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y + POINT_BOX_WIDTH
	};

	param_to_pt(&pt, 1);

	// 振幅を調整するハンドル(右)
	const RECT handle_amp_right = {
		(LONG)to_client(aului::Point<LONG>(pt)).x - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).x + POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y + POINT_BOX_WIDTH
	};

	param_to_pt(&pt, 2);

	// 振動数・減衰を調整するハンドル
	const RECT pt_freq_decay = {
		(LONG)to_client(aului::Point<LONG>(pt)).x - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y - POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).x + POINT_BOX_WIDTH,
		(LONG)to_client(aului::Point<LONG>(pt)).y + POINT_BOX_WIDTH
	};

	if (::PtInRect(&handle_amp_left, pt_client) || ::PtInRect(&handle_amp_right, pt_client)) {
		pt_address->index = 0;
		pt_address->position = Point_Address::Center;
	}
	else if (::PtInRect(&pt_freq_decay, pt_client)) {
		pt_address->index = 1;
		pt_address->position = Point_Address::Center;
	}
	else {
		pt_address->index = NULL;
		pt_address->position = Point_Address::Null;
	}
}



//---------------------------------------------------------------------
//		ハンドルを移動
//---------------------------------------------------------------------
void cve::Curve_Elastic::move_handle(const Point_Address pt_address, const POINT& pt_graph)
{
	switch (pt_address.index) {
	// 振幅
	case 0:
		ampl = pt_to_param(pt_graph.y, 0);
		break;

	// 振動数, 減衰
	case 1:
		freq = pt_to_param(pt_graph.x, 1);
		dec = pt_to_param(pt_graph.y, 2);
		break;
	}
}



//---------------------------------------------------------------------
//		カーブを反転
//---------------------------------------------------------------------
void cve::Curve_Elastic::reverse_curve()
{
	reverse = !reverse;
}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve_Elastic::draw_curve(
	Cve_Paint_Object* paint_object,
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
			y1 = func_elastic(1. - to_graph(x, 0.).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 1., .5);
			y2 = func_elastic(1. - to_graph(x + DRAW_GRAPH_STEP, 0.).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 1., .5);
		}
		else {
			y1 = func_elastic(to_graph(x, 0.).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 0., .5);
			y2 = func_elastic(to_graph(x + DRAW_GRAPH_STEP, 0.).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 0., .5);
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

		param_to_pt(&pt_graph, 0);

		// 振幅を調整するハンドル
		paint_object->draw_handle(
			to_client(aului::Point<float>(pt_graph)),
			to_client((float)CVE_GRAPH_RESOLUTION, (float)pt_graph.y),
			drawing_mode, DRAW_HANDLE_ONLY
		);

		param_to_pt(&pt_graph, 2);

		// 振動数・減衰を調整するハンドル
		paint_object->draw_handle(
			to_client((float)pt_graph.x, CVE_GRAPH_RESOLUTION * .5f),
			to_client(aului::Point<float>(pt_graph)),
			drawing_mode, NULL
		);

		// 始点
		paint_object->draw_handle(
			to_client(0.f, reverse ? CVE_GRAPH_RESOLUTION * .5f : 0.f),
			to_client(0.f, reverse ? CVE_GRAPH_RESOLUTION * .5f : 0.f),
			drawing_mode, DRAW_POINT_ONLY
		);
		// 終点
		paint_object->draw_handle(
			to_client((float)CVE_GRAPH_RESOLUTION, reverse ? (float)CVE_GRAPH_RESOLUTION : CVE_GRAPH_RESOLUTION * .5f),
			to_client((float)CVE_GRAPH_RESOLUTION, reverse ? (float)CVE_GRAPH_RESOLUTION : CVE_GRAPH_RESOLUTION * .5f),
			drawing_mode, DRAW_POINT_ONLY
		);
	}
}



//---------------------------------------------------------------------
//		数値を生成
//---------------------------------------------------------------------
int cve::Curve_Elastic::create_number()
{
	int result;
	int f = (int)(2000 / freq);
	int a = (int)(100 * ampl);
	int k = -(int)(100 * (std::exp(-(dec - 1.) * .1) - 1.));
	result = 1 + a + k * 101 + f * 101 * 101;
	if (reverse)
		result *= -1;
	return result;
}



//---------------------------------------------------------------------
//		数値を読み取り
//---------------------------------------------------------------------
bool cve::Curve_Elastic::read_number(int number, double* f, double* k, double* a, bool* rev)
{
	// -2147483647 ~  -11213203：不使用
	//   -11213202 ~  -10211202：バウンスが使用
	//   -10211201 ~         -1：振動が使用
	//           0             ：不使用
	//           1 ~   10211201：振動が使用
	//    10211202 ~   11213202：バウンスが使用
	//    11213203 ~ 2147483647：不使用

	int num;

	if (aului::in_range(number, -10211201, -1, true)) {
		*rev = true;
		num = -number - 1;
	}
	else if (aului::in_range(number, 1, 10211201, true)) {
		*rev = false;
		num = number - 1;
	}
	else return false;

	int f_i, k_i, a_i;

	f_i = num / (101 * 101);
	k_i = (num - f_i * 101 * 101) / 101;
	a_i = num - f_i * 101 * 101 - k_i * 101;
	*f = 2. / std::max(f_i * 0.001, 0.001);
	*k = -10. * std::log(-k_i * 0.01 + 1.) + 1.;
	*a = std::clamp(a_i, 0, 100) * 0.01;

	return true;
}



//---------------------------------------------------------------------
//		Luaに渡すやつ
//---------------------------------------------------------------------
double cve::Curve_Elastic::create_result(int number, double ratio, double st, double ed)
{
	double f, k, a;
	bool rev;

	ratio = std::clamp(ratio, 0., 1.);

	if (!read_number(number, &f, &k, &a, &rev))
		return st + (ed - st) * ratio;

	if (rev)
		return func_elastic(1. - ratio, f, k, a, ed, st);
	else
		return func_elastic(ratio, f, k, a, st, ed);
}