//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（描画関係の関数）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		Direct2Dを初期化
//---------------------------------------------------------------------
void d2d_initialize()
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
void d2d_setup(HDC hdc, LPRECT rect_wnd, COLORREF cr)
{
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BindDC(hdc, rect_wnd);
		g_render_target->BeginDraw();
		g_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
		g_render_target->Clear(D2D1::ColorF(cr));
		g_render_target->EndDraw();
	}
}


//---------------------------------------------------------------------
//		グラフのグリッドを描画
//---------------------------------------------------------------------
void d2d_draw_grid(ID2D1SolidColorBrush* pBrush, LPRECT rect_wnd) {
	pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_GRID)));
	int kx = std::floor(std::log(CE_GR_RESOLUTION * g_disp_info.scale.x / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int ky = std::floor(std::log(CE_GR_RESOLUTION * g_disp_info.scale.y / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int nx = std::pow(CE_GR_GRID_N, kx);
	int ny = std::pow(CE_GR_GRID_N, ky);
	double dx = (CE_GR_RESOLUTION * g_disp_info.scale.x) / nx;
	double dy = (CE_GR_RESOLUTION * g_disp_info.scale.y) / ny;
	int lx, ly;

	if (to_graph(0, 0).x >= 0)
		lx = std::floor(to_graph(0, 0).x * nx / (double)CE_GR_RESOLUTION);
	else
		lx = std::ceil(to_graph(0, 0).x * nx / (double)CE_GR_RESOLUTION);

	if (to_graph(0, 0).y >= 0)
		ly = std::floor(to_graph(0, 0).y * ny / (double)CE_GR_RESOLUTION);
	else
		ly = std::ceil(to_graph(0, 0).y * ny / (double)CE_GR_RESOLUTION);

	double ax = to_client(lx * CE_GR_RESOLUTION / (double)nx, 0).x;
	double ay = to_client(0, ly * CE_GR_RESOLUTION / (double)ny).y;
	float thickness;

	for (int i = 0; ax + dx * i <= rect_wnd->right; i++) {
		if ((lx + i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(ax + dx * i, 0),
			D2D1::Point2F(ax + dx * i, rect_wnd->bottom),
			pBrush, thickness, NULL
		);
	}

	for (int i = 0; ay + dy * i <= rect_wnd->bottom; i++) {
		if ((ly - i) % CE_GR_GRID_N == 0)
			thickness = CE_GR_GRID_TH_B;
		else
			thickness = CE_GR_GRID_TH_L;
		g_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F(rect_wnd->right, ay + dy * i),
			pBrush, thickness, NULL
		);
	}
}


//---------------------------------------------------------------------
//		ベジェカーブを描画
//---------------------------------------------------------------------
void d2d_draw_bezier(ID2D1SolidColorBrush* pBrush,
	DoublePoint stpt, DoublePoint ctpt1, DoublePoint ctpt2, DoublePoint edpt, float thickness)
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
		g_render_target->DrawGeometry(bezier, pBrush, thickness, pStyle);
}


//---------------------------------------------------------------------
//		グラフのハンドルを描画
//---------------------------------------------------------------------
void d2d_draw_handle(ID2D1SolidColorBrush* pBrush, DoublePoint st, DoublePoint ed)
{
	DoublePoint st_new = subtract_length(ed, st, CE_SUBTRACT_LENGTH);
	DoublePoint ed_new = subtract_length(st, ed, CE_SUBTRACT_LENGTH);
	g_render_target->DrawLine(
		D2D1::Point2F(st_new.x, st_new.y),
		D2D1::Point2F(ed_new.x, ed_new.y),
		pBrush, CE_HANDLE_TH
	);
	g_render_target->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(ed.x, ed.y),
			CE_HANDLE_SIZE, CE_HANDLE_SIZE),
		pBrush, CE_HANDLE_SIRCLE_LINE
	);
	g_render_target->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(st.x, st.y),
			CE_POINT_SIZE, CE_POINT_SIZE),
		pBrush, CE_POINT_SIZE * 1.7
	);
}


