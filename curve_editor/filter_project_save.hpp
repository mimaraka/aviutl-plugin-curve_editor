#pragma once

#include <aviutl/FilterPlugin.hpp>



namespace cved {
	BOOL filter_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size);
}