#include "window_select_idx.hpp"

#include "config.hpp"
#include "curve_editor.hpp"
#include "enum.hpp"
#include "global.hpp"
#include "my_webview2_reference.hpp"
#include "string_table.hpp"


namespace curve_editor {
	LRESULT CALLBACK SelectIdxWindow::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using WebViewType = global::MyWebView2Reference::WebViewType;

		static MyWebView2 my_webview;

		switch (message) {
		case WM_CREATE:
			my_webview.init(hwnd, [](MyWebView2* this_) {
				mkaul::WindowRectangle bounds;
				bounds.from_client_rect(this_->get_hwnd());
				this_->put_bounds(bounds);
				this_->navigate([](MyWebView2* this_) {
					this_->send_command(
						MessageCommand::InitComponent,
						{
							{ "page", "CurveIdxSelector" }
						}
					);
				});
			});
			global::webview.set(WebViewType::SelectIdx, my_webview);
			global::webview.switch_to(WebViewType::SelectIdx);
			return 0;

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
			global::config.set_select_idx_window_size(mkaul::Size{ static_cast<uint32_t>(rect.right), static_cast<uint32_t>(rect.bottom) });
			my_webview.destroy();
			global::webview.switch_to(WebViewType::Main);
			::DestroyWindow(hwnd);
			return 0;
		}

		case WM_COMMAND:
			switch (wparam) {
			case (WPARAM)WindowCommand::SelectIdxClose:
				::DestroyWindow(hwnd);
				break;
			}
			return 0;
		}
		// ウィンドウは Unicode（RegisterClassExW/CreateWindowExW）で作成されるため、
		// 既定処理も Unicode 版に合わせる（ANSI 版だとタイトルが文字化けする）
		return ::DefWindowProc(hwnd, message, wparam, lparam);
	}

	HWND SelectIdxWindow::create(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_parent_ = hwnd;
		auto window_size = global::config.get_select_idx_window_size();
		// TODO: 起動する度にサイズが小さくなってしまうため、応急処置
		auto rect = mkaul::WindowRectangle{ 0, 0, static_cast<LONG>(window_size.width) + 16, static_cast<LONG>(window_size.height) + 39 };
		rect.client_to_screen(hwnd);

		return mkaul::ui::Window::create(
			global::exedit_internal.fp()->dll_hinst,
			hwnd,
			global::string_table[StringId::CaptionSelectCurve],
			L"SelectIdxWindow",
			&wndproc,
			WS_POPUPWINDOW | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
			NULL,
			rect,
			mkaul::WindowRectangle{},
			::LoadCursor(NULL, IDC_ARROW)
		);
	}
} // namespace curve_editor
