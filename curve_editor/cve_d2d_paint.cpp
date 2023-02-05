//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（Direct2D描画）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_GR_GRID_TH_L				0.5f
#define CVE_GR_GRID_TH_B				1.f
#define CVE_GR_GRID_MIN					36



//---------------------------------------------------------------------
//		グラフのグリッドを描画
//---------------------------------------------------------------------
void cve::My_Direct2d_Paint_Object::draw_grid()
{
	aului::Color col_grid = g_theme[g_config.theme].bg_graph;
	col_grid.change_brightness(CVE_BR_GRID);
	brush->SetColor(D2D1::ColorF(col_grid.d2dcolor()));
	// 
	int kx = (int)std::floor(std::log(CVE_GRAPH_RESOLUTION * g_view_info.scale.x / (double)CVE_GR_GRID_MIN) / std::log(CVE_GRAPH_GRID_NUM));
	int ky = (int)std::floor(std::log(CVE_GRAPH_RESOLUTION * g_view_info.scale.y / (double)CVE_GR_GRID_MIN) / std::log(CVE_GRAPH_GRID_NUM));
	// グラフの枠内に表示されるグリッドの本数
	int nx = std::max((int)std::pow(CVE_GRAPH_GRID_NUM, kx), 1);
	int ny = std::max((int)std::pow(CVE_GRAPH_GRID_NUM, ky), 1);
	// 
	float dx = (float)(CVE_GRAPH_RESOLUTION * g_view_info.scale.x) / (float)nx;
	float dy = (float)(CVE_GRAPH_RESOLUTION * g_view_info.scale.y) / (float)ny;
	int lx, ly;

	if (to_graph(0, 0).x >= 0)
		lx = (int)std::floor(to_graph(0, 0).x * nx / (double)CVE_GRAPH_RESOLUTION);
	else
		lx = (int)std::ceil(to_graph(0, 0).x * nx / (double)CVE_GRAPH_RESOLUTION);

	if (to_graph(0, 0).y >= 0)
		ly = (int)std::floor(to_graph(0, 0).y * ny / (double)CVE_GRAPH_RESOLUTION);
	else
		ly = (int)std::ceil(to_graph(0, 0).y * ny / (double)CVE_GRAPH_RESOLUTION);

	float ax = to_client(lx * CVE_GRAPH_RESOLUTION / (float)nx, 0.f).x;
	float ay = to_client(0.f, ly * CVE_GRAPH_RESOLUTION / (float)ny).y;
	float thickness;

	// 縦方向
	for (int i = 0; ax + dx * i <= rect.right; i++) {
		if ((lx + i) % CVE_GRAPH_GRID_NUM == 0)
			thickness = CVE_GR_GRID_TH_B;
		else
			thickness = CVE_GR_GRID_TH_L;
		p_render_target->DrawLine(
			D2D1::Point2F(ax + dx * i, 0),
			D2D1::Point2F(ax + dx * i, (float)rect.bottom),
			brush, thickness, NULL
		);
	}

	// 横方向
	for (int i = 0; ay + dy * i <= rect.bottom; i++) {
		if ((ly - i) % CVE_GRAPH_GRID_NUM == 0)
			thickness = CVE_GR_GRID_TH_B;
		else
			thickness = CVE_GR_GRID_TH_L;

		p_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F((float)rect.right, ay + dy * i),
			brush, thickness, NULL
		);
	}
}



//---------------------------------------------------------------------
//		ラウンドエッジを描画
//---------------------------------------------------------------------
void cve::My_Direct2d_Paint_Object::draw_rounded_edge(int flag, float radius) {
	ID2D1GeometrySink* sink = nullptr;
	ID2D1PathGeometry* edge = nullptr;
	D2D1_POINT_2F pt_1, pt_2, pt_3;
	aului::Color col_edge = g_theme[g_config.theme].bg;

	D2D1_POINT_2F pts_1[] = {
		D2D1::Point2F((float)rect.left, (float)rect.top),
		D2D1::Point2F((float)rect.left, (float)rect.bottom),
		D2D1::Point2F((float)rect.right, (float)rect.top),
		D2D1::Point2F((float)rect.right, (float)rect.bottom)
	};

	D2D1_POINT_2F pts_2[] = {
		D2D1::Point2F((float)rect.left, (float)rect.top + radius),
		D2D1::Point2F((float)rect.left + radius, (float)rect.bottom),
		D2D1::Point2F((float)rect.right - radius, (float)rect.top),
		D2D1::Point2F((float)rect.right, (float)rect.bottom - radius)
	};

	D2D1_POINT_2F pts_3[] = {
		D2D1::Point2F((float)rect.left + radius, (float)rect.top),
		D2D1::Point2F((float)rect.left, (float)rect.bottom - radius),
		D2D1::Point2F((float)rect.right, (float)rect.top + radius),
		D2D1::Point2F((float)rect.right - radius, (float)rect.bottom)
	};

	p_factory->CreatePathGeometry(&edge);
	edge->Open(&sink);

	for (int i = 0; i < 4; i++) {
		if (flag & 1 << i) {
			pt_1 = pts_1[i];
			pt_2 = pts_2[i];
			pt_3 = pts_3[i];

			sink->BeginFigure(pt_1, D2D1_FIGURE_BEGIN_FILLED);
			sink->AddLine(pt_2);
			sink->AddArc(
				D2D1::ArcSegment(
					pt_3,
					D2D1::SizeF(radius, radius),
					0.f,
					D2D1_SWEEP_DIRECTION_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);
			sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
	}

	sink->Close();
	brush->SetColor(D2D1::ColorF(col_edge.d2dcolor()));
	if (is_safe(&edge))
		p_render_target->FillGeometry(edge, brush, NULL);

	release(&sink);
	release(&edge);
}



//---------------------------------------------------------------------
//		メインウィンドウを描画
//---------------------------------------------------------------------
void cve::My_Direct2d_Paint_Object::draw_panel_main(const RECT& rect_sepr)
{
	ID2D1StrokeStyle* style = nullptr;

	p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_SOLID,
			0.f),
		NULL, NULL,
		&style
	);

	const D2D1_POINT_2F line_start = {
		g_config.layout_mode == cve::Config::Vertical ?
			(rect_sepr.right + rect_sepr.left) * 0.5f - CVE_SEPARATOR_LINE_LENGTH :
			(float)(rect_sepr.left + CVE_SEPARATOR_WIDTH),
		g_config.layout_mode == cve::Config::Vertical ?
			(float)(rect_sepr.top + CVE_SEPARATOR_WIDTH) :
			(rect_sepr.top + rect_sepr.bottom) * 0.5f - CVE_SEPARATOR_LINE_LENGTH
	};

	const D2D1_POINT_2F line_end = {
		g_config.layout_mode == cve::Config::Vertical ?
			(rect_sepr.right + rect_sepr.left) * 0.5f + CVE_SEPARATOR_LINE_LENGTH :
			(float)(rect_sepr.left + CVE_SEPARATOR_WIDTH),
		g_config.layout_mode == cve::Config::Vertical ?
			(float)(rect_sepr.top + CVE_SEPARATOR_WIDTH) :
			(rect_sepr.top + rect_sepr.bottom) * 0.5f + CVE_SEPARATOR_LINE_LENGTH
	};

	if (p_render_target != nullptr) {
		p_render_target->BeginDraw();
		// 背景を描画
		p_render_target->Clear(D2D1::ColorF(g_theme[g_config.theme].bg.d2dcolor()));

		brush->SetColor(D2D1::ColorF(g_theme[g_config.theme].sepr.d2dcolor()));

		if (brush)
			p_render_target->DrawLine(
				line_start,
				line_end,
				brush, CVE_SEPARATOR_LINE_WIDTH, style
			);

		p_render_target->EndDraw();
	}
	release(&style);
}



