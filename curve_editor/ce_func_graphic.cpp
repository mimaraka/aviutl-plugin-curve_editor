//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（描画関係の関数）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"

#define CE_GR_GRID_TH_L				0.5f
#define CE_GR_GRID_TH_B				1.0f
#define CE_GR_GRID_MIN				36



//---------------------------------------------------------------------
//		Direct2Dを初期化
//---------------------------------------------------------------------
void ce::d2d_init()
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d1_factory);
	D2D1_RENDER_TARGET_PROPERTIES prop;
	prop = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
		0, 0,
		D2D1_RENDER_TARGET_USAGE_NONE,
		D2D1_FEATURE_LEVEL_DEFAULT
	);
	g_d2d1_factory->CreateDCRenderTarget(&prop, &g_render_target);
}



//---------------------------------------------------------------------
//		グラフのグリッドを描画
//---------------------------------------------------------------------
void d2d_draw_grid(ID2D1SolidColorBrush* brush, const RECT& rect_wnd) {
	brush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_GRID)));
	// 
	int kx = (int)std::floor(std::log(CE_GR_RESOLUTION * g_view_info.scale.x / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int ky = (int)std::floor(std::log(CE_GR_RESOLUTION * g_view_info.scale.y / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	// グラフの枠内に表示されるグリッドの本数
	int nx = MINLIM((int)std::pow(CE_GR_GRID_N, kx), 1);
	int ny = MINLIM((int)std::pow(CE_GR_GRID_N, ky), 1);
	// 
	float dx = (float)(CE_GR_RESOLUTION * g_view_info.scale.x) / (float)nx;
	float dy = (float)(CE_GR_RESOLUTION * g_view_info.scale.y) / (float)ny;
	int lx, ly;

	if (to_graph(0, 0).x >= 0)
		lx = (int)std::floor(to_graph(0, 0).x * nx / (double)CE_GR_RESOLUTION);
	else
		lx = (int)std::ceil(to_graph(0, 0).x * nx / (double)CE_GR_RESOLUTION);

	if (to_graph(0, 0).y >= 0)
		ly = (int)std::floor(to_graph(0, 0).y * ny / (double)CE_GR_RESOLUTION);
	else
		ly = (int)std::ceil(to_graph(0, 0).y * ny / (double)CE_GR_RESOLUTION);

	float ax = to_client((int)(lx * CE_GR_RESOLUTION / (float)nx), 0).x;
	float ay = to_client(0, (int)(ly * CE_GR_RESOLUTION / (float)ny)).y;
	float thickness;

	// 縦方向
	for (int i = 0; ax + dx * i <= rect_wnd.right; i++) {
		if ((lx + i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(ax + dx * i, 0),
			D2D1::Point2F(ax + dx * i, (float)rect_wnd.bottom),
			brush, thickness, NULL
		);
	}

	// 横方向
	for (int i = 0; ay + dy * i <= rect_wnd.bottom; i++) {
		if ((ly - i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F((float)rect_wnd.right, ay + dy * i),
			brush, thickness, NULL
		);
	}
}



//---------------------------------------------------------------------
//		ラウンドエッジを描画
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::draw_rounded_edge(int flag, float radius) {
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* edge;
	D2D1_POINT_2F pt_1, pt_2, pt_3;

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

	g_d2d1_factory->CreatePathGeometry(&edge);
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
					0.0f,
					D2D1_SWEEP_DIRECTION_CLOCKWISE,
					D2D1_ARC_SIZE_SMALL
				)
			);
			sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		}
	}
	
	sink->Close();
	brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].bg)));
	if (edge)
		g_render_target->FillGeometry(edge, brush, NULL);
}