//---------------------------------------------------------------------
//		メインウィンドウを描画
//---------------------------------------------------------------------
void draw_main(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd, LPRECT rect_sepr)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;
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
	d2d_setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg_window));

	if (g_render_target != NULL) {
		g_render_target->BeginDraw();
		if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
		pBrush->SetColor(D2D1::ColorF(TO_BGR(BRIGHTEN(g_theme[g_config.theme].bg_window, CE_BR_SEPR))));

		if (pBrush) g_render_target->DrawLine(
			D2D1::Point2F((rect_sepr->right + rect_sepr->left) * 0.5 - CE_SEPR_LINE_L, rect_sepr->top + CE_SEPR_W),
			D2D1::Point2F((rect_sepr->right + rect_sepr->left) * 0.5 + CE_SEPR_LINE_L, rect_sepr->top + CE_SEPR_W),
			pBrush, CE_SEPR_LINE_W, pStyle
		);
		g_render_target->EndDraw();
	}

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		フッタパネルを描画
//---------------------------------------------------------------------
void draw_footer(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	d2d_setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		ライブラリを描画
//---------------------------------------------------------------------
void draw_panel_library(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	d2d_setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		エディタパネルを描画
//---------------------------------------------------------------------
void draw_panel_editor(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	d2d_setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}


//---------------------------------------------------------------------
//		グラフパネルを描画
//---------------------------------------------------------------------
void draw_panel_graph(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;
	ID2D1StrokeStyle* pStyle = NULL;
	float dashes[] = { CE_GR_POINT_DASH, CE_GR_POINT_DASH };

	DoublePoint ctpt_cl[] = {//クライアント
		{
			g_disp_info.o.x,
			g_disp_info.o.y
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
			g_disp_info.o.x + g_disp_info.scale.x * CE_GR_RESOLUTION,
			g_disp_info.o.y - g_disp_info.scale.y * CE_GR_RESOLUTION
		}
	};
	DoublePoint ctpt_trace_cl[] = {
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
		rect_wnd->bottom
	};

	D2D1_RECT_F rect_right = {
		ctpt_cl[3].x, 0,
		rect_wnd->right,
		rect_wnd->bottom,
	};

	D2D1_RECT_F rect_up = {
		ctpt_cl[0].x,
		0,
		ctpt_cl[3].x,
		to_client(0, 3.73 * CE_GR_RESOLUTION).y
	};

	D2D1_RECT_F rect_down = {
		ctpt_cl[0].x,
		to_client(0, -2.73 * CE_GR_RESOLUTION).y,
		ctpt_cl[3].x,
		rect_wnd->bottom
	};

	//Direct2D初期化
	d2d_setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg_graph));

	//描画
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BeginDraw();
		if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);

		//グリッド
		d2d_draw_grid(pBrush, rect_wnd);

		pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_GR_INVALID)));
		pBrush->SetOpacity(0.5);
		if (pBrush) {
			// Xが0未満1より大の部分を暗くする
			g_render_target->FillRectangle(&rect_left, pBrush);
			g_render_target->FillRectangle(&rect_right, pBrush);
			// Valueモードのとき
			if (g_config.mode == 0) {
				g_render_target->FillRectangle(&rect_up, pBrush);
				g_render_target->FillRectangle(&rect_down, pBrush);
			}
		}
		pBrush->SetOpacity(1);


		//値モードのとき
		if (g_config.mode == 0) {
			// ベジェ(トレース)
			if (g_config.trace) {
				pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_TRACE)));
				d2d_draw_bezier(pBrush, ctpt_trace_cl[0], ctpt_trace_cl[1], ctpt_trace_cl[2], ctpt_trace_cl[3], CE_CURVE_TH);
			}
			// ベジェ
			pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve)));
			d2d_draw_bezier(pBrush, ctpt_cl[0], ctpt_cl[1], ctpt_cl[2], ctpt_cl[3], CE_CURVE_TH);

			if (g_config.show_handle) {
				//ハンドルの色
				pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].handle)));
				//ハンドル（左）
				d2d_draw_handle(pBrush, ctpt_cl[0], ctpt_cl[1]);
				//ハンドル（右）
				d2d_draw_handle(pBrush, ctpt_cl[3], ctpt_cl[2]);
			}
		}

		//IDモードのとき
		else {
			for (int i = 0; i < g_curve_id[g_config.current_id].ctpts.size - 1; i++)
			{
				//色を指定
				pBrush->SetColor(D2D1::ColorF(TO_BGR(CONTRAST(INVERT(g_theme[g_config.theme].bg_graph), CE_GR_POINT_CONTRAST))));
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
						D2D1::Point2F(to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center).x, rect_wnd->bottom),
						pBrush, CE_GR_POINT_TH, pStyle
					);

				// ベジェ曲線(トレース)を描画
				if (g_config.trace) {
					pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].bg_graph), CE_BR_TRACE)));
					d2d_draw_bezier(pBrush,
						to_client(g_curve_id_previous.ctpts[i].pt_center),
						to_client(g_curve_id_previous.ctpts[i].pt_right),
						to_client(g_curve_id_previous.ctpts[i + 1].pt_left),
						to_client(g_curve_id_previous.ctpts[i + 1].pt_center),
						CE_CURVE_TH
					);
				}

				// ベジェ曲線を描画
				pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve)));
				d2d_draw_bezier(pBrush,
					to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center),
					to_client(g_curve_id[g_config.current_id].ctpts[i].pt_right),
					to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_left),
					to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_center),
					CE_CURVE_TH
				);

				//ハンドルの描画
				if (g_config.show_handle) {
					pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].handle)));
					d2d_draw_handle(pBrush,
						to_client(g_curve_id[g_config.current_id].ctpts[i].pt_center),
						to_client(g_curve_id[g_config.current_id].ctpts[i].pt_right)
					);
					d2d_draw_handle(pBrush,
						to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_center),
						to_client(g_curve_id[g_config.current_id].ctpts[i + 1].pt_left)
					);
				}
			}
		}

		g_render_target->EndDraw();
	}


	//ビットマップをバッファから画面に転送
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect_wnd->right, rect_wnd->bottom, hdc_mem, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}