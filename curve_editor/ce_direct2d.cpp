//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (Direct2D)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void ce::Direct2d_Window::init(HWND hwnd, LPRECT rect)
{
	D2D1_SIZE_U size = {
		rect->right - rect->left,
		rect->bottom - rect->top
	};
	g_d2d1_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hwnd, size),
		&render_target
	);
}


//---------------------------------------------------------------------
//		準備
//---------------------------------------------------------------------
void ce::Direct2d_Window::setup(COLORREF cr)
{
	render_target->BeginDraw();
	g_render_target->SetTransform(D2D1::Matrix3x2F::Identity());
	g_render_target->Clear(D2D1::ColorF(TO_BGR(cr)));
	g_render_target->EndDraw();
}



//---------------------------------------------------------------------
//		リサイズ
//---------------------------------------------------------------------
void ce::Direct2d_Window::resize(int width, int height)
{
	D2D1_SIZE_U size = {
		width,
		height
	};
	render_target->Resize(&size);
}



//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
void ce::Direct2d_Window::exit()
{
	if (render_target != NULL)
		render_target->Release();
}