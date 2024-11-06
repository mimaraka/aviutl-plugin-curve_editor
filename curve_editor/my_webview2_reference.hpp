#pragma once

#include "my_webview2.hpp"



namespace cved::global {
	inline class MyWebView2Reference {
		MyWebView2* p_webview_main_ = nullptr;
		MyWebView2* p_webview_select_curve_ = nullptr;
		MyWebView2** pp_current_webview_ = &p_webview_main_;

	public:
		enum class WebViewType {
			Main,
			SelectCurve
		};

		auto operator->() const noexcept {
			return *pp_current_webview_;
		}

		explicit operator bool() const noexcept {
			return *pp_current_webview_ != nullptr;
		}

		void set(WebViewType webview_type, MyWebView2& my_webview) noexcept {
			switch (webview_type) {
			case WebViewType::Main:
				p_webview_main_ = &my_webview;
				break;
			case WebViewType::SelectCurve:
				p_webview_select_curve_ = &my_webview;
				break;
			}
		}

		MyWebView2* get(WebViewType webview_type) {
			switch (webview_type) {
			case WebViewType::Main:
				return p_webview_main_;
			case WebViewType::SelectCurve:
				return p_webview_select_curve_;
			default:
				return nullptr;
			}
		}

		void switch_to(WebViewType webview_type) noexcept {
			switch (webview_type) {
			case WebViewType::Main:
				pp_current_webview_ = &p_webview_main_;
				break;
			case WebViewType::SelectCurve:
				pp_current_webview_ = &p_webview_select_curve_;
				break;
			}
		}
	} webview;
} // namespace cved::global