#include "curve_editor.hpp"
#include "global.hpp"
#include "my_webview2.hpp"
#include "string_table.hpp"
#include "window_select_curve.hpp"
#include <format>
#include <sol/sol.hpp>



namespace cved {
	LRESULT CALLBACK SelectCurveWindow::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static MyWebView2 my_webview;
		static int i = 0;
		const HWND* p_hwnd_script_param = global::exedit_internal.get<HWND>(0xd4524);

		switch (message) {
		case WM_CREATE:
		{
			constexpr size_t BUFFER_SIZE = 128;
			char buffer[BUFFER_SIZE];

			i = *static_cast<int*>(std::bit_cast<LPCREATESTRUCTA>(lparam)->lpCreateParams);
			if (!p_hwnd_script_param) {
				return -1;
			}
			::GetDlgItemTextA(*p_hwnd_script_param, 170 + i, buffer, BUFFER_SIZE);
			::EnableWindow(*p_hwnd_script_param, FALSE);

			my_webview.init(hwnd, [buffer](MyWebView2* this_) {
				mkaul::WindowRectangle bounds;
				bounds.from_client_rect(this_->get_hwnd());
				this_->put_bounds(bounds);
				this_->navigate(L"select_dialog", [buffer](MyWebView2* this_) {
					sol::state lua;
					try {
						lua.script(std::format("t={}", buffer));
						auto mode_str = lua["t"]["mode"].get_or<std::string>(global::CURVE_NAME_NORMAL);
						auto param = lua["t"]["param"].get_or<int, int>(1);
						auto mode = global::editor.get_mode(mode_str);
						if (mode != EditMode::NumEditMode) {
							this_->post_message(
								L"panel-editor", L"setCurve",
								{
									{ "mode", mode},
									{ "param", param }
								}
							);
						}
					}
					catch (const sol::error&) {}
					});
				});
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
		case WM_CLOSE:
			my_webview.destroy();
			if (p_hwnd_script_param) {
				::EnableWindow(*p_hwnd_script_param, TRUE);
				::SetActiveWindow(*p_hwnd_script_param);
			}
			::DestroyWindow(hwnd);
			return 0;

		case WM_COMMAND:
			switch (wparam) {
			case (WPARAM)WindowCommand::SetCurveInfo:
			{
				auto info = std::bit_cast<std::pair<std::string, int>*>(lparam);
				if (info and p_hwnd_script_param) {
					::SetDlgItemTextA(
						*p_hwnd_script_param,
						170 + i,
						std::format("{{mode=\"{}\",param={}}}", info->first, info->second).c_str()
					);
				}
				global::webview_main.post_message(L"panel-editor", L"updateEditor");
				break;
			}
			}
			return 0;
		}
		return ::DefWindowProcA(hwnd, message, wparam, lparam);
	}

	HWND SelectCurveWindow::create(HWND hwnd, void* init_param) noexcept {
		using StringId = global::StringTable::StringId;
		constexpr long WINDOW_WIDTH = 300;
		constexpr long WINDOW_HEIGHT = 450;

		auto rect = mkaul::WindowRectangle{ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
		rect.client_to_screen(hwnd);

		return mkaul::ui::Window::create(
			global::exedit_internal.fp()->dll_hinst,
			hwnd,
			global::string_table[StringId::LabelSelectCurve],
			"SelectCurveWindow",
			&wndproc,
			WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME,
			NULL,
			rect,
			mkaul::WindowRectangle{},
			::LoadCursorA(NULL, IDC_ARROW),
			init_param
		);
	}
} // namespace cved