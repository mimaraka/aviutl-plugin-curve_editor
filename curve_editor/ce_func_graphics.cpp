//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（描画関係の関数）
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		Direct2Dを初期化
//---------------------------------------------------------------------
void D2D1_Init()
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
void D2D1_Setup(HDC hdc, LPRECT rect_wnd, COLORREF cr)
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
//		グラフのグリッドを描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawGrid(ID2D1SolidColorBrush* pBrush, LPRECT rect_wnd) {
	pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].gr_bg), CE_BR_GRID)));
	int kx = std::floor(std::log(CE_GR_RES * g_disp_info.scale.x / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int ky = std::floor(std::log(CE_GR_RES * g_disp_info.scale.y / (double)CE_GR_GRID_MIN) / std::log(CE_GR_GRID_N));
	int nx = std::pow(CE_GR_GRID_N, kx);
	int ny = std::pow(CE_GR_GRID_N, ky);
	double dx = (CE_GR_RES * g_disp_info.scale.x) / nx;
	double dy = (CE_GR_RES * g_disp_info.scale.y) / ny;
	int lx, ly;

	if (ToGraph(0, 0).x >= 0)
		lx = std::floor(ToGraph(0, 0).x * nx / (double)CE_GR_RES);
	else
		lx = std::ceil(ToGraph(0, 0).x * nx / (double)CE_GR_RES);

	if (ToGraph(0, 0).y >= 0)
		ly = std::floor(ToGraph(0, 0).y * ny / (double)CE_GR_RES);
	else
		ly = std::ceil(ToGraph(0, 0).y * ny / (double)CE_GR_RES);

	double ax = ToClient(lx * CE_GR_RES / (double)nx, 0).x;
	double ay = ToClient(0, ly * CE_GR_RES / (double)ny).y;
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
//		ベジェカーブを描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawBezier(ID2D1SolidColorBrush* pBrush,
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
//		グラフのハンドルを描画(Direct2D)
//---------------------------------------------------------------------
void D2D1_DrawHandle(ID2D1SolidColorBrush* pBrush, DoublePoint stpt, DoublePoint edpt)
{
	g_render_target->DrawLine(
		D2D1::Point2F(stpt.x, stpt.y),
		D2D1::Point2F(edpt.x, edpt.y),
		pBrush, CE_HANDLE_TH
	);
	g_render_target->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(edpt.x, edpt.y),
			CE_HANDLE_SIZE, CE_HANDLE_SIZE),
		pBrush, CE_HANDLE_SIRCLE_LINE
	);
	g_render_target->DrawEllipse(
		D2D1::Ellipse(
			D2D1::Point2F(stpt.x, stpt.y),
			CE_POINT_SIZE, CE_POINT_SIZE),
		pBrush, CE_POINT_SIZE * 1.5
	);
}


