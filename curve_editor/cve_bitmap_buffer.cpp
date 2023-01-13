//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（ビットマップバッファ）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_GR_GRID_TH_L				0.5f
#define CVE_GR_GRID_TH_B				1.0f
#define CVE_GR_GRID_MIN					36



//---------------------------------------------------------------------
//		コンストラクタ
//---------------------------------------------------------------------
cve::Bitmap_Buffer::Bitmap_Buffer() :
	bitmap(NULL),
	hwnd(NULL),
	hdc_memory(NULL),
	brush(nullptr),
	rect({NULL})
{}



//---------------------------------------------------------------------
//		
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::exit()
{
	if (hdc_memory)
		::DeleteDC(hdc_memory);
	if (bitmap)
		::DeleteObject(bitmap);

	release(&brush);
}



void cve::Bitmap_Buffer::init(HWND hw)
{
	hwnd = hw;
	HDC hdc = ::GetDC(hw);
	hdc_memory = ::CreateCompatibleDC(hdc);
	bitmap = ::CreateCompatibleBitmap(hdc, CVE_MAX_W, CVE_MAX_H);
	::SelectObject(hdc_memory, bitmap);
	::ReleaseDC(hw, hdc);

	resize();

	if (g_p_render_target != nullptr && brush == nullptr)
		g_p_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);
}



//---------------------------------------------------------------------
//		RenderTargetをDCにバインド & 背景を塗りつぶし
//---------------------------------------------------------------------
bool cve::Bitmap_Buffer::d2d_setup(COLORREF cr)
{
	if (g_p_render_target != nullptr && g_p_factory != nullptr) {
		g_p_render_target->BindDC(hdc_memory, &rect);
		g_p_render_target->BeginDraw();
		g_p_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
		g_p_render_target->Clear(D2D1::ColorF(cr));
		g_p_render_target->EndDraw();
		return true;
	}
	else
		return false;
}



//---------------------------------------------------------------------
//		ビットマップをバッファから画面に転送
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::transfer() const
{
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint(hwnd, &ps);
	::BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdc_memory, 0, 0, SRCCOPY);
	::EndPaint(hwnd, &ps);
	::DeleteDC(hdc);
}



//---------------------------------------------------------------------
//		サイズを更新
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::resize()
{
	::GetClientRect(hwnd, &rect);
}



//---------------------------------------------------------------------
//		グラフのグリッドを描画
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::draw_grid()
{
	brush->SetColor(D2D1::ColorF(CHANGE_BRIGHTNESS(TO_BGR(g_theme[g_config.theme].bg_graph), CVE_BR_GRID)));
	// 
	int kx = (int)std::floor(std::log(CVE_GRAPH_RESOLUTION * g_view_info.scale.x / (double)CVE_GR_GRID_MIN) / std::log(CVE_GRAPH_GRID_NUM));
	int ky = (int)std::floor(std::log(CVE_GRAPH_RESOLUTION * g_view_info.scale.y / (double)CVE_GR_GRID_MIN) / std::log(CVE_GRAPH_GRID_NUM));
	// グラフの枠内に表示されるグリッドの本数
	int nx = MIN_LIMIT((int)std::pow(CVE_GRAPH_GRID_NUM, kx), 1);
	int ny = MIN_LIMIT((int)std::pow(CVE_GRAPH_GRID_NUM, ky), 1);
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

	float ax = to_client((int)(lx * CVE_GRAPH_RESOLUTION / (float)nx), 0).x;
	float ay = to_client(0, (int)(ly * CVE_GRAPH_RESOLUTION / (float)ny)).y;
	float thickness;

	// 縦方向
	for (int i = 0; ax + dx * i <= rect.right; i++) {
		if ((lx + i) % CVE_GRAPH_GRID_NUM == 0)
			thickness = CVE_GR_GRID_TH_B;
		else
			thickness = CVE_GR_GRID_TH_L;
		g_p_render_target->DrawLine(
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

		g_p_render_target->DrawLine(
			D2D1::Point2F(0, ay + dy * i),
			D2D1::Point2F((float)rect.right, ay + dy * i),
			brush, thickness, NULL
		);
	}
}



//---------------------------------------------------------------------
//		ラウンドエッジを描画
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::draw_rounded_edge(int flag, float radius) {
	ID2D1GeometrySink* sink = nullptr;
	ID2D1PathGeometry* edge = nullptr;
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

	g_p_factory->CreatePathGeometry(&edge);
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

	if (edge != nullptr)
		g_p_render_target->FillGeometry(edge, brush, NULL);
	
	release(&edge);
	release(&sink);
}



//---------------------------------------------------------------------
//		メインウィンドウを描画
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::draw_panel_main(const RECT& rect_sepr)
{
	ID2D1StrokeStyle* style = nullptr;

	g_p_factory->CreateStrokeStyle(
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

	//Direct2D初期化
	d2d_setup(TO_BGR(g_theme[g_config.theme].bg));

	if (g_p_render_target != nullptr) {
		g_p_render_target->BeginDraw();

		brush->SetColor(D2D1::ColorF(TO_BGR(g_theme[g_config.theme].sepr)));

		if (brush) g_p_render_target->DrawLine(
			line_start,
			line_end,
			brush, CVE_SEPARATOR_LINE_WIDTH, style
		);

		g_p_render_target->EndDraw();
	}
	release(&style);

	//ビットマップをバッファから画面に転送
	transfer();
}



//---------------------------------------------------------------------
//		パネルを描画
//---------------------------------------------------------------------
void cve::Bitmap_Buffer::draw_panel()
{
	//Direct2D初期化
	d2d_setup(TO_BGR(g_theme[g_config.theme].bg));

	//ビットマップをバッファから画面に転送
	transfer();
}