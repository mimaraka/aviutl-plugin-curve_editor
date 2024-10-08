#pragma once

#include <wil/com.h>
#include <WebView2.h>
#include <aviutl.hpp>
#include <mkaul/window.hpp>
#include <mkaul/exedit.hpp>
#include "curve.hpp"
#include "theme.hpp"


namespace cved {
	namespace global {
		inline AviUtl::FilterPlugin* fp = nullptr;
		inline mkaul::exedit::Internal exedit_internal;
		inline wil::com_ptr<ICoreWebView2Environment> webview_env = nullptr;
	}
}