//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (振動モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_ELASTIC_FREQ_MAX			100
#define CVE_ELASTIC_FREQ_DEFAULT		5.0



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void cve::Curve_Elastic::initialize()
{
	freq = CVE_ELASTIC_FREQ_DEFAULT;
}



//---------------------------------------------------------------------
//		振動の関数
//---------------------------------------------------------------------
double cve::Curve_Elastic::func_elastic(double st, double ed, double f, double t)
{
	return (st - ed) * std::pow(CVE_ELASTIC_DECAY, t) * std::cos(CVE_MATH_PI * f * t) + ed;
}



//---------------------------------------------------------------------
//		振動の関数の値が最大となる点を返す
//---------------------------------------------------------------------
void cve::Curve_Elastic::elastic_maxpoint(double st, double ed, double f, Double_Point* pt)
{
	const double extpoint_x = (std::atan(std::log(CVE_ELASTIC_DECAY) / (CVE_MATH_PI * f)) + CVE_MATH_PI) / (CVE_MATH_PI * f);
	const double extpoint_y = func_elastic(0, 1, f, extpoint_x);
	*pt = { extpoint_x, extpoint_y };
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve_Elastic::pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	Double_Point pt;
	elastic_maxpoint(0, 1, freq, &pt);

	const RECT handle_left = {
		(LONG)to_client(0, 0).x - CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(pt.y * CVE_GRAPH_RESOLUTION * 0.5)).y - CVE_POINT_RANGE,
		(LONG)to_client(0, 0).x + CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(pt.y * CVE_GRAPH_RESOLUTION * 0.5)).y + CVE_POINT_RANGE
	};

	const RECT handle_right = {
		(LONG)to_client(CVE_GRAPH_RESOLUTION, 0).x - CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(pt.y * CVE_GRAPH_RESOLUTION * 0.5)).y - CVE_POINT_RANGE,
		(LONG)to_client(CVE_GRAPH_RESOLUTION, 0).x + CVE_POINT_RANGE,
		(LONG)to_client(0, (int)(pt.y * CVE_GRAPH_RESOLUTION * 0.5)).y + CVE_POINT_RANGE
	};
	pt_address->index = NULL;

	if (::PtInRect(&handle_left, pt_client) || ::PtInRect(&handle_right, pt_client))
		pt_address->position = Point_Address::Center;
	else
		pt_address->position = Point_Address::Null;
}



//---------------------------------------------------------------------
//		ハンドルの位置からfreqを決定
//---------------------------------------------------------------------
void cve::Curve_Elastic::move_handle(int pt_graph_y)
{
	double x = pt_graph_y / (double)CVE_GRAPH_RESOLUTION * 2;
	Double_Point extpoint;

	// 二分法によりxの値からfを近似する
	// fの範囲は1~CVE_ELASTIC_FREQ_MAX
	double f_border = (CVE_ELASTIC_FREQ_MAX + 1) / 2.0;
	for (int i = 0; i < 12; i++) {
		elastic_maxpoint(0, 1, f_border, &extpoint);
		if (x == extpoint.y) {
			break;
		}
		else if (x > extpoint.y) {
			f_border += (CVE_ELASTIC_FREQ_MAX - 1) * std::pow(0.5, i + 2);
		}
		else {
			f_border -= (CVE_ELASTIC_FREQ_MAX - 1) * std::pow(0.5, i + 2);
		}
	}
	freq = f_border;
}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve_Elastic::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;

	Double_Point extpoint;
	elastic_maxpoint(0, 1, freq, &extpoint);


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
				(float)(to_client(0, (int)(func_elastic(0.0, 1.0, freq, to_graph(x, 0).x / (double)CVE_GRAPH_RESOLUTION) * CVE_GRAPH_RESOLUTION * 0.5)).y)),
				//100),
			D2D1::Point2F(
				x + CVE_DRAW_GRAPH_INCREASEMENT,
				(float)(to_client(0, (int)(func_elastic(0.0, 1.0, freq, to_graph(x + CVE_DRAW_GRAPH_INCREASEMENT, 0).x / (double)CVE_GRAPH_RESOLUTION) * CVE_GRAPH_RESOLUTION * 0.5)).y)),
				//100),
			bitmap_buffer->brush, CVE_CURVE_THICKNESS
		);
	}

	// ハンドル・ポイントを描画
	if (g_config.show_handle) {
		bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(handle_color)));

		draw_handle(
			bitmap_buffer,
			to_client((int)(extpoint.x * CVE_GRAPH_RESOLUTION), (int)(extpoint.y * CVE_GRAPH_RESOLUTION * 0.5)),
			to_client(0, (int)(extpoint.y * CVE_GRAPH_RESOLUTION * 0.5)),
			drawing_mode, false
		);

		draw_handle(
			bitmap_buffer,
			to_client((int)(extpoint.x * CVE_GRAPH_RESOLUTION), (int)(extpoint.y * CVE_GRAPH_RESOLUTION * 0.5)),
			to_client(CVE_GRAPH_RESOLUTION, (int)(extpoint.y * CVE_GRAPH_RESOLUTION * 0.5)),
			drawing_mode, false
		);

		// 始点
		draw_handle(
			bitmap_buffer,
			to_client(0, 0),
			to_client(0, 0),
			drawing_mode, true
		);
		// 終点
		draw_handle(
			bitmap_buffer,
			to_client(CVE_GRAPH_RESOLUTION, (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			to_client(CVE_GRAPH_RESOLUTION, (int)(CVE_GRAPH_RESOLUTION * 0.5)),
			drawing_mode, true
		);
	}
}



//---------------------------------------------------------------------
//		数値を生成
//---------------------------------------------------------------------
int cve::Curve_Elastic::create_number()
{
	return (int)(freq * CVE_ELASTIC_FREQ_COEF);
}



//---------------------------------------------------------------------
//		数値を読み取り
//---------------------------------------------------------------------
void cve::Curve_Elastic::read_number(int number)
{

	move_handle(number);
}



//---------------------------------------------------------------------
//		Luaに渡すやつ
//---------------------------------------------------------------------
double cve::Curve_Elastic::create_result(int number, double ratio, double st, double ed)
{
	return func_elastic(st, ed, MINMAX_LIMIT(number / (double)CVE_ELASTIC_FREQ_COEF, 1, CVE_ELASTIC_FREQ_MAX), ratio);
}