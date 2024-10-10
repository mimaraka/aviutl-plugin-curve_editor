#pragma once

#include "curve.hpp"
#include "my_webview2.hpp"
#include <aviutl.hpp>
#include <mkaul/exedit.hpp>
#include <mkaul/window.hpp>



namespace cved {
	namespace global {
		inline AviUtl::FilterPlugin* fp = nullptr;
		inline mkaul::exedit::Internal exedit_internal;
		inline MyWebView2 webview_main;
	}
}