//---------------------------------------------------------------------
//		メインウィンドウを描画
//---------------------------------------------------------------------
void DrawMain(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	RECT rect_sepr;
	static ID2D1SolidColorBrush* pBrush = NULL;

	rect_sepr = {
	g_config.separator - CE_SEPR_W,
	0,
	g_config.separator + CE_SEPR_W,
	rect_wnd->bottom
	};

	//Direct2D初期化
	D2D1_Setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg_window));

	if (g_render_target != NULL) {
		g_render_target->BeginDraw();
		if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);
		pBrush->SetColor(D2D1::ColorF(TO_BGR(BRIGHTEN(g_theme[g_config.theme].bg_window, CE_BR_SEPR))));

		if (pBrush) g_render_target->DrawLine(
			D2D1::Point2F(rect_sepr.left + CE_SEPR_W, (rect_sepr.top + rect_sepr.bottom) * 0.5 - CE_SEPR_LINE_L),
			D2D1::Point2F(rect_sepr.left + CE_SEPR_W, (rect_sepr.top + rect_sepr.bottom) * 0.5 + CE_SEPR_LINE_L),
			pBrush, CE_SEPR_LINE_W, NULL
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
//		サイドパネルを描画
//---------------------------------------------------------------------
void DrawSide(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	D2D1_Setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

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
void DrawLibrary(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	D2D1_Setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

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
void DrawEditor(HWND hwnd, HDC hdc_mem, LPRECT rect_wnd)
{
	HDC hdc;
	static ID2D1SolidColorBrush* pBrush = NULL;

	//Direct2D初期化
	D2D1_Setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].bg));

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
void DrawGraph(HWND hwnd, HDC hdc_mem, POINT* pt_trace, LPRECT rect_wnd)
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
			ToClient(g_curve_value.control_point[0]).x,
			ToClient(g_curve_value.control_point[0]).y
		},
		{
			ToClient(g_curve_value.control_point[1]).x,
			ToClient(g_curve_value.control_point[1]).y
		},
		{
			g_disp_info.o.x + g_disp_info.scale.x * CE_GR_RES,
			g_disp_info.o.y - g_disp_info.scale.y * CE_GR_RES
		}
	};
	DoublePoint ctpt_hs_cl[] = {
		ctpt_cl[0],
		{
			ToClient(pt_trace[0]).x,
			ToClient(pt_trace[0]).y
		},
		{
			ToClient(pt_trace[1]).x,
			ToClient(pt_trace[1]).y
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

	//Direct2D初期化
	D2D1_Setup(hdc_mem, rect_wnd, TO_BGR(g_theme[g_config.theme].gr_bg));

	//描画
	if (g_render_target != NULL && g_d2d1_factory != NULL) {
		g_render_target->BeginDraw();
		if (pBrush == NULL) g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &pBrush);

		//グリッド
		D2D1_DrawGrid(pBrush, rect_wnd);

		pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].gr_bg), CE_BR_GR_INVALID)));
		pBrush->SetOpacity(0.5);
		if (pBrush) {
			g_render_target->FillRectangle(&rect_left, pBrush);
			g_render_target->FillRectangle(&rect_right, pBrush);
		}
		pBrush->SetOpacity(1);


		//値モードのとき
		if (!g_config.mode) {
			//bezier curve (trace)
			if (g_config.trace) {
				pBrush->SetColor(D2D1::ColorF(BRIGHTEN(TO_BGR(g_theme[g_config.theme].gr_bg), CE_BR_TRACE)));
				D2D1_DrawBezier(pBrush, ctpt_hs_cl[0], ctpt_hs_cl[1], ctpt_hs_cl[2], ctpt_hs_cl[3], CE_CURVE_TH);
			}
			//ベジェ
			pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve)));
			D2D1_DrawBezier(pBrush, ctpt_cl[0], ctpt_cl[1], ctpt_cl[2], ctpt_cl[3], CE_CURVE_TH);

			if (g_config.show_handle) {
				//ハンドルの色
				pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].handle)));
				//ハンドル（左）
				D2D1_DrawHandle(pBrush, ctpt_cl[0], ctpt_cl[1]);
				//ハンドル（右）
				D2D1_DrawHandle(pBrush, ctpt_cl[3], ctpt_cl[2]);
			}
		}

		//IDモードのとき
		else {
			for (int i = 0; i < g_curve_id[g_config.id_current].control_points.size() - 1; i++)
			{
				//色を指定
				pBrush->SetColor(D2D1::ColorF(TO_BGR(CONTRAST(INVERT(g_theme[g_config.theme].gr_bg), CE_GR_POINT_CONTRAST))));
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

				//端点以外の制御点に引かれる点線
				if (i > 0)
					g_render_target->DrawLine(
						D2D1::Point2F(ToClient(g_curve_id[g_config.id_current].control_points[i].pt_center).x, 0),
						D2D1::Point2F(ToClient(g_curve_id[g_config.id_current].control_points[i].pt_center).x, rect_wnd->bottom),
						pBrush, CE_GR_POINT_TH, pStyle
					);

				//ベジェ曲線を描画
				pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].curve)));
				D2D1_DrawBezier(pBrush,
					ToClient(g_curve_id[g_config.id_current].control_points[i].pt_center),
					ToClient(g_curve_id[g_config.id_current].control_points[i].pt_right),
					ToClient(g_curve_id[g_config.id_current].control_points[i + 1].pt_left),
					ToClient(g_curve_id[g_config.id_current].control_points[i + 1].pt_center),
					CE_CURVE_TH
				);

				//ハンドルの描画
				if (g_config.show_handle) {
					pBrush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].handle)));
					D2D1_DrawHandle(pBrush,
						ToClient(g_curve_id[g_config.id_current].control_points[i].pt_center),
						ToClient(g_curve_id[g_config.id_current].control_points[i].pt_right)
					);
					D2D1_DrawHandle(pBrush,
						ToClient(g_curve_id[g_config.id_current].control_points[i + 1].pt_center),
						ToClient(g_curve_id[g_config.id_current].control_points[i + 1].pt_left)
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