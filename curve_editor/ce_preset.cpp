//----------------------------------------------------------------------------------
//		Curve Editor
//		�\�[�X�t�@�C��(�v���Z�b�g)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		�v���Z�b�g���쐬
//---------------------------------------------------------------------
BOOL ce::Preset::create(HWND hwnd_parent)
{
	WNDCLASSEX tmp;
	tmp.cbSize = sizeof(tmp);
	tmp.style = CS_HREDRAW | CS_VREDRAW;
	tmp.lpfnWndProc = wndproc_static;
	tmp.cbClsExtra = 0;
	tmp.cbWndExtra = 0;
	tmp.hInstance = g_fp->dll_hinst;
	tmp.hIcon = NULL;
	tmp.hIconSm = NULL;
	tmp.hCursor = LoadCursor(NULL, IDC_ARROW);
	tmp.hbrBackground = NULL;
	tmp.lpszMenuName = NULL;
	tmp.lpszClassName = name;

	if (RegisterClassEx(&tmp)) {
		hwnd = ::CreateWindowEx(
			NULL,
			name,
			NULL,
			WS_CHILD | WS_VISIBLE,
			0, 0,
			g_config.preset_size,
			g_config.preset_size,
			hwnd_parent,
			NULL,
			g_fp->dll_hinst,
			this
		);
		if (hwnd != nullptr)
			return 1;
	}
	return 0;
}



//---------------------------------------------------------------------
//		�v���Z�b�g���ړ�
//---------------------------------------------------------------------
void ce::Preset::move(int width)
{
	const int w = 2 * CE_MARGIN_PRESET + g_config.preset_size;
	int x_count = index % (width / w);
	int y_count = index / (width / w);
	::MoveWindow(
		hwnd,
		w * x_count,
		w * y_count,
		g_config.preset_size,
		g_config.preset_size + CE_MARGIN_PRESET,
		TRUE
		);
}



//---------------------------------------------------------------------
//		�ÓI�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------
LRESULT CALLBACK ce::Preset::wndproc_static(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Preset* app = (Preset*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!app) {//�擾�ł��Ȃ�����(�E�B���h�E������)�ꍇ
		if (msg == WM_CREATE) {
			app = (Preset*)((LPCREATESTRUCT)lparam)->lpCreateParams;
			if (app) {
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
				return app->wndproc(hwnd, msg, wparam, lparam);
			}
		}
	}
	else {//�擾�ł����ꍇ(�E�B���h�E������)
		return app->wndproc(hwnd, msg, wparam, lparam);
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



//---------------------------------------------------------------------
//		�E�B���h�E�v���V�[�W��(static�ϐ��g�p�s��)
//---------------------------------------------------------------------
LRESULT ce::Preset::wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	return ::DefWindowProc(hwnd, msg, wparam, lparam);
}