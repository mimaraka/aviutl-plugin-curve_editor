#include "actctx_helper.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "drag_and_drop.hpp"
#include "filter_wndproc.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "my_webview2.hpp"
#include "resource.h"
#include "string_table.hpp"
#include "webmessage_handler.hpp"
#include <nlohmann/json.hpp>



namespace cved {
	// キー押下時の処理
	void on_keydown(WPARAM wparam) {
		switch (wparam) {
		case VK_LEFT:
			if (!global::editor.editor_graph().numeric_curve()) {
				//global::window_toolbar.send_command((WPARAM)WindowCommand::IdBack);
			}
			break;

		case VK_RIGHT:
			if (!global::editor.editor_graph().numeric_curve()) {
				//global::window_toolbar.send_command((WPARAM)WindowCommand::IdNext);
			}
			break;

		case VK_DELETE:
			//global::window_toolbar.send_command((WPARAM)WindowCommand::Clear);
			break;

		case 'A':
			global::config.set_align_handle(!global::config.get_align_handle());
			break;

		case 'C':
			if (::GetAsyncKeyState(VK_CONTROL) & 0x8000 and global::editor.editor_graph().numeric_curve()) {
				//global::window_toolbar.send_command((WPARAM)WindowCommand::Copy);
			}
			break;

		case 'R':
			//global::window_grapheditor.send_command((WPARAM)WindowCommand::Reverse);
			break;

		case 'S':
			if (::GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				//global::window_toolbar.send_command((WPARAM)WindowCommand::Save);
			}
			break;
		}
	}


	// ウィンドウプロシージャ
	BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM, AviUtl::EditHandle*, AviUtl::FilterPlugin* fp) {
		using WindowMessage = AviUtl::FilterPlugin::WindowMessage;
		using StringId = global::StringTable::StringId;

		static ActCtxHelper actctx_helper;
		static MyWebView2 my_webview;
		static DragAndDrop dnd;
		static bool init = false;

		switch (message) {
			// ウィンドウ作成時
		case WindowMessage::Init:
			// Alpha版・Beta版の場合はキャプションを変更
			if (global::PLUGIN_VERSION.is_preview()) {
				::SetWindowTextA(
					hwnd,
					std::format("{} ({})", global::PLUGIN_NAME, global::PLUGIN_VERSION.preview_type().str(true)).c_str()
				);
			}
			dnd.init();
			break;

			// AviUtl終了時
		case WindowMessage::Exit:
			actctx_helper.exit();
			dnd.exit();
			break;

		case WindowMessage::ChangeWindow:
			if (fp->exfunc->is_filter_window_disp(fp) and !init) {
				actctx_helper.init(fp->dll_hinst);
				init = true;
				my_webview.init(hwnd, [](MyWebView2* this_) {
					mkaul::WindowRectangle bounds;
					bounds.from_client_rect(this_->get_hwnd());
					this_->put_bounds(bounds);
					this_->navigate(L"panel_main");
					});
			}
			break;

		case WM_SIZE:
		{
			mkaul::WindowRectangle bounds;
			bounds.from_client_rect(hwnd);
			my_webview.put_bounds(bounds);
			break;
		}

		case WM_MOVE:
			my_webview.on_move();
			break;

		case WM_KEYDOWN:
			if (global::config.get_enable_hotkeys()) {
				on_keydown(wparam);
			}
			break;

		case WM_MOUSEMOVE:
			if (dnd.is_dragging()) {
				dnd.update();
			}
			break;

		case WM_LBUTTONUP:
			if (dnd.is_dragging()) {
				::ReleaseCapture();
				dnd.drop();
				my_webview.execute_script(L"onDrop();");
			}
			break;

		case WM_COMMAND:
			switch ((WindowCommand)wparam) {
			case WindowCommand::RedrawAviutl:
				return TRUE;

			case WindowCommand::Reload:
				my_webview.reload();
				break;

			case WindowCommand::StartDnd:
				::SetCapture(hwnd);
				::SetCursor(::LoadCursorA(fp->dll_hinst, MAKEINTRESOURCEA(IDC_DRAG)));
				dnd.drag();
				break;

			case WindowCommand::UpdateHandles:
				my_webview.post_message(L"editor-graph", L"updateHandles");
				break;

			case WindowCommand::UpdateHandlePos:
				my_webview.post_message(L"editor-graph", L"updateHandlePos");
				break;

			case WindowCommand::UpdateAxisLabelVisibility:
				my_webview.post_message(L"editor-graph", L"updateAxisLabelVisibility");
				break;

			case WindowCommand::UpdateHandleVisibility:
				my_webview.post_message(L"editor-graph", L"updateHandleVisibility");
				break;

			case WindowCommand::ApplyPreferences:
				my_webview.update_color_scheme();
				my_webview.post_message(L"editor-graph", L"applyPreferences");
				break;
			}
			break;
		}
		return FALSE;
	}
}