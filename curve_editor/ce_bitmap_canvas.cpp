//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（ビットマップキャンバス）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void ce::Bitmap_Canvas::init(HWND hw)
{
	hwnd = hw;
	HDC hdc = GetDC(hw);
	hdc_memory = CreateCompatibleDC(hdc);
	bitmap = CreateCompatibleBitmap(hdc, CE_MAX_W, CE_MAX_H);
	SelectObject(hdc_memory, bitmap);
	ReleaseDC(hw, hdc);
}



//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
void ce::Bitmap_Canvas::exit()
{
	DeleteDC(hdc_memory);
	DeleteObject(bitmap);
}



//---------------------------------------------------------------------
//		ビットマップをバッファから画面に転送
//---------------------------------------------------------------------
void ce::Bitmap_Canvas::transfer(LPRECT rect)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	BitBlt(hdc, 0, 0, rect->right, rect->bottom, hdc_memory, 0, 0, SRCCOPY);
	EndPaint(hwnd, &ps);
	DeleteDC(hdc);
}