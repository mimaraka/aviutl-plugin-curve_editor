//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（描画関係の関数）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		Direct2Dを初期化
//---------------------------------------------------------------------
void d2d_init()
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
//		Direct2Dの描画の準備
//---------------------------------------------------------------------
void d2d_setup(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd, COLORREF cr)
{
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BindDC(canvas->hdc_memory, rect_wnd);
		g_render_target->BeginDraw();
		g_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
		g_render_target->Clear(D2D1::ColorF(cr));
		g_render_target->EndDraw();
	}
}



//---------------------------------------------------------------------
//		グラフのグリッドを描画
//---------------------------------------------------------------------
void d2d_draw_grid(ID2D1SolidColorBrush* brush, LPRECT rect_wnd) {
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
	for (int i = 0; ax + dx * i <= rect_wnd->right; i++) {
		if ((lx + i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(ax + dx * i, 0),
			D2D1::Point2F(ax + dx * i, (float)rect_wnd->bottom),
			brush, thickness, NULL
		);
	}

	// 横方向
	for (int i = 0; ay + dy * i <= rect_wnd->bottom; i++) {
		if ((ly - i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F((float)rect_wnd->right, ay + dy * i),
			brush, thickness, NULL
		);
	}
}



//---------------------------------------------------------------------
//		ベジェカーブを描画
//---------------------------------------------------------------------
void d2d_draw_bezier(ID2D1SolidColorBrush* brush,
	ce::Float_Point stpt, ce::Float_Point ctpt1, ce::Float_Point ctpt2, ce::Float_Point edpt, float thickness)
{
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* bezier;
	ID2D1StrokeStyle* pStyle = NULL;
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
		&pStyle
	);
	g_d2d1_factory->CreatePathGeometry(&bezier);
	bezier->Open(&sink);
	sink->BeginFigure(D2D1::Point2F(stpt.x, stpt.y), D2D1_FIGURE_BEGIN_HOLLOW);
	sink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(ctpt1.x, ctpt1.y),
		D2D1::Point2F(ctpt2.x, ctpt2.y),
		D2D1::Point2F(edpt.x, edpt.y)
	));
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	if (bezier)
		g_render_target->DrawGeometry(bezier, brush, thickness, pStyle);
}



//---------------------------------------------------------------------
//		グラフのハンドルを描画
//---------------------------------------------------------------------
void d2d_draw_handle(ID2D1SolidColorBrush* brush, ce::Float_Point st, ce::Float_Point ed)
{
	ce::Float_Point st_new = subtract_length(ed, st, CE_SUBTRACT_LENGTH_2);
	ce::Float_Point ed_new = subtract_length(st, ed, CE_SUBTRACT_LENGTH);

	ID2D1StrokeStyle* pStyle = NULL;

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
		&pStyle
	);

	g_render_target->DrawLine(
		D2D1::Point2F(st_new.x, st_new.y),
		D2D1::Point2F(ed_new.x, ed_new.y),
		brush, CE_HANDLE_TH
	);

	g_render_target->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(ed.x, ed.y),
			(float)CE_HANDLE_SIZE, (float)CE_HANDLE_SIZE),
		brush, (float)CE_HANDLE_SIRCLE_LINE
	);

	g_render_target->FillEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(st.x, st.y),
			(float)CE_POINT_SIZE, (float)CE_POINT_SIZE),
			brush
	);
}



