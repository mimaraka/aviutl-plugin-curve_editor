#pragma once

#include "curve.hpp"
#include <aviutl.hpp>
#include <mkaul/exedit.hpp>
#include <mkaul/window.hpp>
#include <WebView2.h>
#include <wil/com.h>



namespace cved {
	namespace global {
		inline AviUtl::FilterPlugin* fp = nullptr;
		inline mkaul::exedit::Internal exedit_internal;
		inline wil::com_ptr<ICoreWebView2Environment> webview_env = nullptr;
	}
}