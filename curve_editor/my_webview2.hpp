#pragma once

#include <string>
#include <functional>
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include <mkaul/rectangle.hpp>



namespace cved {
	class MyWebView2 {
		wil::com_ptr<ICoreWebView2Environment> env_ = nullptr;
		wil::com_ptr<ICoreWebView2Controller> controller_ = nullptr;
		wil::com_ptr<ICoreWebView2> webview_ = nullptr;
		HWND hwnd_ = NULL;
		bool is_ready_ = false;

	public:
		MyWebView2() noexcept {};
		~MyWebView2() noexcept { destroy(); }

		template <class HostObjectClass>
		HRESULT add_host_object(const std::wstring& name) {
			if (!is_ready_) {
				return E_FAIL;
			}
			wil::com_ptr<HostObjectClass> object = Microsoft::WRL::Make<HostObjectClass>();
			VARIANT var_object = {};
			var_object.vt = VT_DISPATCH;
			object.query_to<IDispatch>(&var_object.pdispVal);
			return webview_->AddHostObjectToScript(name.c_str(), &var_object);
		}
		
		bool init(HWND hwnd, std::function<void(MyWebView2*)> after_callback);
		void destroy() noexcept;
		auto get_webview() const noexcept { return webview_.get(); }
		auto get_hwnd() const noexcept { return hwnd_; }
		void navigate(const std::wstring& uri);
		void reload() noexcept;
		void execute_script(const std::wstring& script);
		void put_bounds(const mkaul::WindowRectangle& bounds);
		void on_move() noexcept;
	};
} // namespace cved