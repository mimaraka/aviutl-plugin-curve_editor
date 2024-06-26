#include "filter_wndproc.hpp"
#include "actctx_manager.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "curve_editor.hpp"
#include "global.hpp"
#include "wndproc_main.hpp"



namespace cved {
	// キー押下時の処理
	void on_keydown(WPARAM wparam) {
		switch (wparam) {
			// [R]
		case 82:
			global::window_grapheditor.send_command((WPARAM)WindowCommand::Reverse);
			break;

			// [C]
		case 67:
			if (::GetAsyncKeyState(VK_CONTROL) & 0x8000 and global::editor.editor_graph().numeric_curve()) {
				global::window_toolbar.send_command((WPARAM)WindowCommand::Copy);
			}
			break;

			// [S]
		case 83:
			if (::GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				global::window_toolbar.send_command((WPARAM)WindowCommand::Save);
			}
			break;

			// [<]
		case VK_LEFT:
			if (!global::editor.editor_graph().numeric_curve()) {
				global::window_toolbar.send_command((WPARAM)WindowCommand::IdBack);
			}
			break;

			// [>]
		case VK_RIGHT:
			if (!global::editor.editor_graph().numeric_curve()) {
				global::window_toolbar.send_command((WPARAM)WindowCommand::IdNext);
			}
			break;

			// [Home]
		case VK_HOME:
			if (global::editor.editor_graph().current_curve()) {
				global::window_grapheditor.send_command((WPARAM)WindowCommand::Fit);
			}
			break;

			// [A]
		case 65:
			global::config.set_align_handle(!global::config.get_align_handle());
			break;

			// [Delete]
		case VK_DELETE:
			global::window_toolbar.send_command((WPARAM)WindowCommand::Clear);
			break;
		}
	}


	// ウィンドウプロシージャ
	BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		using WindowMessage = AviUtl::FilterPlugin::WindowMessage;
		static ActCtxManager actctx_manager;
		RECT rect_wnd;
		::GetClientRect(hwnd, &rect_wnd);

		switch (message) {
			// ウィンドウ作成時
		case WindowMessage::Init:
			global::editp = editp;

			actctx_manager.init(fp->dll_hinst);

			// WS_CLIPCHILDRENを追加
			::SetWindowLongPtrA(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);

			// Alpha版・Beta版の場合はキャプションを変更
			if (global::PLUGIN_VERSION.is_preview()) {
				::SetWindowTextA(
					hwnd,
					std::format("{} ({})", global::PLUGIN_NAME, global::PLUGIN_VERSION.preview_type().str(true)).c_str()
				);
			}

			global::window_main.create(
				global::fp->dll_hinst,
				hwnd,
				"WINDOW_MAIN",
				"WINDOWCLS_MAIN",
				wndproc_main,
				WS_CHILD,
				NULL,
				rect_wnd
			);
			return 0;

			// AviUtl終了時
		case WindowMessage::Exit:
			global::window_main.close();
			actctx_manager.exit();
			return 0;

		case WM_SIZE:
		{
			RECT rect_wnd_main = rect_wnd;
			rect_wnd_main.right = std::max((int)rect_wnd_main.right, global::MIN_WIDTH);
			rect_wnd_main.bottom = std::max((int)rect_wnd_main.bottom, global::SEPARATOR_WIDTH * 2 + global::MENU_HEIGHT);
			global::window_main.move(rect_wnd_main);
			return 0;
		}

		case WM_KEYDOWN:
			if (global::config.get_enable_hotkeys()) {
				on_keydown(wparam);
			}
			return 0;

		case WM_COMMAND:
			switch (wparam) {
			case (UINT)WindowCommand::RedrawAviutl:
				return TRUE;

			case (UINT)WindowCommand::Update:
				global::window_main.send_command((WPARAM)WindowCommand::Update);
				break;
			}
			return 0;
		}
		return 0;
	}
}