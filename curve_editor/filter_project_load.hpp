#pragma once

#include <aviutl/FilterPlugin.hpp>



namespace curve_editor {
	BOOL filter_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size);
} // namespace curve_editor