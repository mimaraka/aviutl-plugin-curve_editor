//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (振動モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_ELASTIC_FREQ_DEFAULT		8.0
#define CVE_ELASTIC_DECAY_DEFAULT		6.0
#define CVE_ELASTIC_AMP_DEFAULT			1.0



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void cve::Curve_Elastic::initialize()
{
	freq = CVE_ELASTIC_FREQ_DEFAULT;
	dec = CVE_ELASTIC_DECAY_DEFAULT;
	ampl = CVE_ELASTIC_AMP_DEFAULT;
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
		decay = (std::exp(-k * ratio) - std::exp(-k)) / (1.0 - std::exp(-k));

	return 1 - decay * std::cos(CVE_MATH_PI * f * ratio);
}

double cve::Curve_Elastic::func_elastic(double ratio, double f, double k, double a, double st, double ed)
{
	// 微分係数
	double elastic_der;

	// 二分法によりelasticの導関数の値から極値をとるratioを近似する
	double ratio_border = 1.0 / (2.0 * f);
	for (int i = 0; i < 12; i++) {
		if (k == 0)
			elastic_der = -std::cos(CVE_MATH_PI * f * ratio_border) - CVE_MATH_PI * f * (1 - ratio_border) * std::sin(CVE_MATH_PI * f * ratio_border);
		else {
			elastic_der = -k * std::exp(-k * ratio_border) * std::cos(CVE_MATH_PI * f * ratio_border) - CVE_MATH_PI * f * (std::exp(-k * ratio_border) - std::exp(-k)) * std::sin(CVE_MATH_PI * f * ratio_border);
		}

		if (elastic_der == 0)
			break;
		else if (elastic_der < 0)
			ratio_border += 1.0 / f * std::pow(0.5, i + 2);
		else
			ratio_border -= 1.0 / f * std::pow(0.5, i + 2);
	}
	double e_border = func_elastic_1(ratio_border, f, k);

	if (ratio < ratio_border) {
		return (ed - st) * (a * (e_border - 1.0) + 1.0) / e_border * func_elastic_1(ratio, f, k) + st;
	}
	else {
		return (ed - st) * (a * (func_elastic_1(ratio, f, k) - 1.0) + 1.0) + st;
	}
}



//---------------------------------------------------------------------
//		ポイント -> パラメータ
//		pt_graph_val	: 変換するポイントのグラフ座標(xまたはy)
//		idx_param		: パラメータのインデックス(0: ampl, 1: freq, 2: dec)
//---------------------------------------------------------------------
double cve::Curve_Elastic::pt_to_param(int pt_graph_val, int idx_param)
{
	if (reverse)
		pt_graph_val = CVE_GRAPH_RESOLUTION - pt_graph_val;
	// Amp
	if (idx_param == 0)
		return (MINMAX_LIMIT(pt_graph_val, CVE_GRAPH_RESOLUTION / 2, CVE_GRAPH_RESOLUTION) - CVE_GRAPH_RESOLUTION * 0.5) / (CVE_GRAPH_RESOLUTION * 0.5);
	// Freq
	else if (idx_param == 1)
		return MIN_LIMIT(2.0 / (MIN_LIMIT(pt_graph_val, 10) / (double)CVE_GRAPH_RESOLUTION), 2.0);
	// Decay
	else
		return -10.0 * std::log(-(MINMAX_LIMIT(pt_graph_val, 0, (int)(CVE_GRAPH_RESOLUTION * 0.5 - 1)) / (CVE_GRAPH_RESOLUTION * 0.5)) + 1.0) + 1.0;
}



