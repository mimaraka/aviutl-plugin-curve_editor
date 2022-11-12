//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (振動モードのカーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		カーブ(振動)
//---------------------------------------------------------------------
double cve::Curve_Elastic::func_elastic(double st, double ed, double freq, double t)
{
	return (st - ed) * std::pow(CVE_ELASTIC_DECAY, t) * std::cos(CVE_MATH_PI * freq * t) + ed;
}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve_Elastic::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;

	const bool is_preset = drawing_mode == CVE_DRAW_CURVE_PRESET;


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



	if (right_side < 0 || left_side > rect_wnd.right) {
		return;
	}
	else if (left_side < 0) {
		left_side = 0;
	}
	else if (right_side > rect_wnd.right) {
		right_side = rect_wnd.right;
	}

	bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(curve_color)));

	for (float x = left_side; x > right_side; x += CVE_DRAW_GRAPH_INCREASEMENT) {
		g_render_target->DrawLine(
			D2D1::Point2F(
				x,
				(float)func_elastic(0.0, 1.0, 4.0, to_graph(x, 0).x / (double)CVE_GRAPH_RESOLUTION)),
			D2D1::Point2F(
				x + CVE_DRAW_GRAPH_INCREASEMENT,
				(float)func_elastic(0.0, 1.0, 4.0, (to_graph(x, 0).x + CVE_DRAW_GRAPH_INCREASEMENT) / (double)CVE_GRAPH_RESOLUTION)),
			bitmap_buffer->brush, CVE_CURVE_THICKNESS
		);
	}
	
}