#pragma once

#include <aviutl/FilterPlugin.hpp>



namespace curve_editor {
	BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp);
} // namespace curve_editor