//---------------------------------------------------------------------
//		ラウンドエッジを描画
//---------------------------------------------------------------------
void d2d_draw_rounded_edge(ID2D1SolidColorBrush* brush, LPRECT rect_wnd, int flag, float radius) {
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* edge;
	D2D1_POINT_2F pt_1, pt_2, pt_3;

	D2D1_POINT_2F pts_1[] = {
		D2D1::Point2F((float)rect_wnd->left, (float)rect_wnd->top),
		D2D1::Point2F((float)rect_wnd->left, (float)rect_wnd->bottom),
		D2D1::Point2F((float)rect_wnd->right, (float)rect_wnd->top),
		D2D1::Point2F((float)rect_wnd->right, (float)rect_wnd->bottom)
	};

	D2D1_POINT_2F pts_2[] = {
		D2D1::Point2F((float)rect_wnd->left, (float)rect_wnd->top + radius),
		D2D1::Point2F((float)rect_wnd->left + radius, (float)rect_wnd->bottom),
		D2D1::Point2F((float)rect_wnd->right - radius, (float)rect_wnd->top),
		D2D1::Point2F((float)rect_wnd->right, (float)rect_wnd->bottom - radius)
	};

	D2D1_POINT_2F pts_3[] = {
		D2D1::Point2F((float)rect_wnd->left + radius, (float)rect_wnd->top),
		D2D1::Point2F((float)rect_wnd->left, (float)rect_wnd->bottom - radius),
		D2D1::Point2F((float)rect_wnd->right, (float)rect_wnd->top + radius),
		D2D1::Point2F((float)rect_wnd->right - radius, (float)rect_wnd->bottom)
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
void draw_panel_main(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd, LPRECT rect_sepr)
{
	static ID2D1SolidColorBrush* brush = NULL;
	ID2D1StrokeStyle* pStyle = NULL;

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
		&pStyle
	);

	//Direct2D初期化
	d2d_setup(canvas, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

	if (g_render_target != NULL) {
		g_render_target->BeginDraw();
		if (brush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);
		brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].sepr)));

		if (brush) g_render_target->DrawLine(
			D2D1::Point2F((rect_sepr->right + rect_sepr->left) * 0.5f - CE_SEPR_LINE_L, (float)(rect_sepr->top + CE_SEPR_W)),
			D2D1::Point2F((rect_sepr->right + rect_sepr->left) * 0.5f + CE_SEPR_LINE_L, (float)(rect_sepr->top + CE_SEPR_W)),
			brush, CE_SEPR_LINE_W, pStyle
		);
		g_render_target->EndDraw();
	}

	//ビットマップをバッファから画面に転送
	canvas->transfer(rect_wnd);
}



