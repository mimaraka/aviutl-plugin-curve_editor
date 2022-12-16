//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (振動モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_ELASTIC_FREQ_MAX			100
#define CVE_ELASTIC_FREQ_DEFAULT		5.0
#define CVE_ELASTIC_DECAY_DEFAULT		10.0
#define CVE_ELASTIC_AMP_DEFAULT			1.0
#define CVE_ELASTIC_SMOOTH_COEF			1.94



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
//		振動の関数1
//---------------------------------------------------------------------
double func_elastic_1(double t, double f, double k)
{
	double decay;
	if (k == 0)
		decay = 1 - t;
	else
		decay = (std::exp(-k * t) - std::exp(-k)) / (1.0 - std::exp(-k));

	return 1 - decay * std::cos(CVE_MATH_PI * f * t);
}



//---------------------------------------------------------------------
//		振動の関数
//---------------------------------------------------------------------
double cve::Curve_Elastic::func_elastic(double t, double f, double k, double a, double st, double ed)
{
	// 微分係数
	double elastic_der;

	// 二分法によりelasticの導関数の値から極値をとるtを近似する
	double t_border = 1.0 / (2.0 * f);
	for (int i = 0; i < 12; i++) {
		if (k == 0)
			elastic_der = -std::cos(CVE_MATH_PI * f * t_border) - CVE_MATH_PI * f * (1 - t_border) * std::sin(CVE_MATH_PI * f * t_border);
		else {
			elastic_der = -k * std::exp(-k * t_border) * std::cos(CVE_MATH_PI * f * t_border) - CVE_MATH_PI * f * (std::exp(-k * t_border) - std::exp(-k)) * std::sin(CVE_MATH_PI * f * t_border);
		}

		if (elastic_der == 0)
			break;
		else if (elastic_der < 0)
			t_border += 1.0 / f * std::pow(0.5, i + 2);
		else
			t_border -= 1.0 / f * std::pow(0.5, i + 2);
	}
	double e_border = func_elastic_1(t_border, f, k);

	if (t < t_border) {
		return (ed - st) * (a * (e_border - 1.0) + 1.0) / e_border * func_elastic_1(t, f, k) + st;
	}
	else {
		return (ed - st) * (a * (func_elastic_1(t, f, k) - 1.0) + 1.0) + st;
	}
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve_Elastic::pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	// 振幅を調整するハンドル(左)
	const RECT handle_amp_left = {
		(LONG)to_client(0, 0).x - CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0))).y - CVE_POINT_RANGE,
		(LONG)to_client(0, 0).x + CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0))).y + CVE_POINT_RANGE
	};

	// 振幅を調整するハンドル(右)
	const RECT handle_amp_right = {
		(LONG)to_client(CVE_GRAPH_RESOLUTION, 0).x - CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0))).y - CVE_POINT_RANGE,
		(LONG)to_client(CVE_GRAPH_RESOLUTION, 0).x + CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0))).y + CVE_POINT_RANGE
	};

	// 振動数・減衰を調整するハンドル
	const RECT pt_freq_decay = {
		(LONG)to_client((int)(2.0 * CVE_GRAPH_RESOLUTION / freq), 0).x - CVE_POINT_RANGE,
		(LONG)to_client(0, -(int)((std::exp(-(dec - 1.0) * 0.1) - 1.0) * CVE_GRAPH_RESOLUTION * 0.5)).y - CVE_POINT_RANGE,
		(LONG)to_client((int)(2.0 * CVE_GRAPH_RESOLUTION / freq), 0).x + CVE_POINT_RANGE,
		(LONG)to_client(0, -(int)((std::exp(-(dec - 1.0) * 0.1) - 1.0) * CVE_GRAPH_RESOLUTION * 0.5)).y + CVE_POINT_RANGE
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
	int x;
	switch (pt_address.index) {
	// 振幅
	case 0:
		ampl = (MINMAX_LIMIT(pt_graph.y, CVE_GRAPH_RESOLUTION / 2, CVE_GRAPH_RESOLUTION) - CVE_GRAPH_RESOLUTION * 0.5) / (CVE_GRAPH_RESOLUTION * 0.5);
		break;

	// 振動数, 減衰
	case 1:
		x = MIN_LIMIT(pt_graph.x, 10);
		freq = MIN_LIMIT(2.0 / (x / (double)CVE_GRAPH_RESOLUTION), 2.0);
		dec = -10.0 * std::log(-(MINMAX_LIMIT(pt_graph.y, 0, (int)(CVE_GRAPH_RESOLUTION * 0.5)) / (CVE_GRAPH_RESOLUTION * 0.5)) + 1.0) + 1.0;
		break;
	}
}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve_Elastic::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;

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
	else if (left_side < 0) {
		left_side = 0;
	}
	else if (right_side > rect_wnd.right) {
		right_side = (float)rect_wnd.right;
	}

	bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(curve_color)));

	for (float x = left_side; x < right_side; x += CVE_DRAW_GRAPH_INCREASEMENT) {
		g_render_target->DrawLine(
			D2D1::Point2F(
				x,
				(float)(to_client(0, (int)(func_elastic(to_graph(x, 0).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 0.0, 0.5) * CVE_GRAPH_RESOLUTION)).y)),
			D2D1::Point2F(
				x + CVE_DRAW_GRAPH_INCREASEMENT,
				(float)(to_client(0, (int)(func_elastic(to_graph(x + CVE_DRAW_GRAPH_INCREASEMENT, 0).x / (double)CVE_GRAPH_RESOLUTION, freq, dec, ampl, 0.0, 0.5) * CVE_GRAPH_RESOLUTION)).y)),
			bitmap_buffer->brush, CVE_CURVE_THICKNESS
		);
	}

	// ハンドル・ポイントを描画
	if (g_config.show_handle) {
		bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(handle_color)));

		// 振幅を調整するハンドル
		draw_handle(
			bitmap_buffer,
			to_client(0, (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0))),
			to_client(CVE_GRAPH_RESOLUTION, (int)(CVE_GRAPH_RESOLUTION * 0.5 * (ampl + 1.0))),
			drawing_mode, CVE_DRAW_HANDLE_ONLY
		);

		// 振動数・減衰を調整するハンドル
		draw_handle(
			bitmap_buffer,
			to_client((int)(2.0 * CVE_GRAPH_RESOLUTION / freq), (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			to_client((int)(2.0 * CVE_GRAPH_RESOLUTION / freq), -(int)((std::exp(-(dec - 1.0) * 0.1) - 1.0) * CVE_GRAPH_RESOLUTION * 0.5)),
			drawing_mode, NULL
		);

		// 始点
		draw_handle(
			bitmap_buffer,
			to_client(0, 0),
			to_client(0, 0),
			drawing_mode, CVE_DRAW_POINT_ONLY
		);
		// 終点
		draw_handle(
			bitmap_buffer,
			to_client(CVE_GRAPH_RESOLUTION, (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			to_client(CVE_GRAPH_RESOLUTION, (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			drawing_mode, CVE_DRAW_POINT_ONLY
		);
	}
}



//---------------------------------------------------------------------
//		数値を生成
//---------------------------------------------------------------------
int cve::Curve_Elastic::create_number()
{
	int f = (int)(2000 / freq);
	int a = (int)(100 * ampl);
	int k = -(int)(100 * (std::exp(-(dec - 1.0) * 0.1) - 1.0));
	return a + k * 101 + f * 101 * 1001;
}



//---------------------------------------------------------------------
//		数値を読み取り
//---------------------------------------------------------------------
void cve::Curve_Elastic::read_number(int number, double* f, double* k, double* a)
{
	*f = std::floor(number / (101 * 1001));
	*k = std::floor((number - *f * 101 * 1001) / 101);
	*a = std::floor(number - *f * 101 * 1001 - *k * 101);
	*f = 2.0 / MIN_LIMIT(*f * 0.001, 0.001);
	*k = -10.0 * std::log(-*k * 0.01 + 1.0) + 1.0;
	*a = MINMAX_LIMIT(*a, 0, 100) * 0.01;
}



//---------------------------------------------------------------------
//		Luaに渡すやつ
//---------------------------------------------------------------------
double cve::Curve_Elastic::create_result(int number, double ratio, double st, double ed)
{
	
	if (number >= 0) {

	}
	else {

	}
	double f, k, a;
	read_number(number, &f, &k, &a);
	return func_elastic(ratio, f, k, a, st, ed);
}