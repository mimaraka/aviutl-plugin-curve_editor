#include "window_select_curve.hpp"

#include "config.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "my_webview2_reference.hpp"
#include "string_table.hpp"


namespace curve_editor {
	LRESULT CALLBACK SelectCurveWindow::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using WebViewType = global::MyWebView2Reference::WebViewType;
		
		static MyWebView2 my_webview;

		switch (message) {
		case WM_CREATE:
		{
			auto [mode, param] = *static_cast<ModeParamPair*>(std::bit_cast<LPCREATESTRUCTA>(lparam)->lpCreateParams);

			my_webview.init(hwnd, [mode, param](MyWebView2* this_) {
				mkaul::WindowRectangle bounds;
				bounds.from_client_rect(this_->get_hwnd());
				this_->put_bounds(bounds);
				this_->navigate([mode, param](MyWebView2* this_) {
					this_->send_command(
						MessageCommand::InitComponent,
						{
							{ "page", "SelectDialog"},
							{ "mode", mode},
							{ "param", param }
						}
					);
				});
			});
			global::webview.set(WebViewType::SelectCurve, my_webview);
			global::webview.switch_to(WebViewType::SelectCurve);
			return 0;
		}

		case WM_SIZE:
		{
			mkaul::WindowRectangle bounds;
			bounds.from_client_rect(hwnd);
			my_webview.put_bounds(bounds);
			return 0;
		}

		case WM_MOVE:
			my_webview.on_move();
			return 0;

		case WM_DESTROY:
		{
			RECT rect;
			::GetClientRect(hwnd, &rect);
			global::config.set_select_window_size(mkaul::Size{ static_cast<uint32_t>(rect.right), static_cast<uint32_t>(rect.bottom) });
			my_webview.destroy();
			global::webview.switch_to(WebViewType::Main);
			::PostMessageA(::GetParent(hwnd), WM_COMMAND, (WPARAM)WindowCommand::SelectCurveClose, 0);
			::DestroyWindow(hwnd);
			return 0;
		}

		case WM_COMMAND:
			switch (wparam) {
			case (WPARAM)WindowCommand::SelectCurveOk:
				global::webview.get(global::MyWebView2Reference::WebViewType::Main)->send_command(MessageCommand::UpdateEditor);
				[[fallthrough]];

			case (WPARAM)WindowCommand::SelectCurveCancel:
				::SendMessageA(::GetParent(hwnd), message, wparam, lparam);
				::DestroyWindow(hwnd);
				break;
			}
			return 0;
		}
		return ::DefWindowProcA(hwnd, message, wparam, lparam);
	}

	HWND SelectCurveWindow::create(HWND hwnd, const ModeParamPair* mode_param) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_parent_ = hwnd;
		auto window_size = global::config.get_select_window_size();
		// TODO: 起動する度にサイズが小さくなってしまうため、応急処置
		auto rect = mkaul::WindowRectangle{ 0, 0, static_cast<LONG>(window_size.width) + 16, static_cast<LONG>(window_size.height) + 39};
		rect.client_to_screen(hwnd);

		return mkaul::ui::Window::create(
			global::exedit_internal.fp()->dll_hinst,
			hwnd,
			global::string_table[StringId::LabelSelectCurve],
			"SelectCurveWindow",
			&wndproc,
			WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
			NULL,
			rect,
			mkaul::WindowRectangle{},
			::LoadCursorA(NULL, IDC_ARROW),
			const_cast<ModeParamPair*>(mode_param)
		);
	}
} // namespace curve_editor