//---------------------------------------------------------------------
//		ヘッダパネルを描画
//---------------------------------------------------------------------
void draw_panel_header(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd)
{
	static ID2D1SolidColorBrush* brush = NULL;

	//Direct2D初期化
	d2d_setup(canvas, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	canvas->transfer(rect_wnd);
}



//---------------------------------------------------------------------
//		ライブラリを描画
//---------------------------------------------------------------------
void draw_panel_preset(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd)
{
	static ID2D1SolidColorBrush* brush = NULL;

	//Direct2D初期化
	d2d_setup(canvas, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	canvas->transfer(rect_wnd);
}



//---------------------------------------------------------------------
//		エディタパネルを描画
//---------------------------------------------------------------------
void draw_panel_editor(ce::Bitmap_Canvas* canvas, LPRECT rect_wnd)
{
	static ID2D1SolidColorBrush* brush = NULL;
	ID2D1StrokeStyle* pStyle = NULL;
	float dashes[] = { CE_GR_POINT_DASH, CE_GR_POINT_DASH };

	ce::Float_Point ctpt_cl[] = {//クライアント
		{
			g_view_info.origin.x,
			g_view_info.origin.y
		},
		{
			to_client(g_curve_value.ctpt[0]).x,
			to_client(g_curve_value.ctpt[0]).y
		},
		{
			to_client(g_curve_value.ctpt[1]).x,
			to_client(g_curve_value.ctpt[1]).y
		},
		{
			g_view_info.origin.x + (float)(g_view_info.scale.x * CE_GR_RESOLUTION),
			g_view_info.origin.y - (float)(g_view_info.scale.y * CE_GR_RESOLUTION)
		}
	};
	ce::Float_Point ctpt_trace_cl[] = {
		ctpt_cl[0],
		{
			to_client(g_curve_value_previous.ctpt[0]).x,
			to_client(g_curve_value_previous.ctpt[0]).y
		},
		{
			to_client(g_curve_value_previous.ctpt[1]).x,
			to_client(g_curve_value_previous.ctpt[1]).y
		},
		ctpt_cl[3]
	};

	D2D1_RECT_F rect_left = {
		0, 0,
		ctpt_cl[0].x,
		(float)rect_wnd->bottom
	};

	D2D1_RECT_F rect_right = {
		ctpt_cl[3].x, 0,
		(float)rect_wnd->right,
		(float)rect_wnd->bottom,
	};

	D2D1_RECT_F rect_up = {
		ctpt_cl[0].x,
		0,
		ctpt_cl[3].x,
		to_client(0, (int)(3.73 * CE_GR_RESOLUTION)).y
	};

	D2D1_RECT_F rect_down = {
		ctpt_cl[0].x,
		to_client(0, (int)(-2.73 * CE_GR_RESOLUTION)).y,
		ctpt_cl[3].x,
		(float)rect_wnd->bottom
	};

	//Direct2D初期化
	d2d_setup(canvas, rect_wnd, TO_BGR(g_theme[g_config.theme].bg_graph));

	//描画
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BeginDraw();
		if (brush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);

		//グリッド
		d2d_draw_grid(brush, rect_wnd);

		brush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_GR_INVALID)));
		brush->SetOpacity(0.5);
		if (brush) {
			// Xが0未満1より大の部分を暗くする
			g_render_target->FillRectangle(&rect_left, brush);
			g_render_target->FillRectangle(&rect_right, brush);
			// Valueモードのとき
			if (g_config.mode == 0) {
				g_render_target->FillRectangle(&rect_up, brush);
				g_render_target->FillRectangle(&rect_down, brush);
			}
		}
		brush->SetOpacity(1);


		//値モードのとき
		if (g_config.mode == 0) {
			// ベジェ(トレース)
			if (g_config.trace) {
				brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve_trace)));
				d2d_draw_bezier(brush, ctpt_trace_cl[0], ctpt_trace_cl[1], ctpt_trace_cl[2], ctpt_trace_cl[3], CE_CURVE_TH);
			}
			// ベジェ
			brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve)));
			d2d_draw_bezier(brush, ctpt_cl[0], ctpt_cl[1], ctpt_cl[2], ctpt_cl[3], CE_CURVE_TH);

			if (g_config.show_handle) {
				//ハンドルの色
				brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].handle)));
				//ハンドル（左）
				d2d_draw_handle(brush, ctpt_cl[0], ctpt_cl[1]);
				//ハンドル（右）
				d2d_draw_handle(brush, ctpt_cl[3], ctpt_cl[2]);
			}
		}

		//IDモードのとき
		else {
			// ベジェ曲線(トレース)を描画
			if (g_config.trace) {
				for (int i = 0; i < (int)g_curve_id_previous.ctpts.size - 1; i++) {
					brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve_trace)));
					d2d_draw_bezier(brush,
						to_client(g_curve_id_previous.ctpts[i].pt_center),
						to_client(g_curve_id_previous.ctpts[i].pt_right),
						to_client(g_curve_id_previous.ctpts[i + 1].pt_left),
						to_client(g_curve_id_previous.ctpts[i + 1].pt_center),
						CE_CURVE_TH
					);
				}
			}

			for (int i = 0; i < (int)g_curve_id[g_config.current_id].ctpts.size - 1; i++)
			{
				//色を指定
				brush->SetColor(D2D1::ColorF(TO_BGR(CONTRAST(INVERT(g_theme[g_config.theme].bg_graph), CE_GR_POINT_CONTRAST))));
				g_d2d1_factory->CreateStrokeStyle(
					D2D1::StrokeStyleProperties(
						D2D1_CAP_STYLE_FLAT,
						D2D1_CAP_STYLE_FLAT,
						D2D1_CAP_STYLE_ROUND,
						D2D1_LINE_JOIN_MITER,
						10.0f,
						D2D1_DASH_STYLE_CUSTOM,
						0.0f),
					dashes,
					ARRAYSIZE(dashes),
					&pStyle
				);

				// 端点以外の制御点に引かれる点線
				if (i > 0)
					g_render_target->DrawLine(
						D2D1::Point2F(to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center).x, 0),
						D2D1::Point2F(to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center).x, (float)rect_wnd->bottom),
						brush, CE_GR_POINT_TH, pStyle
					);


				// ベジェ曲線を描画
				brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve)));
				d2d_draw_bezier(brush,
					to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center),
					to_client(g_curve_id[g_config.current_id].ctpts[i].pt_right),
					to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_left),
					to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_center),
					CE_CURVE_TH
				);

				//ハンドルの描画
				if (g_config.show_handle) {
					brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].handle)));
					d2d_draw_handle(brush,
						to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center),
						to_client(g_curve_id[g_config.current_id].ctpts[i].pt_right)
					);
					d2d_draw_handle(brush,
						to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_center),
						to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_left)
					);
				}
			}
		}

		g_render_target->EndDraw();
	}


	//ビットマップをバッファから画面に転送
	canvas->transfer(rect_wnd);
}