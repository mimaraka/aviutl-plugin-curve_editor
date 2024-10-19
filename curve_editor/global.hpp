#pragma once

#include "my_webview2.hpp"
#include <aviutl.hpp>
#include <mkaul/exedit.hpp>



namespace cved {
	namespace global {
		inline AviUtl::FilterPlugin* fp = nullptr;
		inline mkaul::exedit::Internal exedit_internal;
	}
}