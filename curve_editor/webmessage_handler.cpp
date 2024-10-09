#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_curve_code.hpp"
#include "dialog_curve_param.hpp"
#include "dialog_id_jumpto.hpp"
#include "enum.hpp"
#include "menu_curve_segment.hpp"
#include "menu_graph.hpp"
#include "menu_others.hpp"
#include "my_messagebox.hpp"
#include "my_webview2.hpp"
#include "string_table.hpp"
#include "util.hpp"
#include "webmessage_handler.hpp"
#include <mkaul/util.hpp>
#include <mkaul/window.hpp>



namespace cved {
	bool handle_webmessage(HINSTANCE hinst, HWND hwnd, ICoreWebView2* webview, const nlohmann::json& message) {
		using StringId = global::StringTable::StringId;
		const auto& config = global::config;

		if (message.contains("command")) {
			auto command = message["command"].get<std::string>();
			if (command == "copy") {
				auto tmp = std::to_string(global::editor.track_param());
				if (!util::copy_to_clipboard(hwnd, tmp.c_str()) and config.get_show_popup()) {
					my_messagebox(global::string_table[StringId::ErrorCodeCopyFailed], hwnd, MessageBoxIcon::Error);
				}
				return true;
			}
			else if (command == "read") {
				CurveCodeDialog dialog;
				dialog.show(hwnd);
				return true;
			}
			else if (command == "save") {
				return true;
			}
			else if (command == "lock") {
				if (message.contains("switch")) {
					bool sw = message["switch"].get<bool>();
					if (sw) {
						return true;
					}
					else {
						return true;
					}
				}
			}
			else if (command == "clear") {
				int response = IDOK;
				if (config.get_show_popup()) {
					response = my_messagebox(
						global::string_table[StringId::WarningDeleteCurve],
						hwnd,
						MessageBoxIcon::Warning,
						MessageBoxButton::OkCancel
					);
				}

				if (response == IDOK) {
					auto curve = global::editor.current_curve();
					if (curve) {
						curve->clear();
						// TODO: アップデート処理
					}
				}
				return true;
			}
			else if (command == "others") {
				OthersMenu menu(hinst);
				menu.show(hwnd);
				return true;
			}
			/*else if (command == "flash") {
				auto hwnd_id = ::FindWindowA("IDWindow", NULL);
				if (hwnd_id) {
					::SendMessageA(hwnd_id, WM_COMMAND, 1, NULL);
				}
				return true;
			}*/
			else if (command == "button-id") {
				IdJumptoDialog dialog;
				dialog.show(hwnd);
				/*mkaul::ui::Window window;
				auto rect = mkaul::WindowRectangle{ 0, 0, 300, 450 };
				rect.client_to_screen(hwnd);
				webview->ExecuteScript(L"disablePanel();", nullptr);
				window.create(
					hinst,
					hwnd,
					"カーブを選択",
					"IDWindow",
					[](HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT {
						static MyWebView2 my_webview;
						static ICoreWebView2* webview = nullptr;

						switch (message) {
						case WM_CREATE:
							webview = reinterpret_cast<ICoreWebView2*>(reinterpret_cast<LPCREATESTRUCTA>(lparam)->lpCreateParams);
							my_webview.init(hwnd, [](MyWebView2* this_) {
								mkaul::WindowRectangle bounds;
								bounds.from_client_rect(this_->get_hwnd());
								this_->put_bounds(bounds);
								this_->navigate(L"select_dialog");
							});
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
						case WM_CLOSE:
							webview->ExecuteScript(L"enablePanel();", nullptr);
							my_webview.destroy();
							::DestroyWindow(hwnd);
							return 0;

						case WM_COMMAND:
							switch (wparam) {
							case 1:	
							{
								FLASHWINFO fwi;
								fwi.cbSize = sizeof(FLASHWINFO);
								fwi.hwnd = hwnd;
								fwi.dwFlags = FLASHW_ALL;
								fwi.uCount = 8;
								fwi.dwTimeout = 50;

								::FlashWindowEx(&fwi);
								::MessageBeep(MB_ICONASTERISK);
								::SetActiveWindow(hwnd);
								return 0;
							}
							}
							return 0;
						}
						return ::DefWindowProc(hwnd, message, wparam, lparam);
					},
					WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME,
					NULL,
					rect,
					mkaul::WindowRectangle{},
					::LoadCursorA(NULL, IDC_ARROW),
					webview
				);*/
				return true;
			}
			else if (command == "button-param") {
				CurveParamDialog dialog;
				dialog.show(hwnd);
				return true;
			}
			else if (command == "drag-and-drop") {
				::SendMessageA(hwnd, WM_COMMAND, (WPARAM)WindowCommand::StartDnd, NULL);
				return true;
			}
			else if (command == "contextmenu-graph") {
				GraphMenu menu{ hinst };
				menu.show(hwnd);
			}
			else if (command == "contextmenu-curve-segment") {
				// TODO: 値指定カーブに対応させる
				if (message.contains("curvePtr") and global::config.get_edit_mode() == EditMode::Normal) {
					auto curve_ptr = message["curvePtr"].get<int>();
					CurveSegmentMenu menu{ hinst };
					menu.show(reinterpret_cast<GraphCurve*>(curve_ptr), hwnd);
				}
			}
			else if (command == "grapheditor-reverse") {
				auto curve = global::editor.editor_graph().current_curve();
				if (curve) {
					curve->reverse();
					::SendMessageA(hwnd, WM_COMMAND, (WPARAM)WindowCommand::UpdateHandlePos, NULL);
				}
				return true;
			}
		}
		return false;
	}
}