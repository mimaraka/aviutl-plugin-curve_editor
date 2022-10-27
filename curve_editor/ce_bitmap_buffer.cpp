//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（ビットマップバッファ）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::init(HWND hw)
{
	hwnd = hw;
	HDC hdc = GetDC(hw);
	hdc_memory = CreateCompatibleDC(hdc);
	bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
	SelectObject(hdc_memory, bitmap);
	ReleaseDC(hw, hdc);

	if (brush == nullptr)
		g_render_target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &brush);
}



//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::exit() const
{
	DeleteDC(hdc_memory);
	DeleteObject(bitmap);

	brush->Release();
}



//---------------------------------------------------------------------
//		RenderTargetをDCにバインド & 背景を塗りつぶし
//---------------------------------------------------------------------
bool ce::Bitmap_Buffer::d2d_setup(COLORREF cr)
{
	if (g_render_target != nullptr && g_d2d1_factory != nullptr) {
		g_render_target->BindDC(hdc_memory, &rect);
		g_render_target->BeginDraw();
		g_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
		g_render_target->Clear(D2D1::ColorF(cr));
		g_render_target->EndDraw();
		return true;
	}
	else
		return false;
}







//---------------------------------------------------------------------
//		ビットマップをバッファから画面に転送
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::transfer() const
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdc_memory, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}



//---------------------------------------------------------------------
//		サイズを更新
//---------------------------------------------------------------------
void ce::Bitmap_Buffer::set_size(const RECT& rect_wnd)
{
	rect = rect_wnd;
}