#pragma once

#include <format>
#include <functional>
#include <mkaul/rectangle.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <WebView2.h>
#include <wil/com.h>
#include <wrl.h>



namespace cved {
	class MyWebView2 {
		wil::com_ptr<ICoreWebView2Environment> env_ = nullptr;
		wil::com_ptr<ICoreWebView2Controller> controller_ = nullptr;
		wil::com_ptr<ICoreWebView2> webview_ = nullptr;
		HWND hwnd_ = NULL;
		bool is_ready_ = false;

		bool handle_message(const nlohmann::json& message) noexcept;

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
		bool ready() const noexcept { return is_ready_; }
		void destroy() noexcept;
		auto get_webview() const noexcept { return webview_.get(); }
		auto get_hwnd() const noexcept { return hwnd_; }
		void navigate(std::function<void(MyWebView2*)> after_callback = nullptr);
		void reload() noexcept;
		void put_bounds(const mkaul::WindowRectangle& bounds);
		void on_move() noexcept;
		void post_message(const std::wstring& command, const nlohmann::json& options = nlohmann::json::object()) const noexcept;
		void update_color_scheme() noexcept;
	};
} // namespace cved