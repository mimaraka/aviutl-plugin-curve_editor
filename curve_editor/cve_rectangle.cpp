//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル（矩形）
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		初期化(1)
//---------------------------------------------------------------------
void cve::Rectangle::set(const RECT& rc)
{
	rect = rc;
}



//---------------------------------------------------------------------
//		初期化(2)
//---------------------------------------------------------------------
void cve::Rectangle::set(int left, int top, int right, int bottom)
{
	rect = {
		left, top,
		right, bottom
	};
}



//---------------------------------------------------------------------
//		横n個に分割する
//---------------------------------------------------------------------
void cve::Rectangle::divide(LPRECT rects_child[], float weights[], int n) const
{
	// 親要素の横幅
	int width_parent = rect.right - rect.left;
	// 等分する場合の子要素の横幅
	int width_child = width_parent / n;
	float weights_sum = 0;
	// 0番目からk-1番目までの重みの総和がn個格納された配列
	std::vector<float> list_weights_sum;
	int left, right;

	// 重みがある場合
	if (weights) {
		// 重みの総和を計算するとともに、0番目からk-1番目までの重みの総和をlist_weights_sum[k - 1]に代入する。
		for (int i = 0; i < n; i++) {
			weights_sum += weights[i];
			list_weights_sum.emplace_back(weights_sum);
		}
	}

	// それぞれのRECTの位置を設定
	for (int i = 0; i < n; i++) {
		// 重みがある場合
		if (weights) {
			left = (int)(width_parent * ((i == 0) ? 0 : list_weights_sum[i - 1]) / weights_sum);
			right = (int)(width_parent * list_weights_sum[i] / weights_sum);
		}
		// 重みがない(等分する)場合
		else {
			left = i * width_child;
			right = (i + 1) * width_child;
		}
		rects_child[i]->left = rect.left + left;
		rects_child[i]->right = rect.left + right;
		rects_child[i]->top = rect.top;
		rects_child[i]->bottom = rect.bottom;
	}
}



//---------------------------------------------------------------------
//		余白を設定する
//---------------------------------------------------------------------
void cve::Rectangle::set_margin(int left, int top, int right, int bottom)
{
	rect.left += left;
	rect.top += top;
	rect.right -= right;
	rect.bottom -= bottom;

	if (rect.left > rect.right)
		rect.left = rect.right = (rect.left + rect.right) / 2;
	if (rect.top > rect.bottom)
		rect.top = rect.bottom = (rect.top + rect.bottom) / 2;
}



//---------------------------------------------------------------------
//		クライアントからスクリーン
//---------------------------------------------------------------------
void cve::Rectangle::client_to_screen(HWND hwnd)
{
	POINT pt[2];
	pt[0] = {
		rect.left,
		rect.top
	};
	pt[1] = {
		rect.right,
		rect.bottom
	};
	::ClientToScreen(hwnd, &pt[0]);
	::ClientToScreen(hwnd, &pt[1]);
	rect = {
		pt[0].x, pt[0].y,
		pt[1].x, pt[1].y,
	};
}



//---------------------------------------------------------------------
//		スクリーンからクライアント
//---------------------------------------------------------------------
void cve::Rectangle::screen_to_client(HWND hwnd)
{
	POINT pt[2];
	pt[0] = {
		rect.left,
		rect.top
	};
	pt[1] = {
		rect.right,
		rect.bottom
	};
	::ScreenToClient(hwnd, &pt[0]);
	::ScreenToClient(hwnd, &pt[1]);
	rect = {
		pt[0].x, pt[0].y,
		pt[1].x, pt[1].y,
	};
}