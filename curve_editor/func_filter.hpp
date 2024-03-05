#pragma once

#include <Windows.h>
#include <aviutl.hpp>



namespace cved {
	BOOL filter_init(AviUtl::FilterPlugin* fp);
	BOOL filter_exit(AviUtl::FilterPlugin* fp);
	BOOL filter_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size);
	BOOL filter_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size);
	BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp);
}