//---------------------------------------------------------------------
//		パラメータ -> ポイント
//		pt_graph		: 変換するポイントのグラフ座標
//		idx_pt			: ポイントのインデックス(0: 振幅(左), 1: 振幅(右), 2: 振動数・減衰)
//---------------------------------------------------------------------
void cve::Curve_Elastic::param_to_pt(POINT* pt_graph, int idx_pt)
{
	if (idx_pt <= 1) {
		if (idx_pt == 0)
			pt_graph->x = 0;
		else
			pt_graph->x = CVE_GRAPH_RESOLUTION;
		if (reverse)
			pt_graph->y = (int)(CVE_GRAPH_RESOLUTION * 0.5 * (-ampl + 1.0));
		else
			pt_graph->y = (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0));
	}
	else if (idx_pt == 2) {
		if (reverse) {
			pt_graph->x = (int)(CVE_GRAPH_RESOLUTION * (1 - 2.0 / freq));
			pt_graph->y = CVE_GRAPH_RESOLUTION + (int)((std::exp(-(dec - 1.0) * 0.1) - 1.0) * CVE_GRAPH_RESOLUTION * 0.5);
		}
		else {
			pt_graph->x = (int)(2.0 * CVE_GRAPH_RESOLUTION / freq);
			pt_graph->y = -(int)((std::exp(-(dec - 1.0) * 0.1) - 1.0) * CVE_GRAPH_RESOLUTION * 0.5);
		}
	}
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve_Elastic::pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	POINT pt;

	param_to_pt(&pt, 0);
	// 振幅を調整するハンドル(左)
	const RECT handle_amp_left = {
		(LONG)to_client(pt).x - CVE_POINT_RANGE,
		(LONG)to_client(pt).y - CVE_POINT_RANGE,
		(LONG)to_client(pt).x + CVE_POINT_RANGE,
		(LONG)to_client(pt).y + CVE_POINT_RANGE
	};

	param_to_pt(&pt, 1);

	// 振幅を調整するハンドル(右)
	const RECT handle_amp_right = {
		(LONG)to_client(pt).x - CVE_POINT_RANGE,
		(LONG)to_client(pt).y - CVE_POINT_RANGE,
		(LONG)to_client(pt).x + CVE_POINT_RANGE,
		(LONG)to_client(pt).y + CVE_POINT_RANGE
	};

	param_to_pt(&pt, 2);

	// 振動数・減衰を調整するハンドル
	const RECT pt_freq_decay = {
		(LONG)to_client(pt).x - CVE_POINT_RANGE,
		(LONG)to_client(pt).y - CVE_POINT_RANGE,
		(LONG)to_client(pt).x + CVE_POINT_RANGE,
		(LONG)to_client(pt).y + CVE_POINT_RANGE
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
void cve::Curve_Elastic::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;
	POINT pt_graph;

	if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_config.curve_color;
	}
	else if (drawing_mode == CVE_DRAW_CURVE_TRACE) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_theme[g_config.theme].curve_trace;
	}
	else {
		handle_color = g_theme[g_config.theme].handle_preset;
		curve_color = g_theme[g_config.theme].curve_prset;
	}

	float left_side = to_client(0, 0).x;
	float right_side = to_client(CVE_GRAPH_RESOLUTION, 0).x;

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

	bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(curve_color)));

	float y1, y2;

	for (float x = left_side; x < right_side; x += CVE_DRAW_GRAPH_INCREASEMENT) {
		if (reverse) {
			y1 = func_elastic(1.0 - to_graph(x, 0).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 1.0, 0.5);
			y2 = func_elastic(1.0 - to_graph(x + CVE_DRAW_GRAPH_INCREASEMENT, 0).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 1.0, 0.5);
		}
		else {
			y1 = func_elastic(to_graph(x, 0).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 0.0, 0.5);
			y2 = func_elastic(to_graph(x + CVE_DRAW_GRAPH_INCREASEMENT, 0).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 0.0, 0.5);
		}
		g_render_target->DrawLine(
			D2D1::Point2F(
				x,
				(float)(to_client(0, (int)(y1 * CVE_GRAPH_RESOLUTION)).y)),
			D2D1::Point2F(
				x + CVE_DRAW_GRAPH_INCREASEMENT,
				(float)(to_client(0, (int)(y2 * CVE_GRAPH_RESOLUTION)).y)),
			bitmap_buffer->brush, CVE_CURVE_THICKNESS
		);
	}

	// ハンドル・ポイントを描画
	if (g_config.show_handle) {
		bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(handle_color)));

		param_to_pt(&pt_graph, 0);

		// 振幅を調整するハンドル
		draw_handle(
			bitmap_buffer,
			to_client(pt_graph),
			to_client(CVE_GRAPH_RESOLUTION, pt_graph.y),
			drawing_mode, CVE_DRAW_HANDLE_ONLY
		);

		param_to_pt(&pt_graph, 2);

		// 振動数・減衰を調整するハンドル
		draw_handle(
			bitmap_buffer,
			to_client(pt_graph.x, (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			to_client(pt_graph),
			drawing_mode, NULL
		);

		// 始点
		draw_handle(
			bitmap_buffer,
			to_client(0, reverse ? (int)(CVE_GRAPH_RESOLUTION * 0.5) : 0),
			to_client(0, reverse ? (int)(CVE_GRAPH_RESOLUTION * 0.5) : 0),
			drawing_mode, CVE_DRAW_POINT_ONLY
		);
		// 終点
		draw_handle(
			bitmap_buffer,
			to_client(CVE_GRAPH_RESOLUTION, reverse ? CVE_GRAPH_RESOLUTION : (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			to_client(CVE_GRAPH_RESOLUTION, reverse ? CVE_GRAPH_RESOLUTION : (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			drawing_mode, CVE_DRAW_POINT_ONLY
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
	int k = -(int)(100 * (std::exp(-(dec - 1.0) * 0.1) - 1.0));
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

	if (ISINRANGEEQ(number, -10211201, -1)) {
		*rev = true;
		num = -number - 1;
	}
	else if (ISINRANGEEQ(number, 1, 10211201)) {
		*rev = false;
		num = number - 1;
	}
	else return false;

	int f_i, k_i, a_i;

	f_i = num / (101 * 101);
	k_i = (num - f_i * 101 * 101) / 101;
	a_i = num - f_i * 101 * 101 - k_i * 101;
	*f = 2.0 / MIN_LIMIT(f_i * 0.001, 0.001);
	*k = -10.0 * std::log(-k_i * 0.01 + 1.0) + 1.0;
	*a = MINMAX_LIMIT(a_i, 0, 100) * 0.01;

	return true;
}



//---------------------------------------------------------------------
//		Luaに渡すやつ
//---------------------------------------------------------------------
double cve::Curve_Elastic::create_result(int number, double ratio, double st, double ed)
{
	double f, k, a;
	bool rev;

	ratio = MINMAX_LIMIT(ratio, 0.0, 1.0);

	if (!read_number(number, &f, &k, &a, &rev))
		return st + (ed - st) * ratio;

	if (rev)
		return func_elastic(1.0 - ratio, f, k, a, ed, st);
	else
		return func_elastic(ratio, f, k, a, st, ed);
}