//---------------------------------------------------------------------
//		メインウィンドウを描画
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::draw_panel_main(const RECT& rect_sepr)
{
	ID2D1StrokeStyle* style = nullptr;

	g_d2d1_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		NULL, NULL,
		&style
	);

	//Direct2D初期化
	d2d_setup(TO_BGR(g_theme[g_config.theme].bg));

	if (g_render_target != nullptr) {
		g_render_target->BeginDraw();

		brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].sepr)));

		if (brush) g_render_target->DrawLine(
			D2D1::Point2F((rect_sepr.right + rect_sepr.left) * 0.5f - CE_SEPR_LINE_L, (float)(rect_sepr.top + CE_SEPR_W)),
			D2D1::Point2F((rect_sepr.right + rect_sepr.left) * 0.5f + CE_SEPR_LINE_L, (float)(rect_sepr.top + CE_SEPR_W)),
			brush, CE_SEPR_LINE_W, style
		);

		// brush->Release();

		g_render_target->EndDraw();
	}

	//ビットマップをバッファから画面に転送
	transfer();
}



//---------------------------------------------------------------------
//		ヘッダパネルを描画
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::draw_panel_header()
{
	//Direct2D初期化
	d2d_setup(TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	transfer();
}



//---------------------------------------------------------------------
//		プリセットパネルを描画
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::draw_panel_preset()
{
	//Direct2D初期化
	d2d_setup(TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	transfer();
}



//---------------------------------------------------------------------
//		エディタパネルを描画
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::draw_panel_editor()
{
	Curve* curve_ptr;
	Curve* curve_trace_ptr;

	D2D1_RECT_F rect_left = {
		0,
		0,
		g_view_info.origin.x,
		(float)rect.bottom
	};

	D2D1_RECT_F rect_right = {
		g_view_info.origin.x + (float)(g_view_info.scale.x * CE_GR_RESOLUTION),
		0,
		(float)rect.right,
		(float)rect.bottom,
	};

	D2D1_RECT_F rect_up = {
		g_view_info.origin.x,
		0,
		g_view_info.origin.x + (float)(g_view_info.scale.x * CE_GR_RESOLUTION),
		to_client(0, (int)(CE_CURVE_VALUE_MAX_Y * CE_GR_RESOLUTION)).y
	};

	D2D1_RECT_F rect_down = {
		g_view_info.origin.x,
		to_client(0, (int)(CE_CURVE_VALUE_MIN_Y * CE_GR_RESOLUTION)).y,
		g_view_info.origin.x + (float)(g_view_info.scale.x * CE_GR_RESOLUTION),
		(float)rect.bottom
	};


	// Valueモードのとき
	if (g_config.mode == Mode_Value) {
		curve_ptr = &g_curve_value;
		curve_trace_ptr = &g_curve_value_previous;
	}
	// IDモードのとき
	else {
		curve_ptr = &g_curve_id[g_config.current_id];
		curve_trace_ptr = &g_curve_id_previous;
	}

	//Direct2D初期化
	d2d_setup(TO_BGR(g_theme[g_config.theme].bg_graph));

	//描画
	if (g_render_target != nullptr && g_d2d1_factory != nullptr) {
		g_render_target->BeginDraw();

		//グリッド
		d2d_draw_grid(brush, rect);

		brush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_GR_INVALID)));
		brush->SetOpacity(0.5f);
		if (brush) {
			// Xが0未満1より大の部分を暗くする
			g_render_target->FillRectangle(&rect_left, brush);
			g_render_target->FillRectangle(&rect_right, brush);
			// Valueモードのとき
			if (g_config.mode == ce::Mode_Value) {
				g_render_target->FillRectangle(&rect_up, brush);
				g_render_target->FillRectangle(&rect_down, brush);
			}
		}
		brush->SetOpacity(1);

		curve_trace_ptr->draw_curve(brush, rect, CE_DRAW_CURVE_TRACE);
		curve_ptr->draw_curve(brush, rect, CE_DRAW_CURVE_REGULAR);

		g_render_target->EndDraw();
	}


	//ビットマップをバッファから画面に転送
	transfer();
}