//---------------------------------------------------------------------
//		パネルを描画
//---------------------------------------------------------------------
void cve::My_Direct2d_Paint_Object::draw_panel()
{
	//Direct2D初期化
	d2d_setup(g_theme[g_config.theme].bg);
}



//---------------------------------------------------------------------
//		エディタパネルを描画
//---------------------------------------------------------------------
void cve::My_Direct2d_Paint_Object::draw_panel_editor()
{
	aului::Color col_overlay = g_theme[g_config.theme].bg_graph;
	D2D1_RECT_F rect_left = {
		0.f,
		0.f,
		(float)g_view_info.origin.x,
		(float)rect.bottom
	};

	D2D1_RECT_F rect_right = {
		(float)(g_view_info.origin.x + g_view_info.scale.x * CVE_GRAPH_RESOLUTION),
		0.f,
		(float)rect.right,
		(float)rect.bottom,
	};

	D2D1_RECT_F rect_up = {
		(float)g_view_info.origin.x,
		0.f,
		(float)(g_view_info.origin.x + g_view_info.scale.x * CVE_GRAPH_RESOLUTION),
		to_client(0.f, Curve_Bezier::MAX_Y * CVE_GRAPH_RESOLUTION).y
	};

	D2D1_RECT_F rect_down = {
		(float)g_view_info.origin.x,
		to_client(0.f, Curve_Bezier::MIN_Y * CVE_GRAPH_RESOLUTION).y,
		(float)(g_view_info.origin.x + g_view_info.scale.x * CVE_GRAPH_RESOLUTION),
		(float)rect.bottom
	};


	//描画
	if (is_safe(&p_render_target)) {
		p_render_target->BeginDraw();
		// 背景を描画
		p_render_target->Clear(D2D1::ColorF(g_theme[g_config.theme].bg_graph.d2dcolor()));

		//グリッド
		draw_grid();

		col_overlay.change_brightness(CVE_BR_GR_INVALID);
		brush->SetColor(D2D1::ColorF(col_overlay.d2dcolor()));
		brush->SetOpacity(0.5f);
		if (brush) {
			// Xが0未満1より大の部分を暗くする
			p_render_target->FillRectangle(&rect_left, brush);
			p_render_target->FillRectangle(&rect_right, brush);
			// ベジェモードのとき
			if (g_config.edit_mode == cve::Mode_Bezier) {
				p_render_target->FillRectangle(&rect_up, brush);
				p_render_target->FillRectangle(&rect_down, brush);
			}
		}
		brush->SetOpacity(1);

		// 編集モード振り分け
		switch (g_config.edit_mode) {
			// ベジェモードのとき
		case Mode_Bezier:
			if (g_config.trace)
				g_curve_bezier_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bezier.draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// ベジェ(複数)モードのとき
		case Mode_Bezier_Multi:
			if (g_config.trace)
				g_curve_bezier_multi_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bezier_multi[g_config.current_id.multi - 1].draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// 値指定モードのとき
		case Mode_Bezier_Value:
			if (g_config.trace)
				g_curve_bezier_value_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bezier_value[g_config.current_id.value].draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// 振動モードのとき
		case Mode_Elastic:
			if (g_config.trace)
				g_curve_elastic_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_elastic.draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;

			// バウンスモードのとき
		case Mode_Bounce:
			if (g_config.trace)
				g_curve_bounce_trace.draw_curve(this, rect, Curve::DRAW_CURVE_TRACE);

			g_curve_bounce.draw_curve(this, rect, Curve::DRAW_CURVE_NORMAL);
			break;
		}

		p_render_target->EndDraw();
	}
}