#pragma once

#include <nlohmann/json.hpp>
#include <WebView2.h>


namespace cved {
	bool handle_webmessage(HINSTANCE hinst, HWND hwnd, ICoreWebView2* webview, const nlohmann::json& message);
}