#pragma once

#include <format>
#include <functional>
#include <mkaul/rectangle.hpp>
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

		template <typename... Args>
		void post_message(const std::wstring& to, const std::wstring& command, Args... args) {
			static_assert(sizeof...(Args) % 2 == 0, "The number of arguments must be even.");
			if constexpr (0 < sizeof...(Args)) {
				static_assert(std::is_same_v<const std::wstring&, Args...>
					or std::is_same_v<std::wstring, Args...>
					or std::is_same_v <const wchar_t*, Args... >,
					"The type of arguments must be std::wstring or std::wstring& or const wchar_t*."
				);
			}
			
			if (!is_ready_) {
				return;
			}
			std::vector<std::wstring> arg_list = { args... };
			std::wstring options = L"";
			for (size_t i = 0; i < arg_list.size(); i += 2) {
				std::wstring key = arg_list[i];
				std::wstring value = arg_list[i + 1];
				options += std::format(L",\"{}\":\"{}\"", key, value);
			}

			webview_->PostWebMessageAsJson(
				std::format(L"{{\"to\":\"{}\",\"command\":\"{}\"{}}}", to, command, options).c_str()
			);
		}
		
		bool init(HWND hwnd, std::function<void(MyWebView2*)> after_callback);
		bool ready() const noexcept { return is_ready_; }
		void destroy() noexcept;
		auto get_webview() const noexcept { return webview_.get(); }
		auto get_hwnd() const noexcept { return hwnd_; }
		void navigate(const std::wstring& uri);
		void reload() noexcept;
		void execute_script(const std::wstring& script);
		void put_bounds(const mkaul::WindowRectangle& bounds);
		void on_move() noexcept;
		void update_color_scheme() noexcept;
	};
} // namespace cved